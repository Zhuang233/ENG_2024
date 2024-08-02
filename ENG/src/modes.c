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
#include "ui_default_exchangeMenu_0.h"
#include "ui_default_exchangeMenu_1.h"
#include "ui_default_exchange_0.h"
#include "ui_default_menu2_2.h"
#include "ui_default_menu3_2.h"
#include "string.h"
#include "ui_interface.h"
#include "referee.h"

typedef enum{
	AUTO_MODE,
	BY_HAND
}Fetchmode_t;
typedef enum{
	TO_EXCHANGE_TOP,
	TO_EXCHANGE_LEFT,
	TO_EXCHANGE_RIGHT,
	TO_EXCHANGE_SIDE
}to_exchange_pos_t;

typedef enum{
	TO_STORE_LEFT,
	TO_STORE_RIGHT
}to_store_pos_t;

typedef enum{
	TO_FETCH_SLIVER_CENTER,
	TO_FETCH_SLIVER_LEFT,
	TO_FETCH_SLIVER_RIGHT
}to_fetch_sliver_t;

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

uint8_t fetch_num = 3;
Fetchmode_t fetch_mode = BY_HAND;
to_exchange_pos_t to_exchange_pos = TO_EXCHANGE_TOP;
to_store_pos_t to_store_pos = TO_STORE_RIGHT;
to_fetch_sliver_t to_fetch_sliver = TO_FETCH_SLIVER_CENTER;

uint8_t select_stage = 0;
bool first_show_SubArm_unreset_warning = true;
bool first_delete_SubArm_unreset_warning = true;

bool sw1_down_flag = false;
uint8_t xipan_debug_open = 1;
//--------------菜单选择框切换函数------------------
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
void select_exchange_top(){
	ui_default_exchangeMenu_selectexchange->start_x = 34;
    ui_default_exchangeMenu_selectexchange->start_y = 755;
    ui_default_exchangeMenu_selectexchange->end_x = 106;
    ui_default_exchangeMenu_selectexchange->end_y = 755 + 50;
}
void select_exchange_right(){
	ui_default_exchangeMenu_selectexchange->start_x = 118;
	ui_default_exchangeMenu_selectexchange->start_y = 755;
	ui_default_exchangeMenu_selectexchange->end_x =118 + 108;
	ui_default_exchangeMenu_selectexchange->end_y =755 + 50;
}
void select_exchange_left(){
	ui_default_exchangeMenu_selectexchange->start_x = 232;
	ui_default_exchangeMenu_selectexchange->start_y = 755;
	ui_default_exchangeMenu_selectexchange->end_x = 232 + 96;
	ui_default_exchangeMenu_selectexchange->end_y = 755 + 50;
}
void select_exchange_side(){
	ui_default_exchangeMenu_selectexchange->start_x = 330;
	ui_default_exchangeMenu_selectexchange->start_y = 755;
	ui_default_exchangeMenu_selectexchange->end_x = 330 + 96;
	ui_default_exchangeMenu_selectexchange->end_y = 755 + 50;
}
//-------------------------------------------------

//--------------条件函数---------------------------
bool lift_greater(int target){
	return LIFT_READ > target;
}
bool qs_greater(int target){
	return QS_READ > target;
}
bool pitch_less(int target){
	return PITCH_READ < target;
}
bool hy_greater(int target){
	return HY_READ > target;
}
bool hy_less(int target){
	return HY_READ < target;
}
bool small_yaw_less(int target){
	return SMALL_YAW_READ < target;
}
bool yaw_close(int target){
	return (YAW_READ > target - 2000 || YAW_READ < target + 2000);
}
bool press_key_F(int none){
	return Key_Check_Press(&Keys.KEY_F);
}

bool one_sliver_selected(int none){
	if(Key_Check_Press(&Keys.KEY_G)){
		if(to_fetch_sliver == TO_FETCH_SLIVER_CENTER){
			to_fetch_sliver = TO_FETCH_SLIVER_LEFT;
			HY = -11200;
		}
		else if(to_fetch_sliver == TO_FETCH_SLIVER_LEFT){
			to_fetch_sliver = TO_FETCH_SLIVER_RIGHT;
			HY = -364000;
		}
		else if(to_fetch_sliver == TO_FETCH_SLIVER_RIGHT){
			to_fetch_sliver = TO_FETCH_SLIVER_CENTER;
			HY = -186000;
		}
	}
	return Key_Check_Press(&Keys.KEY_F);
}

void wait_until(bool (*condition)(int), int arg){
	while(!condition(arg)){
		osDelay(1);
	}
}
//-------------------------------------------------


