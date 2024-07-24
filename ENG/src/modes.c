#include "modes.h"
#include "Key.h"
#include "cmsis_os2.h"
#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include "RoboArm.h"
#include "uart_zbw.h"
#include "cmsis_os.h"
#include "relay.h"
#include "chassis.h"
#include "key.h"
#include "RcDriver.h"
#include "ui.h"
#include "pwm.h"
#include "ui_default_menu2_2.h"
#include "ui_default_menu3_2.h"
#include "string.h"

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

typedef enum{
	AUTO_MODE,
	BY_HAND
}Fetchmode_t;

uint8_t fetch_sliver_num = 1;
Fetchmode_t fetch_sliver_mode = AUTO_MODE;
uint8_t select_stage = 0;
void select_more_ore(){
	ui_default_menu3_select->start_x = 239;
	ui_default_menu3_select->start_y = 832 +15;
	ui_default_menu3_select->end_x = 339;
	ui_default_menu3_select->end_y = 882 +15;
	_ui_update_default_menu3_2();
}

void select_singel_ore(){
	ui_default_menu3_select->start_x = 99;
	ui_default_menu3_select->start_y = 832 +15;
	ui_default_menu3_select->end_x = 199;
	ui_default_menu3_select->end_y = 882 +15;
	_ui_update_default_menu3_2();
}

void select_auto_mode(){
	ui_default_menu2_select->start_x = 99;
	ui_default_menu2_select->start_y = 727 +15;
	ui_default_menu2_select->end_x = 199;
	ui_default_menu2_select->end_y = 777 +15;
	_ui_update_default_menu2_2();
}
void select_by_hand(){
	ui_default_menu2_select->start_x = 239;
	ui_default_menu2_select->start_y = 727 +15;
	ui_default_menu2_select->end_x = 339;
	ui_default_menu2_select->end_y = 777 +15;
	_ui_update_default_menu2_2();
}

bool lift_greater(int target){
	return LIFT_READ > target;
}
bool qs_greater(int target){
	return QS_READ > target;
}
bool pitch_less(int target){
	return PITCH_READ < target;
}
bool small_yaw_less(int target){
	return SMALL_YAW_READ < target;
}

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
void camera_reset(){
	CAMERA_PITCH = CAMERA_PITCH_STD;
	CAMERA_YAW = CAMERA_YAW_STD;
	LIFT_CAMERA = 0;
}

void NONE_loop(){
//	ui_init_default_Menu();
}

void FETCH_GOLD_INIT_loop(){
	if (Key_Check_Press(&Keys.KEY_V)) {
		if(select_stage == 0){
			ui_default_menu3_select->color = 6;
			_ui_update_default_menu3_2();
		}
		else if (select_stage == 1) {
			ui_default_menu2_select->color = 6;
			_ui_update_default_menu2_2();
		}
		select_stage++;
	}

	if(select_stage == 0){
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_sliver_num = 1;
			select_singel_ore();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_sliver_num = 3;
			select_more_ore();
		}
	}
	else if (select_stage == 1) {
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_sliver_mode = AUTO_MODE;
			select_auto_mode();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_sliver_mode = BY_HAND;
			select_by_hand();
		}
	}
}

void FETCH_GOLD_AUTO_loop(){

}

void FETCH_SLIVER_AUTO_loop(){

}

void DEBUG_loop(){

}

void FETCH_SLIVER_INIT_loop(){
	if (Key_Check_Press(&Keys.KEY_V)) {
		if(select_stage == 0){
			ui_default_menu3_select->color = 6;
			_ui_update_default_menu3_2();
		}
		else if (select_stage == 1) {
			ui_default_menu2_select->color = 6;
			_ui_update_default_menu2_2();
		}
		select_stage++;
	}

	if(select_stage == 0){
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_sliver_num = 1;
			select_singel_ore();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_sliver_num = 3;
			select_more_ore();
		}
	}
	else if (select_stage == 1) {
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_sliver_mode = AUTO_MODE;
			select_auto_mode();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_sliver_mode = BY_HAND;
			select_by_hand();
		}
	}
}

void EXCHANGE_INIT_loop(){

}

void EXCHANGE_loop(){
	SubArm_Ctrl();
	if(Key_Check_Press(&Keys.KEY_Q)){
		xipan_top_close();
	}
}

void Before_NONE_INIT(){
	// 显示none模式
	_ui_init_default_Menu_0();

}

