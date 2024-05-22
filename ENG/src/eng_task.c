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

	RoboArm_RC_Ctrl_Fixed_Point();
	RoboArm_RC_Ctrl();
	Update_Lift_Pos();
	Update_Expand_Pos();
	Update_Small_Lift_Pos();
	Update_Flip_Pos();
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
			MotoState[7].angle_desired = -427240;
		}
		// 开泵吸取、抬高
		else if(flip_mode == FETCH_SLIVER_2){
			xipan_left_open();
//			xipan_right_open();
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



