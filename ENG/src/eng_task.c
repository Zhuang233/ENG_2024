#include "eng_task.h"
#include "cmsis_os.h"
#include "can.h"
#include "DJIMotoDriver.h"
#include "LKMotoDriver.h"
#include "pid.h"
#include "test.h"
#include "chassis.h"
#include "relay.h"
#include "usart.h"
#include "RoboArm.h"
#include "uart_zbw.h"
#include "JointReset.h"
#include "RoboArm.h"
#include "RCDriver.h"
#include "chassis.h"
#include "ui.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
#include "referee.h"

//// 事件组句柄
//EventGroupHandle_t xEventGroup;

//// 定义事件标志位
//const EventBits_t ALL_JOINTS_SAFE = (1 << 0);
//const EventBits_t RESET_COMPLETE = (1 << 1);


void just_one_time(void (*func)(void), bool* have_run){
	if(!(*have_run)){
		(*func)();
		*have_run = true;
	}
}

void pull_out(){
	sync_data_to_c.data.qs_pos = 10000;
	osDelay(2000);
	vy = -500;
	osDelay(1500);
	pump_top_close();
	xipan_top_close();
	vy = 0;
}


int zbwtest = 0;

// 单元测试/回归测试用
void TestTask(void const * argument)
{
	
	chassis_pid_init();
	RoboArm_Pos_Init();
	osDelay(1000);
	reset_lift();
	expand_init();
  small_lift_init();

  for(;;)
  {	
		/*---------------------------------------------------

		// 测试电机电流直接控制
		SetMotoCurrent(&hcan1, Ahead, 300, 900, 2700, 8100);
		
		// 瓴控电机速度闭环测试,单位：度/秒（未减速前）
		LKSetSpeed(LK_Motor1_ID, 90000);
		osDelay(10);	
		
		// 位置环测试
		test_pid_pos();
		
		// 遥控控制电机测试
		test_rc_moto();
		
		// 微动开关测试
		test_wd();

		test_reset_qs();
		
		// EC电机测试
		EC_set_motor_position(6, 30.0, 300, 50, 3);
		
		// 底盘控制测试
		//		ChassisTask();
				
		// 测试电机电流直接控制
		//		SetMotoCurrent(&hcan1, Back, zbwtest, 0, 0, 0);
		//		osDelay(1);
		
		-----------------------------------------------------*/

//	RoboArm_RC_Ctrl_Fixed_Point();
	RoboArm_RC_Ctrl();
	Update_Lift_Pos();
	Update_Expand_Pos();
	Update_Small_Lift_Pos();
	Update_Flip_Pos();
//				MotoState[0].speed_desired = -vy + vx + vr;		// 1  right front
//				MotoState[1].speed_desired =  vy + vx + vr;		// 2  left front
//				MotoState[2].speed_desired =  vy - vx + vr;		// 3	left back
//				MotoState[3].speed_desired = -vy - vx + vr;		// 4	right back  
//				chassis_control();
//		ChassisTask();
	osDelay(1);
  }
}


// 电机任务函数
void MotoTask(void const * argument)
{
	osDelay(3000);
	reset_expand();
  for(;;)
  {
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



#define VIRTUAL_LINK 1
#define FETCH_SLIVER_1 2
#define FETCH_SLIVER_2 3
#define FETCH_SLIVER_3 4
#define RC_FLIP 5

uint8_t flip_mode = VIRTUAL_LINK;
void FlipTask(void const * argument){
	flip_init();
  for(;;)
  {
		// 函数形式，外部控制翻转就行
		if(flip_mode == VIRTUAL_LINK){
			MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		}
		// 取银矿高度和伸展
		else if(flip_mode == FETCH_SLIVER_1){
			MotoState[6].angle_desired = 315000;
			MotoState[5].angle_desired = -200000;
			osDelay(2000);
			MotoState[7].angle_desired = -427240;
		}
		// 开泵吸取、抬高
		else if(flip_mode == FETCH_SLIVER_2){
			xipan_left_open();
			xipan_right_open();
			xipan_middle_open();
			pump_bottom_open();
			osDelay(5000);
			MotoState[6].angle_desired = 450000;
			osDelay(1000);
			MotoState[5].angle_desired = -170000;
		}
		// 靠紧、函数回收
		
		

		else if(flip_mode == FETCH_SLIVER_3){
			MotoState[7].angle_desired = -10000;
			MotoState[5].angle_desired = -10000;
			MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		}
		else if(flip_mode == RC_FLIP){
				MotoState[5].angle_desired += RC_CtrlData.rc.ch4/5;
				if(MotoState[5].angle_desired > 0) MotoState[5].angle_desired = 0;
				if(MotoState[5].angle_desired < -200000) MotoState[5].angle_desired = -200000;
		}
    osDelay(1); 
  }
}


bool lock_ctrl = true;

void CtrlLockTask(void const * argument){
  for(;;)
  {

		lock_ctrl = true;
		osDelay(500);
		if(lock_ctrl == true){
			custom_controller_data_t.vx = 0.0f;
			custom_controller_data_t.vy = 0.0f;
			custom_controller_data_t.vz = 0.0f;
		}
		osDelay(1);
	}
}

#define WINDOW_FRONT_POS_MODE 1
#define WINDOW_DEEP_POS_MODE 2
#define WINDOW_DEEP_UP_POS_MODE 3
#define OUTWINDOW_MODE 4
uint8_t auto_fetch_stage = 0;
bool flag1 = false;
void AutoFetchTask(void const * argument){
	
  for(;;)
  {
		switch(auto_fetch_stage){
			case WINDOW_FRONT_POS_MODE:
			{
				sync_data_to_c.data.qs_pos = 10000;
				sync_data_to_c.data.hy_pos = -194951;
				MotoState[4].angle_desired = -1723474;
				sync_data_to_c.data.theta1 = 28492;
				sync_data_to_c.data.theta2 = 25823;
				sync_data_to_c.data.theta3 = 24370;
			}
			break;
			case WINDOW_DEEP_POS_MODE:
			{
				sync_data_to_c.data.qs_pos = 785267;
				pump_top_open();
				xipan_top_open();
			}
			break;
			case WINDOW_DEEP_UP_POS_MODE:
			{
				MotoState[4].angle_desired = -1575355;
				sync_data_to_c.data.theta1 = 28492;
			}
			break;
			case OUTWINDOW_MODE:
			{
				just_one_time(pull_out,&flag1);
			}
			break;
		}
	
		osDelay(1);
	}


}



void TimeCtrlTask(void const * argument){
  for(;;)
  {
		if(RC_CtrlData.rc.sw2 == 2){
			auto_fetch_stage ++;
			if(auto_fetch_stage == 1) osDelay(1000);
			if(auto_fetch_stage == 2) osDelay(2000);
			if(auto_fetch_stage == 3) osDelay(1000);
			if(auto_fetch_stage == 4) osDelay(4000);
		}
		else {
			auto_fetch_stage = 0;
			flag1 = false;
		}
	
		osDelay(1);
	}

}

void UITask(void const * argument){
	 for(;;)
  {
		ui_init_gold_island_middle();
		osDelay(500);
	}

}

