#include "eng_task2.h"
// #include "portmacro.h"
#include "cmsis_os2.h"
#include "ui.h"
#include "cmsis_os.h"
#include "chassis.h"
#include "DJIMotoDriver.h"
#include "uart_zbw.h"
#include "RoboArm.h"
#include "Key.h"
#include "RcDriver.h"
#include "JointReset.h"
#include "relay.h"
#include "referee.h"
#include "trace.h"
#include "modes.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "wd.h"

int8_t zbw_test = 0;
extern uint8_t pump_flag;
void UITask(void const * argument){
	ui_queue = xQueueCreate(128, sizeof(ui_msg_t));
	ui_msg_t ui_msg_buffer;
	for(;;)
  	{
		// xQueueReceive(ui_queue, &ui_msg_buffer, portMAX_DELAY);
		// HAL_UART_Transmit(&huart7, ui_msg_buffer.ui_msg, ui_msg_buffer.len, 100);
		osDelay(100);
	}
}

void LiftTask(void const * argument){
	osDelay(1000);
	reset_lift();
  for(;;)
  {	
	Update_Lift_Pos();
    osDelay(1);
  }
}

void DataSyncAnCTask(void const * argument){
	sync_data_to_c_init();
	osDelay(500);
	sync_data_to_c.data.head = 0x55;
	sync_data_to_c.data.tail = 0xaa;
  for(;;)
  {
	data_sync_uart();
    osDelay(10); 
  }
}

// 底盘任务
//1.控制数据接收与处理
//2.pid解算
//3.给电流
void ChassisMotoTask(void const * argument){
	 chassis_pid_init();
	 for(;;)
  {
		trace_angle(MotoState[7].angle);
		chassis_control_RC();
		for(int i=0;i<4;i++){
			pid_calculate(&chassis_pid_spd_moto[i], MotoState[i].speed_desired, MotoState[i].speed_actual);
		}
		SetMotoCurrent(&hcan1,Ahead,chassis_pid_spd_moto[0].outPID,
																chassis_pid_spd_moto[1].outPID,
																chassis_pid_spd_moto[2].outPID,
																chassis_pid_spd_moto[3].outPID);
		osDelay(1);
	}
}

struct Pose_offest{
	int32_t qs;
	int32_t hy;
	int32_t lift;
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
	int32_t flip;
	int32_t small_lift;
	int32_t expand;
}pose_offest;


#ifdef OLD_CAR
#define HY_STEP 2000
#define LIFT_STEP 8000
#define QS_STEP 4000
#define FLIP_STEP 2000
#else
#define HY_STEP 600
#define LIFT_STEP 2400
#define QS_STEP 1200
#define FLIP_STEP 200
#endif
void pose_offest_clear(void){
	pose_offest.hy = 0;
	pose_offest.qs = 0;
	pose_offest.lift = 0;
	pose_offest.pitch = 0;
	pose_offest.roll = 0;
	pose_offest.yaw = 0;
	pose_offest.flip = 0;
	pose_offest.small_lift = 0;
	pose_offest.expand = 0;
}



bool pitch_rotate_slow_flag = false;
bool roll_rotate_slow_flag = false;
bool yaw_rotate_slow_flag = false;

uint16_t pitch_slow =  ARM_ANGLE_STD_1;
uint16_t roll_slow = ARM_ANGLE_STD_2;
uint16_t yaw_slow = ARM_ANGLE_STD_3;

void RotationSlowTask(void const * argument){
	for(;;){
		if(pitch_rotate_slow_flag){
				if(sync_data_to_c.data.theta1 > pitch_slow + 65) sync_data_to_c.data.theta1-=30;
				if(sync_data_to_c.data.theta1 < pitch_slow - 65) sync_data_to_c.data.theta1+=30;
		}
		if(roll_rotate_slow_flag){
			if(sync_data_to_c.data.theta2 > roll_slow + 65) sync_data_to_c.data.theta2-=30;
			if(sync_data_to_c.data.theta2 < roll_slow - 65) sync_data_to_c.data.theta2+=30;
		}
		if(yaw_rotate_slow_flag){
			if(sync_data_to_c.data.theta3 > yaw_slow + 65) sync_data_to_c.data.theta3-=30;
			if(sync_data_to_c.data.theta3 < yaw_slow - 65) sync_data_to_c.data.theta3+=30;
		}
		osDelay(1);
	}
}