//------------------杂项函数-----------------------
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
	LIFT_CAMERA = LIFT_CAMERA_MIN;
}
void SubArm_limit_ui_show(){
	if((lift_greater(LIFT_MAX - mm2angle_Lift(10)))  && ui_default_exchange_topLine->start_x == 0){
		ui_default_exchange_topLine->start_x = 488;
		ui_default_exchange_topLine->start_y = 802;
		ui_default_exchange_topLine->end_x = 1173;
		ui_default_exchange_topLine->end_y = 802;
		ui_update_default_exchange();
	}
	else if (!(lift_greater(LIFT_MAX - mm2angle_Lift(10)))  && ui_default_exchange_topLine->start_x != 0) {
		hind_line(ui_default_exchange_topLine);
		ui_update_default_exchange();
	}

	if((hy_greater(HY_ANGLE_MAX - mm2angle_Hy(10)))  && ui_default_exchange_llimitLine->start_x == 0){
		ui_default_exchange_llimitLine->start_x = 299;
		ui_default_exchange_llimitLine->start_y = 778;
		ui_default_exchange_llimitLine->end_x = 299;
		ui_default_exchange_llimitLine->end_y = 357;
		ui_update_default_exchange();
	}
	else if (!(hy_greater(HY_ANGLE_MAX - mm2angle_Hy(10)))  && ui_default_exchange_llimitLine->start_x != 0){
		hind_line(ui_default_exchange_llimitLine);
		ui_update_default_exchange();
	} 

	if((hy_less(HY_ANGLE_MIN + mm2angle_Hy(10)))  && ui_default_exchange_rlimitLine->start_x == 0){
		ui_default_exchange_rlimitLine->start_x = 1349;
		ui_default_exchange_rlimitLine->start_y = 778;
		ui_default_exchange_rlimitLine->end_x = 1349;
		ui_default_exchange_rlimitLine->end_y = 357;
		ui_update_default_exchange();
	}
	else if (!(hy_less(HY_ANGLE_MIN + mm2angle_Hy(10)))  && ui_default_exchange_rlimitLine->start_x != 0){
		hind_line(ui_default_exchange_rlimitLine);
		ui_update_default_exchange();
	} 

}
void SubArm_unreset_warning_show(){
	// 首次进入显示，首次6轴复位标志全true清除
	if(first_show_SubArm_unreset_warning){
		first_show_SubArm_unreset_warning = false;
		ui_init_default_waring();
	}
	if(	first_delete_SubArm_unreset_warning \
		&&  SubArmResetState.hy == true \
		// && 	SubArmResetState.qs == true \
		// && 	SubArmResetState.lift == true \
		// && 	SubArmResetState.roll == true \
		// && 	SubArmResetState.yaw == true \
		// && 	SubArmResetState.pitch == true 
	){
		first_delete_SubArm_unreset_warning = false;
		ui_remove_default_waring();
	}
}
//-------------------------------------------------

//------------------空模式-------------------------------
void Before_NONE_INIT(){
	// 显示none模式
	ui_init_default_Menu();
}
void NONE_loop(){
	ui_init_default_Menu();
	
	if(Key_Check_Hold(&Keys.KEY_E) && Key_Check_Press(&Keys.KEY_R)){
		if(LIFT_CAMERA < 300000) {
			LIFT_CAMERA = LIFT_CAMERA_MAX;
			CAMERA_PITCH = 1350;
		}
		else{
			LIFT_CAMERA = LIFT_CAMERA_MIN;
		}
	}
	// 新改
	//往上拨
	if(RC_CtrlData.rc.sw1_last == 3 && RC_CtrlData.rc.sw1 == 1 && sw1_down_flag == true){
		sw1_down_flag = false;
		if(xipan_debug_open == 1){
			xipan_debug_open = 2; 
			xipan_top_open();
		}
		else if(xipan_debug_open == 2){
			xipan_debug_open = 3; 
			xipan_left_open();
		}
		else if(xipan_debug_open == 3){
			xipan_debug_open = 4; 
			xipan_right_open();
			
		}
		else if(xipan_debug_open == 4){
			xipan_debug_open = 1; 
			xipan_bottom_open();
			
		}
	}
	if(RC_CtrlData.rc.sw1 == 3){
		sw1_down_flag = true;
		xipan_right_close();
		xipan_left_close();
		xipan_top_close();
		xipan_bottom_close();
		
	}
}
//-------------------------------------------------

