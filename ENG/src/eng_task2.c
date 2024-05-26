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
	FETCH_GOLD_INIT,   // 预备态
	FETCH_GOLD_INDEEP, // 深入
	FETCH_GOLD_INDEEP_UP,// 抬高 
	FETCH_GOLD_OUT, // 取出
	FETCH_SLIVER_1, // 
	FETCH_SLIVER_2,
	FETCH_SLIVER_3
}PoseMode;

struct Pose_offest{
	int32_t qs;
	int32_t hy;
	int32_t lift;
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
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


void pose_offest_clear(void){
	pose_offest.hy = 0;
	pose_offest.qs = 0;
	pose_offest.lift = 0;
	pose_offest.pitch = 0;
	pose_offest.roll = 0;
	pose_offest.yaw = 0;

}

// 姿态控制任务，所有固定姿态由此处理,UI需要显示
void ModePoseTask(void const * argument){
	int32_t* qs = &(sync_data_to_c.data.qs_pos);
	int32_t* hy = &(sync_data_to_c.data.hy_pos);
	int32_t* lift = &(MotoState[4].angle_desired);
	uint16_t* pitch = &(sync_data_to_c.data.theta1);
	uint16_t* roll = &(sync_data_to_c.data.theta2);
	uint16_t* yaw = &(sync_data_to_c.data.theta3);
	
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
				pose_auto = false;
			} break;
			case FETCH_GOLD_INIT:{
				*qs = 2000;
				*hy = -194951;
				*lift = -1723474;
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
				*lift = -1723474 + pose_offest.lift;
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
				*lift = -1600000 + pose_offest.lift;
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
				*lift = -1600000 + pose_offest.lift;
				*pitch = 28000;
				*roll = 25823;
				*yaw = 24700;
				if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
					posemod = NONE;
					pose_offest_clear();
					osDelay(300);
				}
				else if(RC_CtrlData.mouse.press_r== 1 && RC_CtrlData.mouse.last_press_r == 0){
					posemod = FETCH_GOLD_INDEEP_UP;
					pose_offest_clear();
					osDelay(300);	
				}
				
			} break;
			case FETCH_SLIVER_1:{
			
			
			} break;
			case FETCH_SLIVER_2:{
			
			} break;
			case FETCH_SLIVER_3:{
			
			} break;
			case VIRTUAL_LINK: {
			
			
			}break;
			
		}
		osDelay(1);
	}
}
// 一键金矿模式





//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"


//// 事件组句柄
//EventGroupHandle_t xEventGroup;

//// 定义事件标志位
//const EventBits_t ALL_JOINTS_SAFE = (1 << 0);
//const EventBits_t RESET_COMPLETE = (1 << 1);





