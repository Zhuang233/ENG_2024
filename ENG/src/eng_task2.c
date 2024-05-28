#include "eng_task2.h"
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

int8_t zbw_test = 0;


void UITask(void const * argument){
	 for(;;)
  {	
		osDelay(100);
		
	}

}

void LiftTask(void const * argument)
{
	osDelay(1000);
	reset_lift();
  for(;;)
  {	
		Update_Lift_Pos();
    osDelay(1);
  }
}

void DataSyncAnCTask(void const * argument){
	osDelay(500);
  for(;;)
  {
		data_sync_uart();
    osDelay(1); 
  }
}

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


// 底盘任务
//1.控制数据接收与处理
//2.pid解算
//3.给电流
void ChassisMotoTask(void const * argument){
	 chassis_pid_init();
	 for(;;)
  {
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






typedef enum{
	VIRTUAL_LINK,
	// 取金矿，全过程应均可手动微调，以及回退（左键确认，右键回退）
	DEFAULT_INIT_NO_CLOSE, // 不关泵回默认姿态
	SWITCH_MODE, // 功能切换入口
	DEFAULT_INIT,// 关泵回默认姿态
	NONE, // 只管无泵
	FETCH_GOLD_INIT,   // 预备态ok
	FETCH_GOLD_INDEEP, // 深入ok
	FETCH_GOLD_INDEEP_UP,// 抬高 ok
	FETCH_GOLD_OUT, // 取出ok
	FETCH_GOLD_STORE_LEFT, // 存矿左ok
	FETCH_GOLD_STORE_RIGHT, // 存矿右 (歪)
	FETCH_GOLD_INIT_LEFT, // 左预备态 8
	FETCH_GOLD_INDEEP_LEFT, // 左深入 ok
	FETCH_GOLD_INDEEP_UP_LEFT,// 左抬高 ok
	FETCH_GOLD_OUT_LEFT, // 左取出 ok
	
	FETCH_SLIVER_INIT, // 上抬升 12
	FETCH_SLIVER_FLIP,		// 翻转吸
	FETCH_SLIVER_BACK1,		// 翻回存
	FETCH_SLIVER_BACK2,		// 翻回存2
	FETCH_SLIVER_TOP_DOWN, // 顶部吸盘降
	FETCH_SLIVWER_STORE_LEFT, //存
	SELECT_EXCANGE_MODE, //选择兑矿模式
}PoseMode;

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

PoseMode posemod = NONE;
bool pose_auto = false; // 给其他任务的通知全局变量
//#define VIRTUAL_LINK 1
//#define FETCH_SLIVER_1 2
//#define FETCH_SLIVER_2 3
//#define FETCH_SLIVER_3 4
//#define RC_FLIP 5

//#define HY_STEP_FETCH_GOLD_INIT 10000
//#define LIFT_STEP_FETCH_GOLD_INIT 40000
//#define QS_STEP_FETCH_GOLD_INIT 20000
//#define FLIP_STEP_OFFSET 10000

#define HY_STEP 2000
#define LIFT_STEP 8000 //(+12个)
#define QS_STEP 4000
#define FLIP_STEP 2000


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



bool rotateslow_flag = false;
uint16_t pitch_slow =  ARM_ANGLE_STD_1;
uint16_t roll_slow = ARM_ANGLE_STD_2;
uint16_t yaw_slow = ARM_ANGLE_STD_3;
void RotationSlowTask(void const * argument){
	for(;;){
		if(rotateslow_flag){
			if(sync_data_to_c.data.theta1 > pitch_slow + 20) sync_data_to_c.data.theta1-=10;
			if(sync_data_to_c.data.theta2 > roll_slow + 20) sync_data_to_c.data.theta2-=10;
			if(sync_data_to_c.data.theta3 > yaw_slow + 20) sync_data_to_c.data.theta3-=10;
			if(sync_data_to_c.data.theta1 < pitch_slow - 20) sync_data_to_c.data.theta1+=10;
			if(sync_data_to_c.data.theta2 < roll_slow - 20) sync_data_to_c.data.theta2+=10;
			if(sync_data_to_c.data.theta3 < yaw_slow - 20) sync_data_to_c.data.theta3+=10;
			osDelay(1);
		}
	}
}


bool virtual_link_flag = true;
// 姿态控制任务，所有固定姿态由此处理,UI需要显示
void ModePoseTask(void const * argument){
	int32_t* qs = &(sync_data_to_c.data.qs_pos);
	int32_t* hy = &(sync_data_to_c.data.hy_pos);
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
	bool first_time_fetch_sliver_flip = true;
	bool first_time_fetch_sliver_back = true;
	bool first_time_fetch_sliver_store_left = true;
	
	RoboArm_Pos_Init();
	Key_Init();
	osDelay(1000);
	for(;;){
		if(posemod != NONE){
			pose_auto = true;
		}
		switch(posemod){
			case SWITCH_MODE:{
				// 3种模式入口
				if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_Z)){
					posemod = FETCH_GOLD_INIT;
					ui_remove_gold_island_sliver();
					osDelay(200);
					ui_init_gold_island_middle();
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_X)){
					posemod = FETCH_GOLD_INIT_LEFT;
					// 加UI
					pose_offest_clear();
				}
				else if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_C)){
					posemod = FETCH_SLIVER_INIT;
					ui_remove_gold_island_middle();
					osDelay(200);
					ui_init_gold_island_sliver();
					pose_offest_clear();
				}
			}
			
			// 不关泵 + 初始位
			case DEFAULT_INIT_NO_CLOSE:{
				*qs = 0;
				*hy = -194951;
				*lift = -1773474;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				posemod = SWITCH_MODE;
			}
			// 关泵 + 初始位
			case DEFAULT_INIT:{
				*qs = 0;
				*hy = -194951;
				*lift = -1773474;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				posemod = NONE;
			}
			case NONE:{
				// 全关 但位置未知
				pump_top_close();
				xipan_top_close();
				pump_bottom_close();
				xipan_left_close();
				xipan_right_close();
				pose_auto = false;
				posemod = SWITCH_MODE;
			} break;
			
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
					posemod = DEFAULT_INIT_NO_CLOSE;
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
					posemod = FETCH_GOLD_STORE_LEFT;
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r == 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					osDelay(300);
				}
				
			} break;
			case FETCH_GOLD_STORE_LEFT:{
				*qs = 160*780000/388;
				*hy = -194951;
				*lift = -1000000;
				*roll = 25823;
				*pitch = 44950;
				*yaw = 40754;
				osDelay(2000);
				pump_bottom_open();
				xipan_left_open();
				osDelay(500);
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(2000);
				pump_top_close();
				xipan_top_close();
				osDelay(2500);
				*lift = *lift + 55.0 * 500000.0/87.5;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				osDelay(3000);
				*lift = -1723474;
				posemod = DEFAULT_INIT_NO_CLOSE;
			} break;
			
			
			case FETCH_GOLD_STORE_RIGHT:{
				*qs = 160*780000/388;
				*hy = -200000;
				*lift = -1000000;
				*roll = 25823;
				*pitch = 44950;
				*yaw = 7986;
				osDelay(2000);
				pump_bottom_open();
				xipan_right_open();
				osDelay(500);
				*lift = *lift - 55.0 * 500000.0/87.5;
				osDelay(2000);
				pump_top_close();
				xipan_top_close();
				osDelay(2500);
				*lift = *lift + 55.0 * 500000.0/87.5;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				osDelay(3000);
				*lift = -1723474;
				posemod = DEFAULT_INIT_NO_CLOSE;
			} break;