//------------------金矿模式-----------------------
void Before_FETCH_GOLD_INIT(){
	// 双金相机视角
	CAMERA_PITCH = CAMERA_PITCH_GOLD;
	CAMERA_YAW = CAMERA_YAW_GOLD;
	LIFT_CAMERA = LIFT_CAMERA_GOLD_DOBULE;

	// 双臂摆到合适位置
	SMALL_LIFT = 80000 + 100000;
	SMALL_YAW = 50000 - 496199;
	wait_until(small_yaw_less, 70000);
	LIFT = -1720000 + mm2angle_Lift(20);
	// HY = -330000;
	ROLL = ROLL_STD;
	osDelay(1000);

	//金矿对齐UI
	ui_init_default_GOLD();

	// 显示菜单ui
	strcpy(ui_default_Menu_ModeText->string, "GOLD  ");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();
	ui_init_default_menu2();
	ui_init_default_menu3();
	if(fetch_num == 1){
		select_singel_ore();
	}
	else{
		select_more_ore();
	}
	if(fetch_mode == BY_HAND){
		select_by_hand();
	}
	else{
		select_auto_mode();
	}
	ui_update_default_menu2();
	ui_update_default_menu3();
	select_stage = 0;
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
			fetch_num = 1;
			select_singel_ore();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_num = 3;
			select_more_ore();
		}
	}
	else if (select_stage == 1) {
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_mode = AUTO_MODE;
			select_auto_mode();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_mode = BY_HAND;
			select_by_hand();
		}
	}
}

void Before_FETCH_GOLD_AUTO(){
	if(fetch_num > 1){
		HY = -330000;
		osDelay(600);
		PITCH = 16000;
		wait_until(press_key_F, 0);
		QS = 550000 - mm2angle_Qs(30);
		SMALL_QS = 500000;
		xipan_top_open();
		xipan_bottom_open();
		osDelay(1000);
		SMALL_LIFT = 300000 + 150000;
		LIFT = -1550000 + mm2angle_Lift(20);
		osDelay(500);
		SMALL_YAW = 60000 - 496199;
		QS = QS_STD;
		SMALL_QS = SMALL_QS_MIN;
		osDelay(500);
	}
	else if (fetch_num == 1) {
		SMALL_YAW = 100000 - 496199;
		HY = -143200;
		osDelay(300);
		PITCH = 16000;
		wait_until(press_key_F, 0);
		xipan_top_open();
		QS = 550000- mm2angle_Qs(30);
		if(fetch_mode = BY_HAND){
			wait_until(press_key_F, 0);
		}
		else{
			osDelay(1000);
		}
		LIFT = -1550000 + mm2angle_Lift(20);
		if(fetch_mode = BY_HAND){
			wait_until(press_key_F, 0);
		}
		else{
			osDelay(500);
		}
		
		if(fetch_mode = BY_HAND){
			QS = 550000- mm2angle_Qs(210);
			wait_until(press_key_F, 0);
			QS = QS_STD;
		}
		else{
			QS = QS_STD;
		}
		
	}

	if(fetch_mode = BY_HAND){
		wait_until(press_key_F, 0);
	}
	else{
		// 自动后退
		chassis_back(800,1000);
	}

	// SMALL_YAW = SMALL_YAW_MAX;
	SMALL_LIFT = 30000;
	HY = HY_STD;
	LIFT = LIFT_STD;
	pitch_rotate_slow_flag = true;
	pitch_slow = PITCH_UP;
	wait_until(pitch_less, 1000);
	pitch_rotate_slow_flag = false;

	// 自动退出
	last_posemod = posemod;
	posemod = NONE;
	posemode_change_flag = true;
}
void FETCH_GOLD_AUTO_loop(){

}
//-------------------------------------------------