extern uint8_t key_2_last;
extern float gx,gy,gz;

# ifdef OLD_CAR
bool virtual_link_flag = true;
void FlipTask(void const * argument){
	expand_init();
  small_lift_init();
	flip_init();
	reset_expand();
	for(;;)
  {
		Update_Flip_Pos();
		Update_Small_Lift_Pos();
		Update_Expand_Pos();
    osDelay(1); 
  }
}


// 姿态控制任务，所有固定姿态由此处理,UI需要显示
void ModePoseTask(void const * argument){
__packed int32_t* qs = &(sync_data_to_c.data.qs_pos);
__packed int32_t* hy = &(sync_data_to_c.data.hy_pos);
	int32_t* lift = &(MotoState[4].angle_desired);
//	uint16_t* pitch = &(sync_data_to_c.data.theta1);
//	uint16_t* roll = &(sync_data_to_c.data.theta2);
//	uint16_t* yaw = &(sync_data_to_c.data.theta3);
	uint16_t* pitch = &(pitch_slow);
	uint16_t* roll = &(roll_slow);
	uint16_t* yaw = &(yaw_slow);
	int32_t* flip = &(MotoState[5].angle_desired);
	int32_t* small_lift = &(MotoState[6].angle_desired);
	int32_t* expand = &(MotoState[7].angle_desired);
	
	rotateslow_flag = true;
	bool first_time_fetch_sliver_store_left = true;
	
	RoboArm_Pos_Init();
	Key_Init();
	osDelay(1000);
	for(;;){
		if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_F)){
			pump_top_open();
		}
		if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_G)){
			pump_top_close();
		}
			
		if(posemod != NONE){
			pose_auto = true;
		}
		switch(posemod){
			case PARA_FIND:{
				osDelay(1);
			}break;
			case NONE:{
				// 回默认位置
				*qs = 0;
				*hy = -194951;
				*lift = -1273474;
				*pitch = 12590;
				*roll = 25823;
				*yaw = 24700;
				*expand = 0;
				
//				pump_top_close();
//				xipan_top_close();
//				pump_bottom_close();
//				xipan_left_close();
//				xipan_right_close();
				pose_auto = false;
				
				// 3种模式入口
				if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_Z)){
					posemod = FETCH_GOLD_INIT;
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_X)){
					posemod = SINGEL_SLIVER_INIT;
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_C)){
					posemod = FETCH_SLIVER_INIT;
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_V)){
					posemod = SELECT_EXCANGE_MODE;
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_B)){
					posemod = TAKE_GROUND_ORE_INIT;
					pose_offest_clear();
				}
				
			} break;
			
			case SINGEL_SLIVER_INIT:{
				rotatesfaster = true;
				*lift = -1203474 + pose_offest.lift;
				*hy = -194951 + pose_offest.hy;
				*pitch = 28770;
				*qs = 25000 + pose_offest.qs;
				osDelay(5);
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					rotatesfaster = false;
					posemod = SINGEL_SLIVER_BACK;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					rotatesfaster = false;
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
			} break;
			
			case SINGEL_SLIVER_BACK:{
				pump_top_open();
				xipan_top_open();
				osDelay(2500);
				*lift = -803474 + pose_offest.lift;
				osDelay(1500);
				*pitch = 12590;
				posemod = NONE;
			}break;
			
			case FETCH_GOLD_INIT:{
				// 中金初始姿态
				*qs = 0;
				*hy = -194951 + pose_offest.hy;
				*lift = -1773474 + pose_offest.lift;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
				
			} break;
			case FETCH_GOLD_INDEEP: {
				*qs = 550000 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1773474 + pose_offest.lift;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					pump_top_open();
					xipan_top_open();
					osDelay(3000);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INIT;
					osDelay(300);
				}
			
			}break;
			case FETCH_GOLD_INDEEP_UP:{
				*qs = 550000 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1650000+ pose_offest.lift;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_OUT;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					pump_top_close();
					xipan_top_close();
					posemod = FETCH_GOLD_INDEEP;
					osDelay(3000);
				}
			} break;

			case FETCH_GOLD_OUT:{
				*qs = 0 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1650000+ pose_offest.lift;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = NONE;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					osDelay(300);
				}
				
			} break;
			case FETCH_GOLD_STORE_LEFT:{
				// 向上抬 左转					
				*qs = 261649;
				*hy = -215000;
				*lift = -1000000;
				*roll = 25823;
				*pitch = 44950;
				*yaw = 42754;
				osDelay(2000);
					
				// 开底泵
				pump_bottom_open();
				xipan_left_open();
				osDelay(500);
				// 下压
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(2000);
				// 关上泵
				pump_top_close();
				xipan_top_close();
				osDelay(2500);
				// 上抬转回来
				*lift = *lift + 55.0 * 500000.0/87.5;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				// 降到初始位置
				osDelay(3000);
				*lift = -1723474;
				posemod = NONE;
			} break;
			case FETCH_GOLD_STORE_RIGHT:{
				// 向上抬 右转	
				*qs = 191649;
				*hy = -190000;
				*lift = -1000000;
				*roll = 25823;
				*pitch = 44950;
				*yaw = 7986;
				osDelay(500);
				
				// 开底泵
				pump_bottom_open();
				xipan_right_open();
				
				// 下压
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(800);
				
				// 关上泵
				pump_top_close();
				xipan_top_close();
				osDelay(2500);
				
				// 上抬转回来
				*lift = *lift + 55.0 * 500000.0/87.5;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				
				// 降到初始位置
				osDelay(3000);
				*lift = -1723474;
				posemod = NONE;
			} break;
			case FETCH_SLIVER_INIT:{
				*qs = 25000;
				*hy = -194951;
				*lift = 0;
				*pitch = ARM_ANGLE_STD_1;
				*roll = ARM_ANGLE_STD_2;
				*yaw = ARM_ANGLE_STD_3;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_SLIVER_FLIP;
					virtual_link_flag = true;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
				
			} break;
			case FETCH_SLIVER_FLIP:{
				// 伸出去，抬到最顶
				*qs = 330 * 390000.0/294.0;
				*hy = -194951;
				*lift = 0;
				*pitch = ARM_ANGLE_MAX_1;
				*roll = ARM_ANGLE_STD_2;
				*yaw = ARM_ANGLE_STD_3;
				*flip = -170000;
				osDelay(1500);
				*expand = -427240;
				osDelay(1000);
				
				virtual_link_flag = false;
				osDelay(30);
				pump_bottom_open();
				xipan_left_open();
				xipan_right_open();
				
				// 取双边银矿标准姿态
				*small_lift = 325000;
				*flip = -220000;
				osDelay(3000); // 吸3s
				
				// 拔起
				*small_lift = 410000;
				osDelay(800);
				
				*flip = -150000;
				osDelay(1000);
		
				virtual_link_flag = true;
				*expand = 0;
				osDelay(300);
				*flip = 0;
			
				posemod = NONE;
			} break;
			case FETCH_SLIVWER_STORE_LEFT:{
				*qs = 160*780000/388;
				*hy = -194951;
				if(first_time_fetch_sliver_store_left){
					*lift = 0;
					rotatesfaster = true;
					osDelay(1000);
					*roll = 25823;
					*pitch = 44950;	
					*yaw = 40754;
					osDelay(2000);
					rotatesfaster = false;
					*lift = *lift - (50.0 * 500000.0/87.5 + 500000.0);
					first_time_fetch_sliver_store_left = false;
				}
				else{
					if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_V)){
						posemod = SELECT_EXCANGE_MODE;
						pose_offest_clear();
						osDelay(300);
					}
				}
			} break;
			case TAKE_GROUND_ORE_INIT:{
				rotatesfaster = true;
				*lift = -1773474 + pose_offest.lift;
				*hy = -194951 + pose_offest.hy;
				*qs = 780000 + pose_offest.qs;
				*yaw = 24700 + pose_offest.yaw;
				*pitch = 44950 + pose_offest.pitch;
				*roll = 25823;
				
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					rotatesfaster = false;
					posemod = TAKE_GROUND_ORE_TAKE_BACK;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					rotatesfaster = false;
					posemod = NONE;
					osDelay(300);
				}
			}break;
			case TAKE_GROUND_ORE_TAKE_BACK:{
				
				// 开泵下压
				*lift = -1873474;
				pump_top_open();
				xipan_top_open();
				osDelay(2500);
				
				// 抬起上翻
				*lift = -1273474;
				*hy = -194951;
				osDelay(1000);
				*pitch = 12590;
				*yaw = 24700;
				osDelay(1000);
				*qs = 0;
				posemod = NONE;
			
			}break;
			
			case SELECT_EXCANGE_MODE:{	
				// 上吸盘兑换 F
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Press(&Keys.KEY_F)){
						*lift = -1400000;
						*qs = 0;
						*hy = -194951;
						*pitch = 28492;
						*roll = ARM_ANGLE_STD_2;
						*yaw = ARM_ANGLE_STD_3;
					
					
						*qs = 98042;
						*hy = -162500;
						*lift = -1009038;
						*pitch = 28492;
						*roll = 25823;
						*yaw = 24370; 
//						key_2_last = custom_controller_data_t.key_2;	
						osDelay(3500);
						rotateslow_flag = false;
					// 兑换循环	
					gx=280,gy=350.0,gz=500.0;
					for(;;){
						// RoboArm_RC_Ctrl_Fixed_Point(); 
						SubArm_Ctrl();
						if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_S)) {
							rotateslow_flag = true;
							break; 
						}
						osDelay(1);
					}	
					posemod = NONE;
				}
				
				// 左吸盘兑换 C
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Press(&Keys.KEY_C)){	
				// 向上抬 左转	
				rotatesfaster = true;					
				*qs = 361649;
				*hy = -215000;
				*lift = -1000000;
				*roll = 25823;
				*pitch = 44950;
				*yaw = 42754;
				osDelay(500);
					
				// 开上泵
				pump_top_open();
				xipan_top_open();
	
				// 下压
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(1000);
					
				// 关底泵
				pump_bottom_close();
				xipan_left_close();
				rotatesfaster = false;
				osDelay(2500);
				// 上抬转回来
				* qs = 98042;
				*hy = -162500;
				*lift = -1009038;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24370;  
				osDelay(2500);
//				key_2_last = custom_controller_data_t.key_2;	
					// 兑换循环
					rotateslow_flag = false;	
					gx=280,gy=350.0,gz=500.0;				
					for(;;){	
						
						RoboArm_RC_Ctrl_Fixed_Point(); 
						if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_S)) {
							rotateslow_flag = true;
							break; 
						}
						osDelay(1);
					}
					posemod = NONE;
				}
				
				// 右吸盘兑换 V
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Press(&Keys.KEY_V)){
				// 向上抬 右转	
				rotatesfaster = true;
				*qs = 291649;
				*hy = -190000;
				*lift = -1000000;
				*roll = 59069;
				*pitch = 12590;
				*yaw = 7986;
				osDelay(500);
					
				// 开上泵
				pump_top_open();
				xipan_top_open();
	
				// 下压
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(1000);
					
				// 关底泵
				pump_bottom_close();
				xipan_right_close();
				rotatesfaster = false;
				osDelay(2500);
				// 上抬转回来
				*lift = -1009038;
				osDelay(300);
				*roll = 25823;
				osDelay(1500);
				*qs = 98042;
				*hy = -162500;
				*pitch = 28492;
				*yaw = 24370;
				osDelay(1000);
//				key_2_last = custom_controller_data_t.key_2;	
					// 兑换循环
					rotateslow_flag = false;		
					gx=280,gy=350.0,gz=500.0;
					for(;;){
						RoboArm_RC_Ctrl_Fixed_Point(); 
						if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_S)){
							rotateslow_flag = true;
							break;
						} 
						osDelay(1);
					}
					posemod = NONE;
				}
				// 误触退出兑换选择
				if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = NONE;
				}
			} break;
		}
		osDelay(10);
	}
}


