#include "eng_task.h"


//#include "FreeRTOS.h"
//#include "task.h"
//#include "main.h"
#include "cmsis_os.h"

#include "can.h"
#include "DJIMotoDriver.h"
#include "LKMotoDriver.h"
#include "pid.h"
//#include "trace.h"
#include "test.h"
#include "chassis.h"
#include "usart.h"
#include "RoboArm.h"
#include "uart_zbw.h"
#include "JointReset.h"
#include "RoboArm.h"



int zbwtest = 0;

// 单元测试/回归测试用
void TestTask(void const * argument)
{
	
//	pidInit(&pidtest, 10000, 10000, 20, 0, 0);
//	test_pid_pos_init();
//	chassis_pid_init();
//	RoboArm_Pos_Init();
//	osDelay(1000);
//	reset_lift();
//	TestDataUnion test;
//	uint8_t aaa[3]={0x2f,0x42,0x40};
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


//	test.data.a = MotoState[5].angle;
//	test.data.b = MotoState[6].angle;
//	HAL_UART_Transmit(&huart6, test.bytes, 4, 100);
//	HAL_UART_Transmit(&huart6, test.bytes+4, 4, 100);
//	HAL_UART_Transmit(&huart6, aaa, 3, 100);
		
		-----------------------------------------------------*/
		// 底盘控制测试
//		ChassisTask();
		
		// 抬升测试
//		test_lift();
		// 写一个pid测试用

//	
//		osDelay(1);
		// 测试电机电流直接控制
//		SetMotoCurrent(&hcan1, Back, zbwtest, 0, 0, 0);
//		osDelay(1);
	
//	ChassisTask();


	RoboArm_RC_Ctrl();
	Update_Lift_Pos();
	Update_Expand_Pos();
	Update_Small_Lift_Pos();



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
//		SetMotoCurrent(&hcan2, Ahead, 1000, 0, 0, 0);
  }
}


void LedTask(void const * argument)
{
  for(;;)
  {
//     HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
//		 osDelay(500);
//		 HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
//		 osDelay(500);
  }
}

void DataSyncAnCTask(void const * argument){
  for(;;)
  {
//		data_sync_uart();
    osDelay(1); 
  }
}