//-------------------银矿模式----------------------
void Before_FETCH_SLIVER_INIT(){
		// 调整图传视角
		CAMERA_PITCH = CAMERA_PITCH_SLIVER;
		CAMERA_YAW = CAMERA_YAW_SLIVER;
		LIFT_CAMERA = LIFT_CAMERA_SLIVER;

		// 显示小资源岛对齐ui
		ui_init_default_Sliver();
		// 显示菜单ui
		strcpy(ui_default_Menu_ModeText->string, "SLIVER");
		ui_default_Menu_ModeText->str_length = 6;
		ui_update_default_Menu();
		ui_init_default_menu2();
		ui_init_default_menu3();
		if(fetch_num == 1){
			select_singel_ore();
		}
		else{
			select_more_ore();
		}
		if(fetch_mode == BY_HAND){
			select_by_hand();
		}
		else{
			select_auto_mode();
		}
		ui_update_default_menu2();
		ui_update_default_menu3();
		select_stage = 0;
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
			fetch_num = 1;
			select_singel_ore();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_num = 3;
			select_more_ore();
		}
	}
	else if (select_stage == 1) {
		if(Key_Check_Press(&Keys.KEY_F)){
			fetch_mode = AUTO_MODE;
			select_auto_mode();
		}
		else if(Key_Check_Press(&Keys.KEY_G)){
			fetch_mode = BY_HAND;
			select_by_hand();
		}
	}
}
void Before_FETCH_SLIVER_AUTO(){
		SMALL_YAW = - (496199/2) + 50000;
		LIFT = -622800 + mm2angle_Lift(15);
		//变化
		QS = 620000 - mm2angle_Qs(25);
		if(fetch_num > 1){
			HY = -11200 - mm2angle_Hy(0);	
		}
		else{
			HY = -186000- mm2angle_Hy(30);
		}
		ROLL = ROLL_STD;
		YAW = YAW_STD;
		// 自动后退,摆好位置
		chassis_back(550, 500);
		wait_until(qs_greater, 500000);
		PITCH = PITCH_DOWN;
		osDelay(800);
		if(fetch_num > 1){
			if(fetch_mode == BY_HAND){
				//微调
				wait_until(press_key_F,0);
			}

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

			if(fetch_mode == BY_HAND){
				LIFT_CAMERA = LIFT_CAMERA_MIN;
				CAMERA_PITCH = CAMERA_PITCH_STD;
				CAMERA_YAW = CAMERA_YAW_SLIVER;
				//微调
				wait_until(press_key_F,0);
			}
			
			CAMERA_PITCH = CAMERA_PITCH_SLIVER;
			CAMERA_YAW = CAMERA_YAW_SLIVER;
			LIFT_CAMERA = LIFT_CAMERA_SLIVER;

			xipan_left_open();
			osDelay(1000);
			yaw_rotate_slow_flag = false;
			xipan_top_close();
			osDelay(1000);
			
			// 摆第二个位置
			LIFT = -622800;
			YAW = YAW_STD;
			HY = -364000- mm2angle_Hy(30);
			QS = 620000 - mm2angle_Qs(25);
			wait_until(qs_greater, 500000);
			osDelay(800);
			
			if(fetch_mode == BY_HAND){
				//微调
				wait_until(press_key_F,0);
			}

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

			if(fetch_mode == BY_HAND){
				LIFT_CAMERA = LIFT_CAMERA_MIN;
				CAMERA_PITCH = CAMERA_PITCH_STD;
				CAMERA_YAW = CAMERA_YAW_SLIVER;
				//微调
				wait_until(press_key_F,0);
			}
			
			CAMERA_PITCH = CAMERA_PITCH_SLIVER;
			CAMERA_YAW = CAMERA_YAW_SLIVER;
			LIFT_CAMERA = LIFT_CAMERA_SLIVER;

			xipan_right_open();
			osDelay(1000);
			yaw_rotate_slow_flag = false;
			xipan_top_close();
			osDelay(1000);
		}

		// 摆第三个位置
		LIFT = -622800;
		YAW = YAW_STD;
		HY = -186000 - mm2angle_Hy(30);
		QS = 620000 - mm2angle_Qs(25);
		wait_until(qs_greater, 500000);
		osDelay(800);

		if(fetch_num == 1){
			wait_until(one_sliver_selected, 0);
		}

		if(fetch_mode == BY_HAND && fetch_num != 1){
			//微调
			wait_until(press_key_F,0);
		}

		//下降，吸
		LIFT = -722000;
		xipan_top_open();
		osDelay(1500);
		LIFT = -350000 + mm2angle_Lift(50);
		osDelay(750);
		
		chassis_auto_ctrl_flag = 1;
		vy = -2000;
		osDelay(550);
		vy = 0;
		chassis_auto_ctrl_flag = 0;

		HY = -186000;
		pitch_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		QS = QS_STD;
		LIFT = -1273474;
		osDelay(1500);
		pitch_rotate_slow_flag = false;


		// 自动退出
		last_posemod = posemod;
		posemod = NONE;
		posemode_change_flag = true;
}
void FETCH_SLIVER_AUTO_loop(){

}
//-------------------------------------------------

//-------------------兑换模式----------------------
uint8_t start_pump_flag = 0;