// 一键金矿模式
void VirtualLinkTask(void const * argument){
	for(;;){
		if(virtual_link_flag){
			MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		}
		osDelay(10);
	}
}
#else
void SecondArmTask(void *argument)
{
	osDelay(3000); //复位得等3s 2006上电没那么快工作
	reset_small_lift();
	SMALL_LIFT = SMALL_LIFT_MIN;
	reset_small_qs();
	while (!lift_inited) {
		Update_Small_Lift_Pos();
	 	osDelay(1);
	}
	reset_small_yaw();
	reset_lift_camera();
  for(;;)
  {
		Update_Small_Qs_Pos();
		Update_Small_Lift_Pos();
		Update_Small_Yaw_Pos();
		Update_Lift_Camera_Pos();
    osDelay(1);
  }
}

#endif


#ifdef OLD_CAR
#define NEED_LIFT_OFFSET (posemod == FETCH_GOLD_INIT \
											 || posemod == FETCH_GOLD_INDEEP \
										 	 || posemod == FETCH_GOLD_INDEEP_UP \
											 || posemod == FETCH_GOLD_OUT \
)

#define NEED_HY_OFFSET (posemod == FETCH_GOLD_INIT \
											 || posemod == FETCH_GOLD_INDEEP \
										 	 || posemod == FETCH_GOLD_INDEEP_UP \
											 || posemod == FETCH_GOLD_OUT \
)

