#include "test.h"
#include "cmsis_os.h"
#include "DJIMotoDriver.h"
//#include "trace.h"
#include "RcDriver.h"
#include "wd.h"
#include "uart_zbw.h"

#define TEST_PID_POS_P 1
#define TEST_PID_POS_I 0
#define TEST_PID_POS_D 0
#define TEST_PID_SPD_P 1
#define TEST_PID_SPD_I 0
#define TEST_PID_SPD_D 0

PidTD pidtest;
PidTD pid_test_spd;
PidTD pid_test_pos;
MotoStateTD* p_moto_state_test = &MotoState[5];

int current_test;
bool hy_uninit = true;
float qs_reset_speed = 2500;
bool qs_uninit = true;
uint8_t qs_reset_stage = 0;
int32_t qs_angle_max;

MotoStateTD* p_moto_state_test_lift = &MotoState[4];
PidTD pid_test_spd_lift;
PidTD pid_test_pos_lift;



// 0号电机速度环测试
void test_pid_spd(){
	pid_calculate_inc(&pidtest, 0, MotoState[0].speed_actual);
//	trace_pid(&pidtest);
	SetMotoCurrent(&hcan1, Ahead, pidtest.outPID, 0, 0, 0);
	osDelay(1);	
}
		
void test_pid_pos_init(){
	pidInit(&pid_test_pos, 2000, 10000, TEST_PID_POS_P, TEST_PID_POS_I, TEST_PID_POS_D);
	pidInit(&pid_test_spd, 2000, 10000, TEST_PID_SPD_P, TEST_PID_SPD_I, TEST_PID_SPD_D);
	MotoStateInit(p_moto_state_test);
	current_test = 0;
}

// 0号电机测试位置环
void test_pid_pos(){
		pid_calculate(&pid_test_pos, (float)p_moto_state_test->angle_desired , (float)p_moto_state_test->angle);
		p_moto_state_test->speed_desired = (int)pid_test_pos.outPID;
		pid_calculate(&pid_test_spd, p_moto_state_test->speed_desired , p_moto_state_test->speed_actual);
		current_test = (int)pid_test_spd.outPID;
		SetMotoCurrent(&hcan1, Back, 0, current_test, 0 ,0);
	  osDelay(1);
}

void test_rc_moto(){
	MotoState[4].angle_desired += RC_CtrlData.rc.ch2;
	if(MotoState[4].angle_desired > 1950000) MotoState[4].angle_desired = 1950000;
	if(MotoState[4].angle_desired < 0) MotoState[4].angle_desired = 0;
	osDelay(1);
}


// 微动开关测试(短接io和5v c板RGB灯亮)
//void test_wd(){

//	if(hy_reset == true){
//		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
//	}
//	else{
//		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
//	}
//	
//	if(qsn_reset == true){
//		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
//	}
//	else{
//		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
//	}
//	
//	if(qsw_reset == true){
//		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
//	}
//	else{
//		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
//	}
//	osDelay(1);
//}