void Before_EXCHANGE_INIT(){
	CAMERA_PITCH = CAMERA_PITCH_EXCHANGE;
	CAMERA_YAW = CAMERA_YAW_EXCHANGE;
	LIFT_CAMERA = LIFT_CAMERA_MIN;

	strcpy(ui_default_Menu_ModeText->string, "EXCANG");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();
	ui_init_default_exchange();
	ui_init_default_exchangeMenu();
	if(to_exchange_pos != TO_EXCHANGE_TOP){
		if(to_exchange_pos == TO_EXCHANGE_RIGHT) select_exchange_right();
		else if (to_exchange_pos == TO_EXCHANGE_LEFT) select_exchange_left();
		else if (to_exchange_pos == TO_EXCHANGE_SIDE) select_exchange_side();
		_ui_update_default_exchangeMenu_1();
	}
}
void EXCHANGE_INIT_loop(){
	if(Key_Check_Press(&Keys.KEY_G)){
		if(to_exchange_pos == TO_EXCHANGE_TOP){
			to_exchange_pos = TO_EXCHANGE_RIGHT;
			select_exchange_right();
		}
		else if(to_exchange_pos == TO_EXCHANGE_RIGHT){
			to_exchange_pos = TO_EXCHANGE_LEFT;
			select_exchange_left();
		}
		else if(to_exchange_pos == TO_EXCHANGE_LEFT){
			to_exchange_pos = TO_EXCHANGE_SIDE;
			select_exchange_side();
		}
		else if(to_exchange_pos == TO_EXCHANGE_SIDE){
			to_exchange_pos = TO_EXCHANGE_TOP;
			select_exchange_top();
		}
		_ui_update_default_exchangeMenu_1();
	}

}
void Before_EXCHANGE(){
	// 拿选定位置的矿
	if(to_exchange_pos == TO_EXCHANGE_TOP){
		//啥都不用干
	}
	else if(to_exchange_pos == TO_EXCHANGE_LEFT) {
		// 左边

		LIFT_CAMERA = LIFT_CAMERA_MAX;
		CAMERA_PITCH = 1100;
		CAMERA_YAW = CAMERA_YAW_STD;

		YAW = YAW_LEFT;
		ROLL = ROLL_STD;
		LIFT = -1120000;
		QS = 470000;
		HY = -189000;
		osDelay(700);
		PITCH = PITCH_DOWN;
		osDelay(300);
		wait_until(press_key_F,0);

		// 下压
		LIFT = -1270000;

		CAMERA_PITCH = CAMERA_PITCH_EXCHANGE;
		CAMERA_YAW = CAMERA_YAW_EXCHANGE;
		LIFT_CAMERA = LIFT_CAMERA_MIN;

		xipan_top_open();
		osDelay(500);
		xipan_left_close();
		osDelay(1000);
		LIFT = LIFT_MAX;
		osDelay(500);
		HY = HY_STD;
		QS = QS_STD;
		pitch_rotate_slow_flag = true;
		yaw_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		yaw_slow = YAW_STD;
		wait_until(pitch_less, 1000);
		pitch_rotate_slow_flag = false;
		wait_until(yaw_close, YAW_STD);
		yaw_rotate_slow_flag = false;
	}
	else if(to_exchange_pos == TO_EXCHANGE_RIGHT){
		// 右边
		// 手动调整视角
		LIFT_CAMERA = LIFT_CAMERA_MAX;
		CAMERA_PITCH = 1100;
		CAMERA_YAW = 1900;

		YAW = YAW_RIGHT;
		ROLL = ROLL_STD;
		LIFT = -1120000;
		QS = 437200;
		HY = -189000;
		osDelay(700);
		PITCH = PITCH_DOWN;
		osDelay(300);
		wait_until(press_key_F,0);

		// 下压
		LIFT = -1270000;

		CAMERA_PITCH = CAMERA_PITCH_EXCHANGE;
		CAMERA_YAW = CAMERA_YAW_EXCHANGE;
		LIFT_CAMERA = LIFT_CAMERA_MIN;

		xipan_top_open();
		osDelay(500);
		xipan_right_close();
		osDelay(1000);
		LIFT = LIFT_MAX;
		osDelay(500);
		HY = HY_STD;
		QS = QS_STD;
		pitch_rotate_slow_flag = true;
		yaw_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		yaw_slow = YAW_STD;
		wait_until(pitch_less, 1000);
		pitch_rotate_slow_flag = false;
		wait_until(yaw_close, YAW_STD);
		yaw_rotate_slow_flag = false;
		
//		// 从侧边取版本
//		LIFT_CAMERA = LIFT_CAMERA_MAX;
//		CAMERA_PITCH = 1100;
//		CAMERA_YAW = 1900;
//		YAW = YAW_STD;
//		PITCH = PITCH_UP;
//		ROLL = ROLL_STD - 16384;
////		HY = xxx;
////		QS = xxx;
////		
////		LIFT = xxx;
//		osDelay(1000);
//		
//		xipan_top_open();
//		HY = HY - mm2angle_Hy(50);
//		osDelay(1500);
//		xipan_right_close();
//		osDelay(1000);
//		roll_rotate_slow_flag = true;
//		roll_slow = ROLL_STD;
//		osDelay(1000);
//		roll_rotate_slow_flag = false;
//		
//		LIFT = LIFT_STD;
//		QS = QS_STD;
//		HY = HY_STD;
	}
	else if(to_exchange_pos == TO_EXCHANGE_SIDE){
		LIFT_CAMERA = LIFT_CAMERA_MAX;
		CAMERA_PITCH = 1100;
		CAMERA_YAW = CAMERA_YAW_STD;

		// 侧边
		YAW = 31000;
		ROLL = 31000;
		// LIFT = -1120000;
		LIFT = -1139200;

		
		QS = 696000; // 易变
		HY = -169200; // 易变


		osDelay(700);
		PITCH = PITCH_DOWN;
		osDelay(300);
		xipan_top_open();
		wait_until(press_key_F,0);
		// 下压
		// LIFT = -1270000;
		CAMERA_PITCH = CAMERA_PITCH_EXCHANGE;
		CAMERA_YAW = CAMERA_YAW_EXCHANGE;
		LIFT_CAMERA = LIFT_CAMERA_MIN;

		xipan_bottom_close();
		osDelay(1000);
		LIFT = LIFT_MAX;


		osDelay(500);
		HY = HY_STD;
		QS = QS_STD;
		pitch_rotate_slow_flag = true;
		yaw_rotate_slow_flag = true;
		pitch_slow = PITCH_UP;
		yaw_slow = YAW_STD;
		wait_until(pitch_less, 1000);
		pitch_rotate_slow_flag = false;
		wait_until(yaw_close, YAW_STD);
		yaw_rotate_slow_flag = false;

	}

	// 初始化同步映射
	// wait_until(bool (*condition)(int), int arg)
	LIFT = LIFT_MAX;
	QS = QS_ANGLE_MAX/2;
	HY = HY_STD;
	ROLL = ROLL_STD;
	YAW = YAW_STD;
	PITCH = PITCH_UP;
	SubArmResetState.pitch = false;
	SubArmResetState.roll = false;
	SubArmResetState.yaw = false;
	SubArmResetState.lift = false;
	SubArmResetState.qs = false;
	SubArmResetState.hy = false;

	first_delete_SubArm_unreset_warning = true;
	first_delete_SubArm_unreset_warning = true;
	
	start_pump_flag = custom_controller_data_t.pump_flag;
}