#define NEED_QS_OFFSET (posemod == FETCH_GOLD_INIT \
											 || posemod == FETCH_GOLD_INDEEP \
											 || posemod == FETCH_GOLD_INDEEP_UP \
											 || posemod == FETCH_GOLD_OUT \
)
#endif


void OffsetTask(void const * argument){
	for(;;){
		#ifdef OLD_CAR
		if(NEED_LIFT_OFFSET){
			if(Key_Check_Press(&Keys.KEY_W)) pose_offest.lift += LIFT_STEP;
			if(Key_Check_Press(&Keys.KEY_S)) pose_offest.lift -= LIFT_STEP;
		}
		if(NEED_HY_OFFSET){
			if(Key_Check_Press(&Keys.KEY_A)) pose_offest.hy += HY_STEP;
			if(Key_Check_Press(&Keys.KEY_D)) pose_offest.hy -= HY_STEP;
		}
		if(NEED_QS_OFFSET){
			if(Key_Check_Press(&Keys.KEY_Q)) pose_offest.qs += QS_STEP;
			if(Key_Check_Press(&Keys.KEY_E)) pose_offest.qs -= QS_STEP;
		}
		#else
		if(posemod != NONE && posemod != STORE_INIT){
			if(Key_Check_Hold(&Keys.KEY_W)) LIFT += LIFT_STEP;
			if(LIFT > LIFT_MAX) LIFT = LIFT_MAX;
			if(Key_Check_Hold(&Keys.KEY_S)) LIFT -= LIFT_STEP;
			if(LIFT < LIFT_MIN) LIFT = LIFT_MIN;
			if(Key_Check_Hold(&Keys.KEY_A)) HY += HY_STEP;
			if(Key_Check_Hold(&Keys.KEY_D)) HY -= HY_STEP;
			if(Key_Check_Hold(&Keys.KEY_Q)) QS += QS_STEP;
			if(Key_Check_Hold(&Keys.KEY_E)) QS -= QS_STEP;
		}

		#endif
		osDelay(20);
	}
}

