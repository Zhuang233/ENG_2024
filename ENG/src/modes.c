#include "modes.h"
#include "main.h"
#include <stdbool.h>
#include "RoboArm.h"
#include "uart_zbw.h"
#include "cmsis_os.h"
#include "relay.h"
#include "chassis.h"
#include "key.h"
#include "RcDriver.h"


extern uint8_t power_less_flag;
extern bool pitch_rotate_slow_flag;
extern bool roll_rotate_slow_flag;
extern bool yaw_rotate_slow_flag;

extern uint16_t pitch_slow;
extern uint16_t roll_slow;
extern uint16_t yaw_slow;


bool posemode_change_flag = false;
PoseMode posemod = NONE;
PoseMode last_posemod = NONE;
bool pose_auto = false; // 给其他任务的通知全局变量	

bool lift_greater(int target){
	return LIFT_READ > target;
}
//bool qs_greater(int target){
//	return QS_READ > target;
//}

void wait_until(bool (*condition)(int), int arg){
	while(!condition(arg)){
		osDelay(1);
	}
}


void chassis_back(uint16_t ms, int16_t speed){
		chassis_auto_ctrl_flag = 1;
		vy = -speed;
		osDelay(ms);
		vy = 0;
		chassis_auto_ctrl_flag = 0;
}

void NONE_loop(){

}

void FETCH_GOLD_INIT_loop(){

}

void FETCH_GOLD_AUTO_loop(){

}

void FETCH_SLIVER_AUTO_loop(){

}

void DEBUG_loop(){

}

void FETCH_SLIVER_INIT_loop(){

}

void Before_FETCH_GOLD_INIT(){
		// 双臂摆到合适位置
	LIFT = -1720000;
	HY = -330000;
	SMALL_LIFT = 80000;
	SMALL_YAW = 50000;
	osDelay(1000);
	sync_data_to_c.data.theta1 = 16000;
}

void Before_FETCH_GOLD_AUTO(){
	sync_data_to_c.data.qs_pos = 480000;
	SMALL_QS = 400000;
	xipan_top_open();
	xipan_bottom_open();
	osDelay(1000);
	SMALL_LIFT = 300000;
	LIFT = -1580000;
	osDelay(500);
	sync_data_to_c.data.qs_pos = 0;
	SMALL_QS = 0;
	osDelay(500);
	// 自动后退
	chassis_back(800,2000);
}

void Before_FETCH_SLIVER_INIT(){
		// 显示小资源岛对齐ui
}


void Before_FETCH_SLIVER_AUTO(){
		SMALL_YAW = SMALL_YAW_MAX;
		LIFT = -80000;
		// 自动后退,摆好位置
		chassis_back(550, 500);
		osDelay(500);
		QS = 570000;
		HY = -11200;
		ROLL = ROLL_STD;
		YAW = YAW_STD;
//		wait_until(qs_greater, 300000);
		PITCH = PITCH_DOWN;
		
		osDelay(300);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -80000;
		osDelay(750);
		
		QS = 409000;
		HY = -193000;
//		YAW = YAW_RIGHT;
		yaw_rotate_slow_flag = true;
		yaw_slow = YAW_RIGHT;
		xipan_right_open();
		osDelay(1000);
		yaw_rotate_slow_flag = false;
		
		xipan_top_close();
		
		osDelay(700);
			
		// 摆第二个位置
		YAW = YAW_STD;
		HY = -364000;
		QS = 570000;
		
		osDelay(300);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -80000;
		osDelay(750);
		
		QS = 427000;
		HY = -189000;
//		YAW = YAW_LEFT;
		yaw_rotate_slow_flag = true;
		yaw_slow = YAW_LEFT;
		xipan_left_open();
		osDelay(1000);
		yaw_rotate_slow_flag = false;
		
		xipan_top_close();
		
		osDelay(700);
		
		// 摆第三个位置
		YAW = YAW_STD;
		HY = -186000;
		QS = 570000;
		
		osDelay(300);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -80000;
		osDelay(750);
		
		chassis_auto_ctrl_flag = 1;
		vy = -1000;
		osDelay(550);
		vy = 0;
		chassis_auto_ctrl_flag = 0;
//		PITCH = PITCH_UP;
		pitch_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		QS = 0;
		LIFT = -1273474;
		osDelay(1500);
		pitch_rotate_slow_flag = false;
}

