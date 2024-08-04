#include "RoboArm.h"
#include "uart_zbw.h"
#include "RCDriver.h"
#include "pid.h"
#include "JointReset.h"
#include "chassis.h"
#include "arm_math.h"
#include "relay.h"
#include "referee.h"
#include "math.h"

uint8_t power_less_flag = 0;


int32_t virtual_link(int32_t x);
//抬升-------------------------------------------------------

#define LIFT_POS_P 0.05
#define LIFT_POS_I 0
#define LIFT_POS_D 0
#define LIFT_SPD_P 24
#define LIFT_SPD_I 0
#define LIFT_SPD_D 0

PidTD pid_lift_spd;
PidTD pid_lift_pos;


void lift_init(){
	pidInit(&pid_lift_pos, 2000, 10000, LIFT_POS_P, LIFT_POS_I, LIFT_POS_D);
	pidInit(&pid_lift_spd, 2000, 16380, LIFT_SPD_P, LIFT_SPD_I, LIFT_SPD_D);
	MotoStateInit(&MotoState[4]);
}


void Update_Lift_Pos(){
	if(lift_inited){
		if(!power_less_flag){
			pid_calculate(&pid_lift_pos, (float)MotoState[4].angle_desired , (float)MotoState[4].angle);
			MotoState[4].speed_desired = (int)pid_lift_pos.outPID;
			pid_calculate(&pid_lift_spd, MotoState[4].speed_desired , MotoState[4].speed_actual);
			SetMotoCurrent(&hcan1, Back, pid_lift_spd.outPID + 2000, 0, 0 ,0);
		}
		else{
			SetMotoCurrent(&hcan1, Back, 0, 0, 0 ,0);
		}

	}
		
}

//抬升-------------------------------------------------------


# ifdef OLD_CAR

//缩张-------------------------------------------------------
#define EXPAND_POS_P 0.5
#define EXPAND_POS_I 0
#define EXPAND_POS_D 0
#define EXPAND_SPD_P 10
#define EXPAND_SPD_I 1
#define EXPAND_SPD_D 0

PidTD pid_expand_spd;
PidTD pid_expand_pos;

void expand_init(){
	pidInit(&pid_expand_pos, 2000, 6000, EXPAND_POS_P, EXPAND_POS_I, EXPAND_POS_D);
	pidInit(&pid_expand_spd, 2000, 10000, EXPAND_SPD_P, EXPAND_SPD_I, EXPAND_SPD_D);
	MotoStateInit(&MotoState[4]);
}


void Update_Expand_Pos(){
	if(expand_inited){
		pid_calculate(&pid_expand_pos, (float)MotoState[7].angle_desired , (float)MotoState[7].angle);
		MotoState[7].speed_desired = (int)pid_expand_pos.outPID;
		pid_calculate(&pid_expand_spd, MotoState[7].speed_desired , MotoState[7].speed_actual);
		dji_moto_current_to_send[2] = pid_expand_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
	}
}

//缩张-------------------------------------------------------

//小抬升-------------------------------------------------------

#define SMALL_LIFT_POS_P 0.1
#define SMALL_LIFT_POS_I 0
#define SMALL_LIFT_POS_D 0
#define SMALL_LIFT_SPD_P 20
#define SMALL_LIFT_SPD_I 0
#define SMALL_LIFT_SPD_D 0

PidTD pid_small_lift_spd;
PidTD pid_small_lift_pos;
bool small_lift_inited = true;

void small_lift_init(){
	pidInit(&pid_small_lift_pos, 2000, 3000, SMALL_LIFT_POS_P, SMALL_LIFT_POS_I, SMALL_LIFT_POS_D);
	pidInit(&pid_small_lift_spd, 2000, 10000, SMALL_LIFT_SPD_P, SMALL_LIFT_SPD_I, SMALL_LIFT_SPD_D);
	MotoStateInit(&MotoState[6]);
}


void Update_Small_Lift_Pos(){
	if(small_lift_inited){
		pid_calculate(&pid_small_lift_pos, (float)MotoState[6].angle_desired , (float)MotoState[6].angle);
		MotoState[6].speed_desired = (int)pid_small_lift_pos.outPID;
		pid_calculate(&pid_small_lift_spd, MotoState[6].speed_desired , MotoState[6].speed_actual);
		dji_moto_current_to_send[1] = pid_small_lift_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
	}
		
}