// 机器调试模式（检录用）
bool debug_mode = false;
int xipan_mode = 0;
void DebugModeTask(void const * argument){
	for(;;){
//		if(Key_Check_Hold(&Keys.KEY_Z)){
//			xipan_left_open();
//		}
//		else{
//			xipan_left_close();
//		}
//		if(Key_Check_Hold(&Keys.KEY_X) || xipan_mode == 2){
//			xipan_right_open();
//		}
//		else{
//			xipan_right_close();
//		}
//		if(Key_Check_Hold(&Keys.KEY_C) || xipan_mode == 3){
//			xipan_top_open();
//		}
//		else{
//			xipan_top_close();
//		}
//		if(Key_Check_Hold(&Keys.KEY_V) || xipan_mode == 4){
//			xipan_bottom_open();
//		}
//		else{
//			xipan_bottom_close();
//		}
		osDelay(1);
	}
}
//void DebugModeTask(void const * argument){
//	for(;;){
//		//ctrl+shift+r防误触进入调试模式
//		if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Hold(&Keys.KEY_SHIFT) && Key_Check_Press(&Keys.KEY_R)){
//			debug_mode = !debug_mode;
//		}
//		
//		if(debug_mode == true){
//			//改ui
//			if(Key_Check_Hold(&Keys.KEY_W)) MotoState[4].angle_desired += 1950/5;
//			if(Key_Check_Hold(&Keys.KEY_S)) MotoState[4].angle_desired -= 1950/5;
//			if(Key_Check_Hold(&Keys.KEY_A))	sync_data_to_c.data.hy_pos += 400/5;
//			if(Key_Check_Hold(&Keys.KEY_D))	sync_data_to_c.data.hy_pos -=	400/5;
//			if(Key_Check_Hold(&Keys.KEY_Q)) sync_data_to_c.data.qs_pos +=	780/5;			
//			if(Key_Check_Hold(&Keys.KEY_E)) sync_data_to_c.data.qs_pos -=	780/5;
//			if(Key_Check_Hold(&Keys.KEY_X)) 
//			{
//				pump_top_open();
//				xipan_top_open();
//			}
//			else{
//				pump_top_close();
//				xipan_top_close();
//			}
//			
//			if(Key_Check_Hold(&Keys.KEY_Z)) 
//			{
//				pump_bottom_open();
//				xipan_left_open();
//			}
//			else{
//				xipan_left_close();
//			}
//			
//			if(Key_Check_Hold(&Keys.KEY_C)) 
//			{
//				pump_bottom_open();
//				xipan_right_open();
//			}
//			else{
//				xipan_right_close();
//			}
//			
//			if((!Key_Check_Hold(&Keys.KEY_C)) && (!Key_Check_Hold(&Keys.KEY_Z)))
//				pump_bottom_close();
//		}
//		osDelay(1);
//	}
//}