void EXCHANGE_loop(){
	SubArm_Ctrl();
	
	// 开关泵
	
	if(custom_controller_data_t.pump_flag == start_pump_flag){
		xipan_top_open();
	}
	else if(custom_controller_data_t.pump_flag != start_pump_flag){
		xipan_top_close();
	}

	// 变换视角
	if(Key_Check_Press(&Keys.KEY_C)){
		if(LIFT_CAMERA < LIFT_CAMERA_MIN + 120000){
			LIFT_CAMERA = LIFT_CAMERA_MAX;
			CAMERA_PITCH = 1300;
		}
		else if (LIFT_CAMERA > LIFT_CAMERA_MAX - 120000) {
			LIFT_CAMERA = LIFT_CAMERA_MIN;
			CAMERA_PITCH = CAMERA_PITCH_EXCHANGE;
		}
	}
	SubArm_limit_ui_show();
	SubArm_unreset_warning_show();
}
//-------------------------------------------------

//-------------------存矿模式----------------------
void Before_STORE_INIT(){
	strcpy(ui_default_Menu_ModeText->string, "STORE ");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();

	// 借用兑矿菜单让存矿用
	ui_init_default_exchangeMenu();
	if(to_store_pos == TO_STORE_RIGHT) select_exchange_right();
	else if (to_store_pos == TO_STORE_LEFT) select_exchange_left();
	_ui_update_default_exchangeMenu_1();
}

void STORE_INIT_loop(){
	if(Key_Check_Press(&Keys.KEY_G)){
		if(to_store_pos == TO_STORE_LEFT){
			to_store_pos = TO_STORE_RIGHT;
			select_exchange_right();
		}
		else if(to_store_pos == TO_STORE_RIGHT){
			to_store_pos = TO_STORE_LEFT;
			select_exchange_left();
		}
		_ui_update_default_exchangeMenu_1();
	}
}