//小抬升-------------------------------------------------------


//翻转-------------------------------------------------------

////#define FLIP_POS_P 0.08
#define FLIP_POS_P 0.08
#define FLIP_POS_I 0.000001
#define FLIP_POS_D 0
#define FLIP_SPD_P 25
#define FLIP_SPD_I 0
#define FLIP_SPD_D 0

float Gravity_compensation = 0.0;
float para_gravity = 0.0;
PidTD pid_flip_spd;
PidTD pid_flip_pos;
bool flip_inited = true;

void flip_init(){
	pidInit(&pid_flip_pos, 300, 800, FLIP_POS_P, FLIP_POS_I, FLIP_POS_D);
	pidInit(&pid_flip_spd, 2000, 10000, FLIP_SPD_P, FLIP_SPD_I, FLIP_SPD_D);
	MotoStateInit(&MotoState[5]);
}



void Update_Flip_Pos(){
	if(flip_inited){
		pid_calculate(&pid_flip_pos, (float)MotoState[5].angle_desired , (float)MotoState[5].angle);
		MotoState[5].speed_desired = (int)pid_flip_pos.outPID;
		pid_calculate(&pid_flip_spd, MotoState[5].speed_desired , MotoState[5].speed_actual);
		// 动态重力补偿
		para_gravity = -(2000.0f + 1500.0f * (float)MotoState[5].angle / -200000.0f);
		Gravity_compensation = para_gravity * arm_cos_f32(((float)MotoState[5].angle) * (PI/2.0f)/200000.0f);
		
		dji_moto_current_to_send[0] = pid_flip_spd.outPID + Gravity_compensation;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
	}
}

//翻转-------------------------------------------------------

#else
//小yaw-------------------------------------------------------
#define SMALL_YAW_POS_P 0.5
#define SMALL_YAW_POS_I 0
#define SMALL_YAW_POS_D 0
#define SMALL_YAW_SPD_P 10
#define SMALL_YAW_SPD_I 1
#define SMALL_YAW_SPD_D 0

PidTD pid_small_yaw_spd;
PidTD pid_small_yaw_pos;

void small_yaw_init(){
	pidInit(&pid_small_yaw_pos, 2000, 6000, SMALL_YAW_POS_P, SMALL_YAW_POS_I, SMALL_YAW_POS_D);
	pidInit(&pid_small_yaw_spd, 2000, 10000, SMALL_YAW_SPD_P, SMALL_YAW_SPD_I, SMALL_YAW_SPD_D);
	MotoStateInit(&MotoState[7]);
}


void Update_Small_Yaw_Pos(){
	if(small_yaw_inited){
		pid_calculate(&pid_small_yaw_pos, (float)MotoState[7].angle_desired , (float)MotoState[7].angle);
		MotoState[7].speed_desired = (int)pid_small_yaw_pos.outPID;
		pid_calculate(&pid_small_yaw_spd, MotoState[7].speed_desired , MotoState[7].speed_actual);
		dji_moto_current_to_send[2] = pid_small_yaw_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
	}
}

//小yaw-------------------------------------------------------

//小抬升-------------------------------------------------------
#define SMALL_LIFT_POS_P 0.5
#define SMALL_LIFT_POS_I 0
#define SMALL_LIFT_POS_D 0
#define SMALL_LIFT_SPD_P 10
#define SMALL_LIFT_SPD_I 1
#define SMALL_LIFT_SPD_D 0

PidTD pid_small_lift_spd;
PidTD pid_small_lift_pos;

void small_lift_init(){
	pidInit(&pid_small_lift_pos, 2000, 6000, SMALL_LIFT_POS_P, SMALL_LIFT_POS_I, SMALL_LIFT_POS_D);
	pidInit(&pid_small_lift_spd, 2000, 10000, SMALL_LIFT_SPD_P, SMALL_LIFT_SPD_I, SMALL_LIFT_SPD_D);
	MotoStateInit(&MotoState[6]);
}


void Update_Small_Lift_Pos(){
	if(small_lift_inited){
		pid_calculate(&pid_small_lift_pos, (float)MotoState[6].angle_desired , (float)MotoState[6].angle);
		MotoState[6].speed_desired = (int)pid_small_lift_pos.outPID;
		pid_calculate(&pid_small_lift_spd, MotoState[6].speed_desired , MotoState[6].speed_actual);
		dji_moto_current_to_send[1] = pid_small_lift_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
	}
}