//void PosLimitTask(void const * argument){
//	
//	for(;;){
//		if(sync_data_to_c.data.theta1 > ARM_ANGLE_MAX_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MAX_1;
//		if(sync_data_to_c.data.theta1 < ARM_ANGLE_MIN_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MIN_1;
//		if(sync_data_to_c.data.theta2 > ARM_ANGLE_MAX_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MAX_2;
//		if(sync_data_to_c.data.theta2 < ARM_ANGLE_MIN_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MIN_2;
//		if(sync_data_to_c.data.theta3 > ARM_ANGLE_MAX_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MAX_3;
//		if(sync_data_to_c.data.theta3 < ARM_ANGLE_MIN_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MIN_3;
//		if(sync_data_to_c.data.qs_pos > QS_ANGLE_MAX) sync_data_to_c.data.qs_pos = QS_ANGLE_MAX;
//		if(sync_data_to_c.data.qs_pos < QS_ANGLE_MIN) sync_data_to_c.data.qs_pos = QS_ANGLE_MIN;
//		if(sync_data_to_c.data.hy_pos > HY_ANGLE_MAX) sync_data_to_c.data.hy_pos = HY_ANGLE_MAX;
//		if(sync_data_to_c.data.hy_pos < HY_ANGLE_MIN) sync_data_to_c.data.hy_pos = HY_ANGLE_MIN;
//		if(MotoState[4].angle_desired > LIFT_MAX) MotoState[4].angle_desired = LIFT_MAX;
//		if(MotoState[4].angle_desired < LIFT_MIN) MotoState[4].angle_desired = LIFT_MIN;
//		
//		if(MotoState[5].angle_desired > FLIP_MAX) MotoState[5].angle_desired = FLIP_MAX;
//		if(MotoState[5].angle_desired < FLIP_MIN) MotoState[5].angle_desired = FLIP_MIN;
//		
//		if(MotoState[6].angle_desired > SMALL_LIFT_MAX) MotoState[6].angle_desired = SMALL_LIFT_MAX;
//		if(MotoState[6].angle_desired < SMALL_LIFT_MIN) MotoState[6].angle_desired = SMALL_LIFT_MIN;
//		
//		if(MotoState[7].angle_desired > EXPAND_MAX) MotoState[7].angle_desired = EXPAND_MAX;
//		if(MotoState[7].angle_desired < EXPAND_MIN) MotoState[7].angle_desired = EXPAND_MIN;
//		osDelay(1);
//	}

//}