void Before_STORE(){
	LIFT = -1120000;
	if(to_store_pos == TO_STORE_LEFT){
		QS = 427000;
		HY = -189000 - mm2angle_Hy(250);

		yaw_rotate_slow_flag = true;
		pitch_rotate_slow_flag = true;
		yaw_slow = YAW_LEFT;
		pitch_slow = PITCH_DOWN;
		osDelay(2000);
		yaw_rotate_slow_flag = false;
		pitch_rotate_slow_flag = false;
		xipan_top_close();
		xipan_left_open();
		LIFT = -1270000;
		osDelay(1000);
	}
	else if (to_store_pos == TO_STORE_RIGHT) {
		QS = 427000;
		HY = -189000;

		yaw_rotate_slow_flag = true;
		pitch_rotate_slow_flag = true;
		yaw_slow = YAW_RIGHT;
		pitch_slow = PITCH_DOWN;
		osDelay(2000);
		yaw_rotate_slow_flag = false;
		pitch_rotate_slow_flag = false;
		xipan_top_close();
		xipan_right_open();
		LIFT = -1270000;
		osDelay(1000);
	}

	// 回正
	LIFT = -1120000;
	osDelay(400);
	YAW = YAW_STD;
	PITCH = PITCH_UP;
	osDelay(300);
	QS = QS_STD;
	HY = HY_STD;
	LIFT = LIFT_STD;

	// 自动退出
	last_posemod = posemod;
	posemod = NONE;
	posemode_change_flag = true;
}

void STORE_loop(){
	
}
//-------------------------------------------------

//-------------------地矿模式-----------------------
void Before_GROUND(){
	//UI
	strcpy(ui_default_Menu_ModeText->string, "GROUND");
	ui_default_Menu_ModeText->str_length = 6;
	ui_update_default_Menu();

	QS = mm2angle_Qs(190);
	LIFT = -1720000 - mm2angle_Lift(20);
	wait_until(qs_greater, mm2angle_Qs(180));
	YAW = YAW_STD;
	ROLL = ROLL_STD;
	PITCH = PITCH_DOWN;
	HY = HY_STD;
	
	CAMERA_PITCH = 1100;
	LIFT_CAMERA = LIFT_CAMERA_MAX;
	CAMERA_YAW = 1620;

	wait_until(press_key_F, 0);

	CAMERA_PITCH = CAMERA_PITCH_STD;
	LIFT_CAMERA = LIFT_CAMERA_MIN;
	CAMERA_YAW = CAMERA_YAW_STD;

	xipan_top_open();
	LIFT = LIFT - mm2angle_Lift(30);
	osDelay(1000);
	LIFT = LIFT_STD;
	HY = HY_STD;
	osDelay(100);
	pitch_rotate_slow_flag = true;
	pitch_slow = PITCH_UP;
	wait_until(pitch_less, PITCH_UP + 1000);
	osDelay(100);
	pitch_rotate_slow_flag = false;
	QS = QS_ANGLE_MIN;

	// 自动退出
	last_posemod = posemod;
	posemod = NONE;
	posemode_change_flag = true;

}

void GROUND_loop(){
	
}
//-------------------------------------------------

//-------------------调试模式----------------------
void DEBUG_loop(){

}
//-------------------------------------------------

//-------------模式执行键位、总逻辑框架--------------

// 模式入口单次函数
void Before_Next_Mode(){
	if(posemod == NONE) Before_NONE_INIT();
	if(posemod == last_posemod) return;
	if(posemod == DEBUG) power_less_flag = 1;
	if(posemod == FETCH_GOLD_INIT) Before_FETCH_GOLD_INIT();
	if(posemod == FETCH_GOLD_AUTO) Before_FETCH_GOLD_AUTO();	
	if(posemod == FETCH_SLIVER_INIT) Before_FETCH_SLIVER_INIT();
	if(posemod == FETCH_SLIVER_AUTO) Before_FETCH_SLIVER_AUTO();
	if(posemod == EXCHANGE_INIT) Before_EXCHANGE_INIT();
	if(posemod == EXCHANGE) Before_EXCHANGE();
	if(posemod == STORE_INIT) Before_STORE_INIT();
	if(posemod == STORE) Before_STORE();
	if(posemod == GROUND) Before_GROUND();
}