//			
//			case FETCH_GOLD_INIT_LEFT:{
//				*qs = 2000;
//				*hy = -194951;
//				*lift = -1723474-50000;
//				*pitch = 28492 - 10.0 * 65535.0/360.0;
//				*roll = 25823 + 65535.0/4.0;
//				*yaw = 24700;
//				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
//					posemod = FETCH_GOLD_INDEEP_LEFT;
//					pose_offest_clear();
//					osDelay(300);
//				}
//				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
//					posemod = NONE;
//					pose_offest_clear();
//					osDelay(300);
//				}
//			} break;
//				
//			case FETCH_GOLD_INDEEP_LEFT:{
//				if(Key_Check_Press(&Keys.KEY_A)){
//					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_D)){
//					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_W)){
//					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_S)){
//					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_Q)){
//					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_E)){
//					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
//				}
//				*qs = 310000 + pose_offest.qs;
//				*hy = -194951 + 20.0*390000.0/294.0+ pose_offest.hy;
//				*lift = -1723474-50000 + pose_offest.lift;
//				*pitch = 28492 - 10.0 * 65535.0/360.0;
//				*roll = 25823 + 65535.0/4.0;
//				*yaw = 24700;
//				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
//					posemod = FETCH_GOLD_INDEEP_UP_LEFT;
//					pump_top_open();
//					xipan_top_open();
//					pose_offest_clear();
//					osDelay(3000);
//				}
//				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
//					posemod = FETCH_GOLD_INIT_LEFT;
//					pose_offest_clear();
//					osDelay(300);	
//				}
//			} break;
//			case FETCH_GOLD_INDEEP_UP_LEFT:{
//				if(Key_Check_Press(&Keys.KEY_A)){
//					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_D)){
//					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_W)){
//					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_S)){
//					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_Q)){
//					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_E)){
//					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
//				}
//				*qs = 310000 + pose_offest.qs;
//				*hy = -194951 + 20.0*390000.0/294.0+ pose_offest.hy;
//				*lift = -1600000 -50000+ pose_offest.lift;
//				*pitch = 28492 - 10.0 * 65535.0/360.0;
//				*roll = 25823 + 65535.0/4.0;
//				*yaw = 24700;
//				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
//					posemod = FETCH_GOLD_OUT_LEFT;
//					pose_offest_clear();
//					osDelay(300);
//				}
//				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
//					posemod = FETCH_GOLD_INDEEP_LEFT;
//					pose_offest_clear();
//					osDelay(300);	
//				}
//			} break;
//			
//			case FETCH_GOLD_OUT_LEFT: {
//				if(Key_Check_Press(&Keys.KEY_A)){
//					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_D)){
//					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_W)){
//					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_S)){
//					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_Q)){
//					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
//				}
//				if(Key_Check_Press(&Keys.KEY_E)){
//					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
//				}
//				*qs = 0 + pose_offest.qs;
//				*hy = -194951+ pose_offest.hy;
//				*lift = -1570000-50000 + pose_offest.lift;
//				*pitch = 28492 - 10.0 * 65535.0/360.0;
//				*roll = 25823 + 65535.0/4.0;
//				*yaw = 24700;
//				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
//					posemod = FETCH_GOLD_STORE_RIGHT;
//					pose_offest_clear();
//					osDelay(300);
//				}
//				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
//					posemod = FETCH_GOLD_INDEEP_UP_LEFT;
//					pose_offest_clear();
//					osDelay(300);	
//				}
//			}break;
			case FETCH_SLIVER_INIT:{
				*qs = 2000;
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
				if(first_time_fetch_sliver_flip){
					*qs = 330 * 390000.0/294.0;
					*hy = -194951;
					*lift = 0;
					*pitch = ARM_ANGLE_MAX_1;
					*roll = ARM_ANGLE_STD_2;
					*yaw = ARM_ANGLE_STD_3;
					*flip = -200000;
					osDelay(2000);
					first_time_fetch_sliver_flip = false;
				}

				virtual_link_flag = false;
				pump_bottom_open();
				xipan_left_open();
				xipan_right_open();
				*small_lift = 325000;
				*flip = -220000;
				*expand = -427240;
				
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					first_time_fetch_sliver_flip = true;
					posemod = FETCH_SLIVER_BACK1;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					first_time_fetch_sliver_flip = true;
					posemod = FETCH_SLIVER_INIT;
					pose_offest_clear();
					osDelay(300);
				}
				
			} break;
			case FETCH_SLIVER_BACK1:{
				*qs = 330 * 390000.0/294.0;
				*hy = -194951;
				*lift = 0;
				*pitch = ARM_ANGLE_MAX_1;
				*roll = ARM_ANGLE_STD_2;
				*yaw = ARM_ANGLE_STD_3;

				if(first_time_fetch_sliver_back){
					*flip = -200000;
					first_time_fetch_sliver_back = false;
				}
				
				*small_lift = 410000;
				*expand = -427240;
				osDelay(1000);
				*flip = -150000;
			
				
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					first_time_fetch_sliver_back = true;
					posemod = FETCH_SLIVER_BACK2;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					first_time_fetch_sliver_back = true;
					posemod = FETCH_SLIVER_FLIP;
					pose_offest_clear();
					osDelay(300);
				}
				
			} break;
			case FETCH_SLIVER_BACK2:{
				*lift = 0;
				osDelay(1500);
				*qs = 330 * 390000.0/294.0;
				*hy = -194951;
				*pitch = ARM_ANGLE_MAX_1;
				*roll = ARM_ANGLE_STD_2;
				*yaw = ARM_ANGLE_STD_3;
				
				virtual_link_flag = true;
				*expand = 0;
				*flip = -5000;
			
				
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_SLIVER_TOP_DOWN;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_SLIVER_BACK1;
					pose_offest_clear();
					osDelay(300);
				}
			} break;
			case FETCH_SLIVER_TOP_DOWN:{
				*qs = 330 * 390000.0/294.0;
				*hy = -194951;
				*lift = -780000;
				pump_top_open();
				xipan_top_open();
				*pitch = ARM_ANGLE_MAX_1;
				*roll = ARM_ANGLE_STD_2;
				*yaw = ARM_ANGLE_STD_3;
				
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_SLIVWER_STORE_LEFT;
					pose_offest_clear();
					osDelay(5000);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_SLIVER_BACK2;
					pose_offest_clear();
					osDelay(300);
				}
			} break;
			case FETCH_SLIVWER_STORE_LEFT:{
				*qs = 160*780000/388;
				*hy = -194951;
				if(first_time_fetch_sliver_store_left){
					*lift = 0;
					osDelay(1000);
					*roll = 25823;
					*pitch = 44950;
					*yaw = 40754;
					osDelay(2000);
					*lift = *lift - (50.0 * 500000.0/87.5 + 500000.0);
					first_time_fetch_sliver_store_left = false;
				}
				else{
					posemod = SELECT_EXCANGE_MODE;
					pose_offest_clear();
					osDelay(300);
				}
			} break;
			case SELECT_EXCANGE_MODE:{
				zbw_test = 1;
				// 上吸盘兑换
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Hold(&Keys.KEY_F)){
					// 先到一个合适的兑换初始位（待标定）
//						*lift = 0;
//						*qs = 330 * 390000.0/294.0;
//						*hy = -194951;
//						*pitch = ARM_ANGLE_MAX_1;
//						*roll = ARM_ANGLE_STD_2;
//						*yaw = ARM_ANGLE_STD_3;
//						osDelay(2000);
					
					// 兑换循环 
					for(;;){
						RoboArm_RC_Ctrl_Fixed_Point(); 
						if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_S)) break; 
						osDelay(1);
					}	
					// 兑换完去默认姿态但是别关泵
					posemod = DEFAULT_INIT_NO_CLOSE;
				}
				
				// 左吸盘
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Hold(&Keys.KEY_C)){
				
				
				}
				
				
				// 右吸盘
				if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Hold(&Keys.KEY_V)){
				
				
				}
			} break;
			
			
		}
		osDelay(1);
	}
}
// 一键金矿模式