//小抬升-------------------------------------------------------

//小前伸-------------------------------------------------------
#define SMALL_QS_POS_P 0.5
#define SMALL_QS_POS_I 0
#define SMALL_QS_POS_D 0
#define SMALL_QS_SPD_P 10
#define SMALL_QS_SPD_I 1
#define SMALL_QS_SPD_D 0

PidTD pid_small_qs_spd;
PidTD pid_small_qs_pos;

void small_qs_init(){
	pidInit(&pid_small_qs_pos, 2000, 6000, SMALL_QS_POS_P, SMALL_QS_POS_I, SMALL_QS_POS_D);
	pidInit(&pid_small_qs_spd, 2000, 10000, SMALL_QS_SPD_P, SMALL_QS_SPD_I, SMALL_QS_SPD_D);
	MotoStateInit(&MotoState[5]);
}


void Update_Small_Qs_Pos(){
	if(small_qs_inited){
		pid_calculate(&pid_small_qs_pos, (float)MotoState[5].angle_desired , (float)MotoState[5].angle);
		MotoState[5].speed_desired = (int)pid_small_qs_pos.outPID;
		pid_calculate(&pid_small_qs_spd, MotoState[5].speed_desired , MotoState[5].speed_actual);
		dji_moto_current_to_send[0] = pid_small_qs_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
	}
}

//小前伸-------------------------------------------------------

//云台抬升-------------------------------------------------------
#define LIFT_CAMERA_POS_P 0.5
#define LIFT_CAMERA_POS_I 0
#define LIFT_CAMERA_POS_D 0
#define LIFT_CAMERA_SPD_P 10
#define LIFT_CAMERA_SPD_I 1
#define LIFT_CAMERA_SPD_D 0

PidTD pid_lift_camera_spd;
PidTD pid_lift_camera_pos;

void lift_camera_init(){
	pidInit(&pid_lift_camera_pos, 2000, 10000, LIFT_CAMERA_POS_P, LIFT_CAMERA_POS_I, LIFT_CAMERA_POS_D);
	pidInit(&pid_lift_camera_spd, 2000, 10000, LIFT_CAMERA_SPD_P, LIFT_CAMERA_SPD_I, LIFT_CAMERA_SPD_D);
	MotoStateInit(&MotoState[8]);
}


void Update_Lift_Camera_Pos(){
	if(lift_camera_inited){
		pid_calculate(&pid_lift_camera_pos, (float)MotoState[8].angle_desired , (float)MotoState[8].angle);
		MotoState[8].speed_desired = (int)pid_lift_camera_pos.outPID;
		pid_calculate(&pid_lift_camera_spd, MotoState[8].speed_desired , MotoState[8].speed_actual);
		dji_moto_current_to_send[3] = pid_lift_camera_spd.outPID;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
	}
}

//云台抬升-------------------------------------------------------

#endif

void RoboArm_Pos_Init(){
	sync_data_to_c.data.theta1 = ARM_ANGLE_STD_1;
	sync_data_to_c.data.theta2 = ARM_ANGLE_STD_2;
	sync_data_to_c.data.theta3 = ARM_ANGLE_STD_3;
	sync_data_to_c.data.qs_pos = 2000;
	sync_data_to_c.data.hy_pos = -200000;
	sync_data_to_c.data.reset_qs_flag = 0;
}