// 模式出口清理函数
void Exit_Last_Mode(){
	if(last_posemod == NONE) return;
	else if(posemod == DEBUG){
		power_less_flag = 0;
	}
	else if(last_posemod == FETCH_GOLD_INIT) {
		if(posemod == NONE){
			camera_reset();
			// 取金矿菜单退回NONE
			ui_remove_default_GOLD();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();
			LIFT = LIFT_STD;
			PITCH = PITCH_UP;
			ROLL = ROLL_STD;
			YAW = YAW_STD;
			HY = HY_STD;
			QS = QS_STD;
		}
	}
	else if(last_posemod == FETCH_GOLD_AUTO) {
		if(posemod == NONE){
			camera_reset();
			// 取金矿退回NONE，清理金UI
			ui_remove_default_GOLD();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();

			
		}
	}
	else if(last_posemod == FETCH_SLIVER_INIT) {
		if(posemod == NONE){
			camera_reset();
			// 取银初始化退回NONE，清理银UI
			ui_remove_default_Sliver();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
			ui_remove_default_menu2();
			ui_remove_default_menu3();
			
		}
	}
	else if(last_posemod == FETCH_SLIVER_AUTO) {
		if(posemod == NONE){
			camera_reset();
			// 取银退回NONE，清理银UI
			ui_remove_default_Sliver();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();

			ui_remove_default_menu2();
			ui_remove_default_menu3();
			
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
			ui_remove_default_exchange();
			ui_remove_default_exchangeMenu();
		}
	}
	else if(last_posemod == EXCHANGE_INIT) {
		if(posemod == NONE){
			camera_reset();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
			ui_remove_default_exchange();
			ui_remove_default_exchangeMenu();
		}
	}
	else if(last_posemod == GROUND) {
		if(posemod == NONE){
			camera_reset();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
		}
	}
	else if(last_posemod == STORE_INIT) {
		if(posemod == NONE){
			camera_reset();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
			ui_remove_default_exchangeMenu();
		}
	}
	else if(last_posemod == STORE) {
		if(posemod == NONE){
			camera_reset();
			strcpy(ui_default_Menu_ModeText->string, "NONE  ");
			ui_default_Menu_ModeText->str_length = 6;
			ui_update_default_Menu();
			ui_remove_default_exchangeMenu();
		}
	}
}

// 模式切换管理
void ModeManageTask(void const * argument){
	for(;;){
		// ctrl + xxx
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_Z) && posemod == NONE){
				last_posemod = posemod;
				posemod = FETCH_GOLD_INIT;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_X) && posemod == NONE){
				last_posemod = posemod;
				posemod = FETCH_SLIVER_INIT;
				posemode_change_flag = true;
			}
			
			if(Key_Check_Hold(&Keys.KEY_CTRL) && Key_Check_Press(&Keys.KEY_B)){
				last_posemod = posemod;
				posemod = NONE;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_Q) && posemod == NONE){
				last_posemod = posemod;
				posemod = DEBUG;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_V) && posemod == NONE){
				last_posemod = posemod;
				posemod = GROUND;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_C) && posemod == NONE){
				last_posemod = posemod;
				posemod = EXCHANGE_INIT;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_CTRL) &&  Key_Check_Press(&Keys.KEY_S) && posemod == NONE){
				last_posemod = posemod;
				posemod = STORE_INIT;
				posemode_change_flag = true;
			}
			if(Key_Check_Hold(&Keys.KEY_SHIFT) &&  Key_Check_Press(&Keys.KEY_R)){
				sync_data_to_c.data.reset_qs_flag = 1;
				osDelay(200);
				sync_data_to_c.data.reset_qs_flag = 0;
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
			if(posemod == STORE_INIT){
				last_posemod = posemod;
				posemod = STORE;
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
	osDelay(1000);
	for(;;){
		Before_Next_Mode();
		
		while(!posemode_change_flag){
			if(posemod == FETCH_GOLD_INIT) 	FETCH_GOLD_INIT_loop();
			else if(posemod == FETCH_SLIVER_INIT) 	FETCH_SLIVER_INIT_loop();
			else if(posemod == FETCH_SLIVER_AUTO) 	FETCH_SLIVER_AUTO_loop();
			else if(posemod == FETCH_GOLD_AUTO) 	FETCH_GOLD_AUTO_loop();
			else if(posemod == EXCHANGE_INIT) 		EXCHANGE_INIT_loop();
			else if(posemod == EXCHANGE) 			EXCHANGE_loop();
			else if(posemod == GROUND) 				GROUND_loop();
			else if(posemod == STORE_INIT) 			STORE_INIT_loop();
			else if(posemod == STORE) 				STORE_loop();
			else if(posemod == DEBUG) 				DEBUG_loop();
			else if(posemod == NONE)				NONE_loop();
			osDelay(1);
		}
		
		posemode_change_flag = false;
		Exit_Last_Mode();
		osDelay(5);
	}		
}

//-------------------------------------------------