void VirtualLinkTask(void const * argument){
	for(;;){
		if(virtual_link_flag){
			MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		}
		osDelay(1);
	}

}




// 机器调试模式（检录用）
bool debug_mode = false;
void DebugModeTask(void const * argument){
	for(;;){
		//ctrl+shift+v防误触进入调试模式
		if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Hold(&Keys.KEY_SHIFT) && Key_Check_Press(&Keys.KEY_V)){
			debug_mode = !debug_mode;
		}
		
		if(debug_mode == true){
			//改ui
			if(Key_Check_Hold(&Keys.KEY_W)) MotoState[4].angle_desired += 1950/5;
			if(Key_Check_Hold(&Keys.KEY_S)) MotoState[4].angle_desired -= 1950/5;
			if(Key_Check_Hold(&Keys.KEY_A))	sync_data_to_c.data.hy_pos += 400/5;
			if(Key_Check_Hold(&Keys.KEY_D))	sync_data_to_c.data.hy_pos -=	400/5;
			if(Key_Check_Hold(&Keys.KEY_Q)) sync_data_to_c.data.qs_pos +=	780/5;			
			if(Key_Check_Hold(&Keys.KEY_E)) sync_data_to_c.data.qs_pos -=	780/5;
			if(Key_Check_Hold(&Keys.KEY_X)) 
			{
				pump_top_open();
				xipan_top_open();
			}
			else{
				pump_top_close();
				xipan_top_close();
			}
			
			if(Key_Check_Hold(&Keys.KEY_Z)) 
			{
				pump_bottom_open();
				xipan_left_open();
			}
			else{
				xipan_left_close();
			}
			
			if(Key_Check_Hold(&Keys.KEY_C)) 
			{
				pump_bottom_open();
				xipan_right_open();
			}
			else{
				xipan_right_close();
			}
			
			if((!Key_Check_Hold(&Keys.KEY_C)) && (!Key_Check_Hold(&Keys.KEY_Z)))
				pump_bottom_close();
		}
		osDelay(1);
	}
}

