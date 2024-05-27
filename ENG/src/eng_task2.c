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



void UITask(void const * argument){
	 for(;;)
  {
		ui_init_gold_island_middle();
		osDelay(500);
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
	NONE,
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
	FETCH_SLIVWER_STORE_LEFT //存
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
#define HY_STEP_FETCH_GOLD_INIT 10000
#define LIFT_STEP_FETCH_GOLD_INIT 40000
#define QS_STEP_FETCH_GOLD_INIT 20000
#define FLIP_STEP_OFFSET 10000


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
			if(sync_data_to_c.data.theta1 > pitch_slow + 50) sync_data_to_c.data.theta1-=25;
			if(sync_data_to_c.data.theta2 > roll_slow + 50) sync_data_to_c.data.theta2-=25;
			if(sync_data_to_c.data.theta3 > yaw_slow + 50) sync_data_to_c.data.theta3-=25;
			if(sync_data_to_c.data.theta1 < pitch_slow - 50) sync_data_to_c.data.theta1+=25;
			if(sync_data_to_c.data.theta2 < roll_slow - 50) sync_data_to_c.data.theta2+=25;
			if(sync_data_to_c.data.theta3 < yaw_slow - 50) sync_data_to_c.data.theta3+=25;
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
			case NONE:{
				pump_top_close();
				xipan_top_close();
				pump_bottom_close();
				xipan_left_close();
				xipan_right_close();
				pose_auto = false;
			} break;
			case FETCH_GOLD_INIT:{
				*qs = 2000;
				*hy = -194951;
				*lift = -1723474+70000;
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
				
			} break;
			case FETCH_GOLD_INDEEP: {
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 550000 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1723474+70000 + pose_offest.lift;
				*pitch = 29500;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					pump_top_open();
					xipan_top_open();
					pose_offest_clear();
					osDelay(3000);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INIT;
					pose_offest_clear();
					osDelay(300);	
				}
			
			}break;
			case FETCH_GOLD_INDEEP_UP:{
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 550000 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1600000 +70000+ pose_offest.lift;
				*pitch = 28000;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_OUT;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP;
					pose_offest_clear();
					osDelay(300);	
				}
			} break;
			case FETCH_GOLD_OUT:{
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 0 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1600000 +70000+ pose_offest.lift;
				*pitch = 28000;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_STORE_LEFT;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					pose_offest_clear();
					osDelay(300);	
				}
				
			} break;
			case FETCH_GOLD_STORE_LEFT:{
				*qs = 160*780000/388;
				*hy = -200000;
				*lift = -1000000;
				osDelay(1000);
//				for(; *roll > 25823;(*roll)-=17){
//					osDelay(1);
//				}
				*roll = 25823;
				*pitch = 44950;
				*yaw = 40754;
				pump_bottom_open();
				xipan_left_open();
				osDelay(500);
				*lift = *lift - 50.0 * 500000.0/87.5;
				osDelay(2000);
				pump_top_close();
				xipan_top_close();
				osDelay(2000);
				*lift = *lift + 50.0 * 500000.0/87.5;
				osDelay(500);
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				osDelay(500);
				*lift = -1723474;
				posemod = FETCH_GOLD_INIT;
			} break;
			case FETCH_GOLD_STORE_RIGHT:{
				*qs = 160*780000/388;
				*hy = -200000;
				*lift = -1000000;
				osDelay(1000);
//				for(; *roll > 25823;(*roll)-=17){
//					osDelay(1);
//				}
				*roll = 25823;
				*pitch = 44950;
				*yaw = 7986;
				pump_bottom_open();
				xipan_right_open();
				osDelay(500);
				*lift = *lift - 50.0 * 500000.0/87.5;
				osDelay(2000);
				pump_top_close();
				xipan_top_close();
				osDelay(2000);
				*lift = *lift + 50.0 * 500000.0/87.5;
				osDelay(500);
				*pitch = 28492;
				*roll = 25823;
				*yaw = 24700;
				osDelay(500);
				*lift = -1723474;
				posemod = FETCH_GOLD_INIT;
			} break;
			
			case FETCH_GOLD_INIT_LEFT:{
				*qs = 2000;
				*hy = -194951;
				*lift = -1723474+70000;
				*pitch = 28492 - 10.0 * 65535.0/360.0;
				*roll = 25823 + 65535.0/4.0;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP_LEFT;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
			} break;
				
			case FETCH_GOLD_INDEEP_LEFT:{
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 310000 + pose_offest.qs;
				*hy = -194951 + 20.0*390000.0/294.0+ pose_offest.hy;
				*lift = -1723474+70000 + pose_offest.lift;
				*pitch = 28492 - 10.0 * 65535.0/360.0;
				*roll = 25823 + 65535.0/4.0;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_INDEEP_UP_LEFT;
					pump_top_open();
					xipan_top_open();
					pose_offest_clear();
					osDelay(3000);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INIT_LEFT;
					pose_offest_clear();
					osDelay(300);	
				}
			} break;
			case FETCH_GOLD_INDEEP_UP_LEFT:{
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 310000 + pose_offest.qs;
				*hy = -194951 + 20.0*390000.0/294.0+ pose_offest.hy;
				*lift = -1600000 +70000+ pose_offest.lift;
				*pitch = 28492 - 10.0 * 65535.0/360.0;
				*roll = 25823 + 65535.0/4.0;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_OUT_LEFT;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_LEFT;
					pose_offest_clear();
					osDelay(300);	
				}
			} break;
			
			case FETCH_GOLD_OUT_LEFT: {
				if(Key_Check_Press(&Keys.KEY_A)){
					pose_offest.hy += HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_D)){
					pose_offest.hy -= HY_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_W)){
					pose_offest.lift += LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_S)){
					pose_offest.lift -= LIFT_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_Q)){
					pose_offest.qs += QS_STEP_FETCH_GOLD_INIT;
				}
				if(Key_Check_Press(&Keys.KEY_E)){
					pose_offest.qs -= QS_STEP_FETCH_GOLD_INIT;
				}
				*qs = 0 + pose_offest.qs;
				*hy = -194951+ pose_offest.hy;
				*lift = -1570000+70000 + pose_offest.lift;
				*pitch = 28492 - 10.0 * 65535.0/360.0;
				*roll = 25823 + 65535.0/4.0;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = FETCH_GOLD_STORE_RIGHT;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_UP_LEFT;
					pose_offest_clear();
					osDelay(300);	
				}
			}break;
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





//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"


//// 事件组句柄
//EventGroupHandle_t xEventGroup;

//// 定义事件标志位
//const EventBits_t ALL_JOINTS_SAFE = (1 << 0);
//const EventBits_t RESET_COMPLETE = (1 << 1);


//qs 22cm
//pitch min
//lift 


