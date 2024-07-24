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
bool pose_auto = false; // �����������֪ͨȫ�ֱ���	

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
		// ˫�۰ڵ�����λ��
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
	// �Զ�����
	chassis_back(800,2000);
}

void Before_FETCH_SLIVER_INIT(){
		// ��ʾС��Դ������ui
}


void Before_FETCH_SLIVER_AUTO(){
		SMALL_YAW = SMALL_YAW_MAX;
		LIFT = -80000;
		// �Զ�����,�ں�λ��
		chassis_back(550, 500);
		osDelay(500);
		QS = 570000;
		HY = -11200;
		ROLL = ROLL_STD;
		YAW = YAW_STD;
//		wait_until(qs_greater, 300000);
		PITCH = PITCH_DOWN;
		
		osDelay(300);
		
		//�½�����
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
			
		// �ڵڶ���λ��
		YAW = YAW_STD;
		HY = -364000;
		QS = 570000;
		
		osDelay(300);
		
		//�½�����
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
		
		// �ڵ�����λ��
		YAW = YAW_STD;
		HY = -186000;
		QS = 570000;
		
		osDelay(300);
		
		//�½�����
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

// ģʽ��ڵ��κ���
void Before_Next_Mode(){
	if(posemod == last_posemod) return;
	if(posemod == DEBUG) power_less_flag = 1;
	if(posemod == FETCH_GOLD_INIT) Before_FETCH_GOLD_INIT();
	if(posemod == FETCH_GOLD_AUTO) Before_FETCH_GOLD_AUTO();	
	if(posemod == FETCH_SLIVER_INIT) Before_FETCH_SLIVER_INIT();
	if(posemod == FETCH_SLIVER_AUTO) Before_FETCH_SLIVER_AUTO();
}




// ģʽ����������
void Exit_Last_Mode(){
	if(last_posemod == NONE) return;
	else if(posemod == DEBUG){
		power_less_flag = 0;
	}
	else if(last_posemod == FETCH_GOLD_INIT) {
		if(posemod == NONE){
			// ȡ����˻�NONE
		}
	}
	else if(last_posemod == FETCH_SLIVER_INIT) {
		if(posemod == NONE){
			// ȡ����ʼ���˻�NONE��������UI
		}
	}
}


// ģʽ�л�����
void ModeManageTask(void const * argument){
	for(;;){
		// ctrl + xxx
		if(Key_Check_Hold(&Keys.KEY_CTRL) && (RC_CtrlData.key.info != Keys.KEY_CTRL.code)){ //����ctrl�һ�����������
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
		
		//���ȷ��
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


// ģʽִ��ͳһ�߼�
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