void PosLimitTask(void const * argument){
	
	for(;;){
		if(sync_data_to_c.data.theta1 > ARM_ANGLE_MAX_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MAX_1;
		if(sync_data_to_c.data.theta1 < ARM_ANGLE_MIN_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MIN_1;
		if(sync_data_to_c.data.theta2 > ARM_ANGLE_MAX_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MAX_2;
		if(sync_data_to_c.data.theta2 < ARM_ANGLE_MIN_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MIN_2;
		if(sync_data_to_c.data.theta3 > ARM_ANGLE_MAX_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MAX_3;
		if(sync_data_to_c.data.theta3 < ARM_ANGLE_MIN_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MIN_3;
		if(sync_data_to_c.data.qs_pos > QS_ANGLE_MAX) sync_data_to_c.data.qs_pos = QS_ANGLE_MAX;
		if(sync_data_to_c.data.qs_pos < QS_ANGLE_MIN) sync_data_to_c.data.qs_pos = QS_ANGLE_MIN;
		if(sync_data_to_c.data.hy_pos > HY_ANGLE_MAX) sync_data_to_c.data.hy_pos = HY_ANGLE_MAX;
		if(sync_data_to_c.data.hy_pos < HY_ANGLE_MIN) sync_data_to_c.data.hy_pos = HY_ANGLE_MIN;
		if(MotoState[4].angle_desired > LIFT_MAX) MotoState[4].angle_desired = LIFT_MAX;
		if(MotoState[4].angle_desired < LIFT_MIN) MotoState[4].angle_desired = LIFT_MIN;
		
		if(MotoState[5].angle_desired > FLIP_MAX) MotoState[5].angle_desired = FLIP_MAX;
		if(MotoState[5].angle_desired < FLIP_MIN) MotoState[5].angle_desired = FLIP_MIN;
		
		if(MotoState[6].angle_desired > SMALL_LIFT_MAX) MotoState[6].angle_desired = SMALL_LIFT_MAX;
		if(MotoState[6].angle_desired < SMALL_LIFT_MIN) MotoState[6].angle_desired = SMALL_LIFT_MIN;
		
		if(MotoState[7].angle_desired > EXPAND_MAX) MotoState[7].angle_desired = EXPAND_MAX;
		if(MotoState[7].angle_desired < EXPAND_MIN) MotoState[7].angle_desired = EXPAND_MIN;
		osDelay(1);
	}

}





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



void OffsetTask(void const * argument){
	for(;;){
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
		osDelay(1);
	}
}