// 模式入口单次函数
void Before_Next_Mode(){
	if(posemod == last_posemod) return;
	if(posemod == DEBUG) power_less_flag = 1;
	if(posemod == FETCH_GOLD_INIT) Before_FETCH_GOLD_INIT();
	if(posemod == FETCH_GOLD_AUTO) Before_FETCH_GOLD_AUTO();	
	if(posemod == FETCH_SLIVER_INIT) Before_FETCH_SLIVER_INIT();
	if(posemod == FETCH_SLIVER_AUTO) Before_FETCH_SLIVER_AUTO();
}




// 模式出口清理函数
void Exit_Last_Mode(){
	if(last_posemod == NONE) return;
	else if(posemod == DEBUG){
		power_less_flag = 0;
	}
	else if(last_posemod == FETCH_GOLD_INIT) {
		if(posemod == NONE){
			// 取金矿退回NONE
		}
	}
	else if(last_posemod == FETCH_SLIVER_INIT) {
		if(posemod == NONE){
			// 取银初始化退回NONE，清理银UI
		}
	}
}


// 模式切换管理
void ModeManageTask(void const * argument){
	for(;;){
		// ctrl + xxx
		if(Key_Check_Hold(&Keys.KEY_CTRL) && (RC_CtrlData.key.info != Keys.KEY_CTRL.code)){ //按下ctrl且还按了其他的
			if(Key_Check_Press(&Keys.KEY_Z)){
				last_posemod = posemod;
				posemod = FETCH_GOLD_INIT;
				posemode_change_flag = true;
			}
			if(Key_Check_Press(&Keys.KEY_X)){
				last_posemod = posemod;
				posemod = FETCH_SLIVER_INIT;
				posemode_change_flag = true;
			}
			
			if(Key_Check_Press(&Keys.KEY_B)){
				last_posemod = posemod;
				posemod = NONE;
				posemode_change_flag = true;
			}
			if(Key_Check_Press(&Keys.KEY_Q)){
				last_posemod = posemod;
				posemod = DEBUG;
				posemode_change_flag = true;
			}
			
		}
		
		//左键确认
		if(RC_CtrlData.mouse.press_l == 1 && RC_CtrlData.mouse.last_press_l == 0){
			if(posemod == FETCH_GOLD_INIT){
				last_posemod = posemod;
				posemod = FETCH_GOLD_AUTO;
				posemode_change_flag = true;
			}
			if(posemod == FETCH_SLIVER_INIT){
				last_posemod = posemod;
				posemod = FETCH_SLIVER_AUTO;
				posemode_change_flag = true;
			}
		}
		osDelay(1);
	}
}


// 模式执行统一逻辑
void ModeExecTask(void const * argument){
	for(;;){
		Before_Next_Mode();
		
		while(!posemode_change_flag){
			if(posemod == NONE)										NONE_loop();
			else if(posemod == FETCH_GOLD_INIT) 	FETCH_GOLD_INIT_loop();
			else if(posemod == FETCH_SLIVER_INIT) FETCH_SLIVER_INIT_loop();
			else if(posemod == FETCH_SLIVER_AUTO) FETCH_SLIVER_AUTO_loop();
			else if(posemod == FETCH_GOLD_AUTO) 	FETCH_GOLD_AUTO_loop();
			else if(posemod == DEBUG) 						DEBUG_loop();
			osDelay(1);
		}
		
		posemode_change_flag = false;
		Exit_Last_Mode();
		
	}		
}