// 遥控控制全车
void RoboArm_RC_Ctrl(){
	if(RC_CtrlData.rc.sw1 == 1 && RC_CtrlData.rc.sw2 == 1){
		sync_data_to_c.data.theta1 += RC_CtrlData.rc.ch2 / 15;
		if(sync_data_to_c.data.theta1 > ARM_ANGLE_MAX_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MAX_1;
		if(sync_data_to_c.data.theta1 < ARM_ANGLE_MIN_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MIN_1;
		
		sync_data_to_c.data.theta2 -= RC_CtrlData.rc.ch1 / 15;
		if(sync_data_to_c.data.theta2 > ARM_ANGLE_MAX_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MAX_2;
		if(sync_data_to_c.data.theta2 < ARM_ANGLE_MIN_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MIN_2;
		
		sync_data_to_c.data.theta3 -= RC_CtrlData.rc.ch3 / 15;
		if(sync_data_to_c.data.theta3 > ARM_ANGLE_MAX_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MAX_3;
		if(sync_data_to_c.data.theta3 < ARM_ANGLE_MIN_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MIN_3;
		
	}
	
	
	else if(RC_CtrlData.rc.sw1 == 3 && RC_CtrlData.rc.sw2 == 1){
		sync_data_to_c.data.qs_pos += RC_CtrlData.rc.ch4 * 2.5;
		if(sync_data_to_c.data.qs_pos > 780000) sync_data_to_c.data.qs_pos = 780000;
		if(sync_data_to_c.data.qs_pos < 10000) sync_data_to_c.data.qs_pos = 10000;
		
		sync_data_to_c.data.hy_pos -= RC_CtrlData.rc.ch3 * 1.5;
		if(sync_data_to_c.data.hy_pos > -5000) sync_data_to_c.data.hy_pos = -5000;
		if(sync_data_to_c.data.hy_pos < -390000) sync_data_to_c.data.hy_pos = -390000;
		
		MotoState[4].angle_desired += RC_CtrlData.rc.ch2 * 2.5;
		if(MotoState[4].angle_desired > -10000) MotoState[4].angle_desired = -10000;
		if(MotoState[4].angle_desired < -1950000) MotoState[4].angle_desired = -1950000;
	}
	else if(RC_CtrlData.rc.sw1 == 2){

	}
	
	
	if(RC_CtrlData.rc.sw2 == 2){
		
		// 缩张
		MotoState[7].angle_desired += RC_CtrlData.rc.ch1;
		if(MotoState[7].angle_desired > -10000) MotoState[7].angle_desired = -10000;
		if(MotoState[7].angle_desired < -420000) MotoState[7].angle_desired = -420000;
	
		
		// 翻转
		MotoState[5].angle_desired += RC_CtrlData.rc.ch4/5;
		if(MotoState[5].angle_desired > 0) MotoState[5].angle_desired = 0;
		if(MotoState[5].angle_desired < -200000) MotoState[5].angle_desired = -200000;

		// 小抬升
		//	MotoState[6].angle_desired += RC_CtrlData.rc.ch2;
		
		MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		if(MotoState[6].angle_desired > 380000) MotoState[6].angle_desired = 380000;
		if(MotoState[6].angle_desired < 0) MotoState[6].angle_desired = 0;
	}
}

float gx=280,gy=350.0,gz=500.0;
float theta_real_1 = 0.0,theta_real_2 = 0.0,theta_real_3 = 0.0;
float c1,c2,c3,s1,s2,s3;
float h,w,l;
bool my_ctrl_reset_first_time = true;

extern bool lock_ctrl;
float suofang = 2.0;
uint8_t key_2_last = 0x00;

			extern uint16_t pitch_slow;
			extern uint16_t roll_slow;
			extern uint16_t yaw_slow;
void RoboArm_RC_Ctrl_Fixed_Point(){
//	if(RC_CtrlData.rc.sw2 == 3){
	
//			if(custom_controller_data_t.key_1 == 0x00){
//				pump_top_open();
//				xipan_top_open();
//			}
//			else{
//				pump_top_close();
//				xipan_top_close();
//			}
	
//			if(custom_controller_data_t.key_2 != key_2_last){
//				pump_top_close();
//				xipan_top_close();
//			}
//			else{
//				pump_top_open();
//				xipan_top_open();
//			}
			
//		if(custom_controller_data_t.key_2 == 0xff && my_ctrl_reset_first_time){
//			gx=250.0;
//			gy=250.0;
//			gz=500.0;
//			my_ctrl_reset_first_time = false;
//		}
//		else if(custom_controller_data_t.key_2 == 0x00){
//			my_ctrl_reset_first_time = true;
//		}
			
//			if(custom_controller_data_t.key_2 != 0xff){
//				gz += custom_controller_data_t.vz * suofang;
//				gy -= custom_controller_data_t.vx * suofang;
//				gx += custom_controller_data_t.vy * suofang;
					gy = ((float)custom_controller_data_t.encoder1 + 32768.0f) * 600.0f / 65536.0f  ; //gy 0 ~ 600 encoder -32768 ~ 32767
//			}
			

			
			if(gx < 250.0f) gx = 250.0f;
			if(gx > 800.0f) gx = 800.0f;
			if(gz < 430.0f) gz = 430.0f;	
			if(gz >980.0f) gz = 980.0f;				
			if(gy < 0.0f) gy = 0.0f;
			if(gy > 600.0f) gy = 600.0f;
		
			// 1.遥控给欧拉角
			//			sync_data_to_c.data.theta1 += RC_CtrlData.rc.ch2 / 15;
			//			sync_data_to_c.data.theta2 -= RC_CtrlData.rc.ch1 / 15;
			//			sync_data_to_c.data.theta3 -= RC_CtrlData.rc.ch3 / 15;
			
			// 2.自定义控制器欧拉角
			//			sync_data_to_c.data.theta1 = ARM_ANGLE_STD_1 - custom_controller_data_t.pitch * 16384.0 / 90.0;
			//			sync_data_to_c.data.theta2 = ARM_ANGLE_STD_2 - custom_controller_data_t.roll * 16384.0 / 90.0;
			//			sync_data_to_c.data.theta3 = ARM_ANGLE_STD_1 - custom_controller_data_t.yaw * 16384.0 / 90.0;
			
			
			
			// 3.自定义控制器四元数转欧拉角(不会)

//			sync_data_to_c.data.theta1 = 
//			sync_data_to_c.data.theta2 =
//			sync_data_to_c.data.theta3 = 
			
//			float w,x,y,z;
//			w = custom_controller_data_t.quaternion[0];
//			x = custom_controller_data_t.quaternion[1];
//			y = custom_controller_data_t.quaternion[2];
//	    z = custom_controller_data_t.quaternion[3];
//			
//			float yaw =  asin(2.0 * (x*z - w*y)) * 180.0 / PI;
//			float pitch =  atan2(2.0 * (w*x + y*z), w*w - x*x - y*y + z*z) * 180.0 / PI;
//			float roll = -atan2(2.0 * (w*z + x*y), w*w + x*x - y*y - z*z) * 180.0 / PI;
			
			
			// 
//			sync_data_to_c.data.theta1  = 28492 - pitch * 16384.0 / 90.0;
//			sync_data_to_c.data.theta2 = ARM_ANGLE_STD_2 - roll * 16384.0 / 90.0;
//			sync_data_to_c.data.theta3 = ARM_ANGLE_STD_3 - yaw * 16384.0 / 90.0;
			sync_data_to_c.data.theta3 = (uint16_t)( ((float)custom_controller_data_t.adc_value1 - 400.0f) * ((40754.0f-7986.0f)/(1830.0f-400.0f)) + 7986.0f ); // yaw 7986 ~ 40754 adc 400~1830
			
			
			
			if(sync_data_to_c.data.theta1 > ARM_ANGLE_MAX_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MAX_1;
			if(sync_data_to_c.data.theta1 < ARM_ANGLE_MIN_1) sync_data_to_c.data.theta1 = ARM_ANGLE_MIN_1;
			if(sync_data_to_c.data.theta2 > ARM_ANGLE_MAX_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MAX_2;
			if(sync_data_to_c.data.theta2 < ARM_ANGLE_MIN_2) sync_data_to_c.data.theta2 = ARM_ANGLE_MIN_2;
			if(sync_data_to_c.data.theta3 > ARM_ANGLE_MAX_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MAX_3;
			if(sync_data_to_c.data.theta3 < ARM_ANGLE_MIN_3) sync_data_to_c.data.theta3 = ARM_ANGLE_MIN_3;
		
			theta_real_1 = PI/32767.5f *(float)(sync_data_to_c.data.theta1 - ARM_ANGLE_STD_1);
			theta_real_2 = -PI/32767.5f *(float)(sync_data_to_c.data.theta2 - ARM_ANGLE_STD_2);
			theta_real_3 = PI/32767.5f *(float)(sync_data_to_c.data.theta3 - ARM_ANGLE_STD_3);
			c1 = arm_cos_f32(theta_real_1);
			c2 = arm_cos_f32(theta_real_2 - PI/2);
			c3 = arm_cos_f32(theta_real_3);
			s1 = arm_sin_f32(theta_real_1);
			s2 = arm_sin_f32(theta_real_2 - PI/2);
			s3 = arm_sin_f32(theta_real_3);
			
//			h = gz - 120.0*s2*s1;
			h = 619.0;
//			w = gy - 225.0*s3 -120.0*(c3*c2*s1+s3*c1);
			w = gy;
//			l = gx - 225.0*c3 -120.0*(-s3*c2*s1+c3*c1);
			l = 100.0;
			
			MotoState[4].angle_desired = (int32_t)((h - 973.0f)*500000.0f/175.0f);
			sync_data_to_c.data.hy_pos = (int32_t)(w *390000.0f/600.0f -390000.0f);
			sync_data_to_c.data.qs_pos = (int32_t)(l * 780000.0f/388.0f);
			
			
			if(MotoState[4].angle_desired > -10000) MotoState[4].angle_desired = -10000;
			if(MotoState[4].angle_desired < -1950000) MotoState[4].angle_desired = -1950000;
			
//			
//		}
	
}

// 单位换算函数
// 抬升mm转编码器总角度
int32_t mm2angle_Lift(int32_t pos_mm){
	return (LIFT_MAX - LIFT_MIN) * pos_mm / (888-220);
}

// 前伸mm转编码器总角度
int32_t mm2angle_Qs(int32_t pos_mm){
	return (QS_ANGLE_MAX - QS_ANGLE_MIN) * pos_mm / (450);
}

// 横移mm转编码器总角度
int32_t mm2angle_Hy(int32_t pos_mm){
	return (HY_ANGLE_MAX - HY_ANGLE_MIN) * pos_mm / (580);
}

#ifdef OLD_CAR
// 抬升自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Lift(int16_t InputAngle){
	// h = 619.0;
	// return (int32_t)((h - 973.0f)*500000.0f/175.0f);
	return 1950000.0f/6240.0f * InputAngle - 800000.0f;
}

// 横移自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Hy(int16_t InputAngle){
	// gy = 600.0f - ((float)InputAngle * 600.0f / 4700.0f)  ; //gy 0 ~ 600 encoder 0 ~ 4700
	// return (int32_t)(gy *390000.0f/600.0f -390000.0f);
	return (4700.0f - InputAngle) * 390000.0f/4700.0f - 390000.0f;
}

// 前伸自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Qs(int16_t InputAngle){
	return InputAngle * 780000.0f/3120.0f;
}

// 自定义控制器电位计yaw映射
int32_t AngleMap_Yaw(int16_t InputAngle){
	return (7986.0f - 40754.0f)/(3250.0f - 700.0f) * InputAngle + (700.0f * 7986.0f - 3250.0f * 40754.0f)/(700.0f - 3250.0f);
}

// 自定义控制器电位计pitch映射
int32_t AngleMap_Pitch(int16_t InputAngle){
	return (12590.0f - 44950.0f)/(3300.0f - 690.0f) * InputAngle + (3300.0f * 44950.0f - 12590.0f * 690.0f)/(3300.0f - 690.0f);
}

// 自定义控制器电位计roll映射
int32_t AngleMap_Roll(int16_t InputAngle){
	return (42207.0f - 9439.0f)/(3200.0f - 710.0f) * InputAngle + (3200.0f * 9439.0f - 42207.0f * 710.0f)/(3200.0f - 710.0f);
}
#else
// 抬升自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Lift(int16_t InputAngle){
	// 新改
	float32_t target = (1950000.0f/6240.0f * (InputAngle * 2.0f)) - 1400000.0f;
	if(target > 0) target = 0;
	return target;
}

// 横移自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Hy(int16_t InputAngle){
	return (4700.0f - InputAngle) * 389000.0f/4700.0f - 389000.0f;
}

// 前伸自定义控制器编码器转编码器总角度映射
int32_t AngleMap_Qs(int16_t InputAngle){
	int32_t target = InputAngle * 442000.0f/3120.0f * 2;
	if(target < 0) target = 0;
	return target;
}

// 自定义控制器电位计yaw映射
int32_t AngleMap_Yaw(int16_t InputAngle){
	int32_t target = (0.0f - 32300.0f)/(3250.0f - 700.0f) * InputAngle + (700.0f * 0.0f - 3250.0f * 32300.0f)/(700.0f - 3250.0f);
	if(target <0) target = 0;
	return target;
}
	

// 自定义控制器电位计pitch映射
int32_t AngleMap_Pitch(int16_t InputAngle){
	int32_t target = (0.0f - 32767.0f)/(3300.0f - 690.0f) * InputAngle + (3300.0f * 32767.0f - 0.0f * 690.0f)/(3300.0f - 690.0f);
	if(target <0) target = 0;
	return target;
}

// 自定义控制器电位计roll映射
int32_t AngleMap_Roll(int16_t InputAngle){
	int32_t target = (33500.0f)/(3200.0f - 710.0f) * InputAngle + (3200.0f * 33500.0f/2.0f - (33500.0f*3.0f/2.0f) * 710.0f)/(3200.0f - 710.0f);
	if(target < 0) target = 0;
	return target;
}

#endif

SubArmMap_t SubArm_ctrl_value = {0,0,0,0,0,0};
SubArmResetState_t SubArmResetState = {false,false,false,false,false,false};


bool value_between(int32_t value, int32_t min, int32_t max){
	return (value > min && value < max);
}

void SubArm_Ctrl(){
	SubArm_ctrl_value.lift = AngleMap_Lift(custom_controller_data_t.encoder3);
	SubArm_ctrl_value.hy = AngleMap_Hy(custom_controller_data_t.encoder1);
	SubArm_ctrl_value.qs = AngleMap_Qs(custom_controller_data_t.encoder2);
	SubArm_ctrl_value.pitch = AngleMap_Pitch(custom_controller_data_t.adc_value3);
	SubArm_ctrl_value.roll = AngleMap_Roll(custom_controller_data_t.adc_value2);
	SubArm_ctrl_value.yaw = AngleMap_Yaw(custom_controller_data_t.adc_value1);

	if(value_between(SubArm_ctrl_value.lift, -mm2angle_Lift(50), 0)){
		SubArmResetState.lift = true;
	}
	if(value_between(SubArm_ctrl_value.hy, HY_STD - mm2angle_Hy(50), HY_STD + mm2angle_Hy(50))){
		SubArmResetState.hy = true;
	}
	if(value_between(SubArm_ctrl_value.qs, QS_ANGLE_MAX/2 - mm2angle_Qs(50), QS_ANGLE_MAX/2+mm2angle_Qs(50))){
		SubArmResetState.qs = true;
	}
	if(value_between(SubArm_ctrl_value.roll, ROLL_STD -8192 , ROLL_STD + 8192)){
		SubArmResetState.roll = true;
	}
	if(value_between(SubArm_ctrl_value.pitch, 0, 4000)){
		SubArmResetState.pitch = true;
	}
	if(value_between(SubArm_ctrl_value.yaw, YAW_STD - 2000, YAW_STD + 2000)){
		SubArmResetState.yaw = true;
	}

	// 检查是否复位过
	if(SubArmResetState.hy == true) HY = SubArm_ctrl_value.hy;
	if(SubArmResetState.qs == true) QS = SubArm_ctrl_value.qs;
	if(SubArmResetState.lift == true) LIFT = SubArm_ctrl_value.lift;
	if(SubArmResetState.pitch == true) PITCH = SubArm_ctrl_value.pitch;
	if(SubArmResetState.roll == true) ROLL = SubArm_ctrl_value.roll;
	if(SubArmResetState.yaw == true) YAW = SubArm_ctrl_value.yaw;
}


int32_t virtual_link(int32_t x){
	  if (-30000< x && x <=0)           return (float)x            / -30.0f;
    if (-40000 < x && x<= -30000)      return ((float)x + 3333.3f) / -(1.0f/0.55f);
    if (-57500 < x && x<= -40000)     return ((float)x + 30683)  / -0.493;
    if (-79000 < x && x <= -57500)    return ((float)x + 31461)  / -0.478;
    if (-87000 < x && x <= -79000)   return ((float)x + 63244)  / -0.158;
    if (-103000 < x && x <= -87000)  return ((float)x + 31186)  / -0.372;
    if (-125000 < x && x<= -103000)   return ((float)x - 10246)  / -0.587;
    if (-170000 < x && x<= -125000)   return ((float)x - 298909) / -1.837;
    if (-191500 < x && x <= -170000)  return ((float)x + 41188)  / -0.506;
    if (-250000 < x && x<= -191500)   return ((float)x + 160311) / -0.105;
		return 0;
}

