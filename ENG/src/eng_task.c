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


int zbwtest = 0;

union RV_TypeConvert
{
	float to_float;
	int to_int;
	unsigned int to_uint;
	uint8_t buf[4];
}rv_type_convert;

// 底盘任务
//1.控制数据接收与处理
//2.pid解算
//3.给电流
void ChassisTask_test(void)
{
	chassis_control();
	for(int i=0;i<4;i++){
		pid_calculate(&chassis_pid_spd_moto[i], MotoState[i].speed_desired, MotoState[i].speed_actual);
  }
	SetMotoCurrent(&hcan1,Ahead,chassis_pid_spd_moto[0].outPID,
															chassis_pid_spd_moto[1].outPID,
															chassis_pid_spd_moto[2].outPID,
															chassis_pid_spd_moto[3].outPID);
}

//void EC_set_motor_position(uint16_t motor_id,float pos,uint16_t spd,uint16_t cur,uint8_t ack_status)
//{
//	
//	CAN_TxHeaderTypeDef motor_pos_setting_tx_message;
//	uint8_t motor_pos_setting_can_send_data[8];
//	uint32_t send_mail_box;
//	motor_pos_setting_tx_message.StdId = motor_id;
//	motor_pos_setting_tx_message.RTR = CAN_RTR_DATA;
//	motor_pos_setting_tx_message.IDE = CAN_ID_STD;
//	motor_pos_setting_tx_message.DLC = 8;
//	
//	/*报文返回状态只有0到3*/
//	if(ack_status>3) 	return;
//	
//	rv_type_convert.to_float=pos;
//  motor_pos_setting_can_send_data[0]=0x20|(rv_type_convert.buf[3]>>3);
//	motor_pos_setting_can_send_data[1]=(rv_type_convert.buf[3]<<5)|(rv_type_convert.buf[2]>>3);
//	motor_pos_setting_can_send_data[2]=(rv_type_convert.buf[2]<<5)|(rv_type_convert.buf[1]>>3);
//	motor_pos_setting_can_send_data[3]=(rv_type_convert.buf[1]<<5)|(rv_type_convert.buf[0]>>3);
//	motor_pos_setting_can_send_data[4]=(rv_type_convert.buf[0]<<5)|(spd>>10);
//	motor_pos_setting_can_send_data[5]=(spd&0x3FC)>>2;
//	motor_pos_setting_can_send_data[6]=(spd&0x03)<<6|(cur>>6);
//	motor_pos_setting_can_send_data[7]=(cur&0x3F)<<2|ack_status;
//	
//	HAL_CAN_AddTxMessage(&hcan1,&motor_pos_setting_tx_message,motor_pos_setting_can_send_data,&send_mail_box); 
//}



// 单元测试/回归测试用
void TestTask(void const * argument)
{
	
	pidInit(&pidtest, 10000, 10000, 20, 0, 0);
	test_pid_pos_init();
	chassis_pid_init();
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
		
		// 横移复位测试
		test_reset_hy();
		
		test_reset_qs();
		
		// EC电机测试
		EC_set_motor_position(6, 30.0, 300, 50, 3);
		
		-----------------------------------------------------*/
		// 底盘控制测试
		ChassisTask_test();
		
	
	osDelay(1);

  }
}


// 电机任务函数
void Mototask(void const * argument)
{
  for(;;)
  {
    osDelay(1); 
  }
}


void LedTask(void const * argument)
{
  for(;;)
  {
     HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
		 osDelay(500);
		 HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
		 osDelay(500);
  }
}