void Before_FETCH_GOLD_INIT(){
	//金矿对齐UI
	ui_init_default_GOLD();
	// 双臂摆到合适位置
	SMALL_LIFT = 80000;
	SMALL_YAW = 50000;
	wait_until(small_yaw_less, 70000);
	LIFT = -1720000;
	HY = -330000;
	ROLL = ROLL_STD;
	osDelay(1000);
	sync_data_to_c.data.theta1 = 16000;

	// 显示菜单ui
	strcpy(ui_default_Menu_ModeText->string, "GOLD  ");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();
	ui_init_default_menu2();
	ui_init_default_menu3();
	if(fetch_sliver_num == 1){
		select_singel_ore();
	}
	else{
		select_more_ore();
	}
	select_stage = 0;

	// 双金相机视角
	CAMERA_PITCH = CAMERA_PITCH_GOLD;
	CAMERA_YAW = CAMERA_YAW_GOLD;
	LIFT_CAMERA = LIFT_CAMERA_GOLD_DOBULE;
}

void Before_FETCH_GOLD_AUTO(){
	sync_data_to_c.data.qs_pos = 550000;
	SMALL_QS = 450000;
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
	SMALL_YAW = SMALL_YAW_MAX;
	HY = HY_STD;
	LIFT = LIFT_STD;
	pitch_rotate_slow_flag = true;
	pitch_slow = 0;
	wait_until(pitch_less, 1000);
	pitch_rotate_slow_flag = false;

	// 自动退出
	last_posemod = posemod;
	posemod = NONE;
	posemode_change_flag = true;
}

void Before_FETCH_SLIVER_INIT(){
		// 显示小资源岛对齐ui
		ui_init_default_Sliver();
		// 显示菜单ui
		strcpy(ui_default_Menu_ModeText->string, "SLIVER");
		ui_default_Menu_ModeText->str_length = 6;
		ui_update_default_Menu();
		ui_init_default_menu2();
		ui_init_default_menu3();
		if(fetch_sliver_num == 1){
			select_singel_ore();
		}
		else{
			select_more_ore();
		}
		select_stage = 0;
		// 调整图传视角
		CAMERA_PITCH = CAMERA_PITCH_SLIVER;
		CAMERA_YAW = CAMERA_YAW_SLIVER;
		LIFT_CAMERA = LIFT_CAMERA_SLIVER;
}
void Before_FETCH_SLIVER_AUTO(){
		SMALL_YAW = SMALL_YAW_MAX;
		LIFT = -350000;
		QS = 570000;
		HY = -11200;
		ROLL = ROLL_STD;
		YAW = YAW_STD;
		// 自动后退,摆好位置
		chassis_back(550, 500);
		wait_until(qs_greater, 300000);
		PITCH = PITCH_DOWN;
		
		osDelay(200);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -350000;
		osDelay(750);
		
		QS = 427000;
		HY = -189000;
		yaw_rotate_slow_flag = true;
		yaw_slow = YAW_LEFT;
		xipan_left_open();
		osDelay(1000);
		yaw_rotate_slow_flag = false;
		xipan_top_close();
		osDelay(700);
		
		// 摆第二个位置
		YAW = YAW_STD;
		HY = -364000;
		QS = 570000;
		
		osDelay(200);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -350000;
		osDelay(750);
		
		QS = 409000;
		HY = -193000;
		yaw_rotate_slow_flag = true;
		yaw_slow = YAW_RIGHT;
		xipan_right_open();
		osDelay(1000);
		yaw_rotate_slow_flag = false;
		xipan_top_close();
		osDelay(700);
		
		// 摆第三个位置
		YAW = YAW_STD;
		HY = -186000;
		QS = 570000;
		
		osDelay(200);
		
		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -350000;
		osDelay(750);
		
		chassis_auto_ctrl_flag = 1;
		vy = -1000;
		osDelay(550);
		vy = 0;
		chassis_auto_ctrl_flag = 0;

		pitch_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		QS = 0;
		LIFT = -1273474;
		osDelay(1500);
		pitch_rotate_slow_flag = false;


		// 自动退出
		last_posemod = posemod;
		posemod = NONE;
		posemode_change_flag = true;
}

void Before_EXCHANGE_INIT(){
	strcpy(ui_default_Menu_ModeText->string, "EXCANG");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();
}

void Before_EXCHANGE(){
	// 初始化同步映射
	// wait_until(bool (*condition)(int), int arg)
}

// 模式入口单次函数
void Before_Next_Mode(){
	if(posemod == last_posemod) return;
	if(posemod == DEBUG) power_less_flag = 1;
	if(posemod == NONE) Before_NONE_INIT();
	if(posemod == FETCH_GOLD_INIT) Before_FETCH_GOLD_INIT();
	if(posemod == FETCH_GOLD_AUTO) Before_FETCH_GOLD_AUTO();	
	if(posemod == FETCH_SLIVER_INIT) Before_FETCH_SLIVER_INIT();
	if(posemod == FETCH_SLIVER_AUTO) Before_FETCH_SLIVER_AUTO();
	if(posemod == EXCHANGE_INIT) Before_EXCHANGE_INIT();
	if(posemod == EXCHANGE) Before_EXCHANGE();
}

// 模式出口清理函数
void Exit_Last_Mode(){
	if(last_posemod == NONE) return;
	else if(posemod == DEBUG){
		power_less_flag = 0;
	}
	else if(last_posemod == FETCH_GOLD_INIT) {
		if(posemod == NONE){
			// 取金矿菜单退回NONE
			ui_remove_default_GOLD();
			camera_reset();

		}
	}
	else if(last_posemod == FETCH_GOLD_AUTO) {
		if(posemod == NONE){
			// 取金矿退回NONE，清理金UI
			ui_remove_default_GOLD();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();
			camera_reset();
		}
	}
	else if(last_posemod == FETCH_SLIVER_INIT) {
		if(posemod == NONE){
			// 取银初始化退回NONE，清理银UI
			ui_remove_default_Sliver();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
			ui_remove_default_menu2();
			ui_remove_default_menu3();
			camera_reset();
		}
	}
	else if(last_posemod == FETCH_SLIVER_AUTO) {
		if(posemod == NONE){
			// 取银退回NONE，清理银UI
			ui_remove_default_Sliver();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();
			camera_reset();
		}
	}
	else if(last_posemod == EXCHANGE) {
		if(posemod == NONE){
			LIFT = LIFT_STD;
			PITCH = PITCH_UP;
			ROLL = ROLL_STD;
			YAW = YAW_STD;
			HY = HY_STD;
			QS = QS_STD;

			camera_reset();

			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();
		}
	}
	else if(last_posemod == EXCHANGE_INIT) {
		if(posemod == NONE){
				strcpy(ui_default_Menu_ModeText->string, "NONE  ");
				ui_default_Menu_ModeText->str_length = 6;
				ui_update_default_Menu();

				ui_remove_default_menu2();
				ui_remove_default_menu3();
				camera_reset();
		}
	}
}


// 模式切换管理
void ModeManageTask(void const * argument){
	for(;;){
		// ctrl + xxx
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_Z)){
				last_posemod = posemod;
				posemod = FETCH_GOLD_INIT;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_X)){
				last_posemod = posemod;
				posemod = FETCH_SLIVER_INIT;
				posemode_change_flag = true;
			}
			
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_B)){
				last_posemod = posemod;
				posemod = NONE;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_Q)){
				last_posemod = posemod;
				posemod = DEBUG;
				posemode_change_flag = true;
			}

			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_C)){
				last_posemod = posemod;
				posemod = EXCHANGE_INIT;
				posemode_change_flag = true;
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
			if(posemod == EXCHANGE_INIT){
				last_posemod = posemod;
				posemod = EXCHANGE;
				posemode_change_flag = true;
			}
		}

		if(posemod == NONE){
			if(Key_Check_Hold(&Keys.KEY_Q) && Key_Check_Hold(&Keys.KEY_W)){
				xipan_top_close();
				xipan_bottom_close();
				xipan_left_close();
				xipan_right_close();
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
			if(posemod == FETCH_GOLD_INIT) 	FETCH_GOLD_INIT_loop();
			else if(posemod == FETCH_SLIVER_INIT) 	FETCH_SLIVER_INIT_loop();
			else if(posemod == FETCH_SLIVER_AUTO) 	FETCH_SLIVER_AUTO_loop();
			else if(posemod == FETCH_GOLD_AUTO) 	FETCH_GOLD_AUTO_loop();
			else if(posemod == EXCHANGE_INIT) 		EXCHANGE_INIT_loop();
			else if(posemod == EXCHANGE) 			EXCHANGE_loop();
			else if(posemod == DEBUG) 				DEBUG_loop();
			else if(posemod == NONE)				NONE_loop();
			osDelay(1);
		}
		
		posemode_change_flag = false;
		Exit_Last_Mode();
		osDelay(5);
	}		
}