#include "RoboArm.h"
#include "uart_zbw.h"
#include "RCDriver.h"
#include "pid.h"
#include "DJIMotoDriver.h"
#include "JointReset.h"
#include "chassis.h"
#include "arm_math.h"

//pitch
#define ARM_ANGLE_MAX_1 45077
#define ARM_ANGLE_MIN_1 12650
#define ARM_ANGLE_STD_1 28492

//roll
#define ARM_ANGLE_MAX_2 42583
#define ARM_ANGLE_MIN_2 9858
#define ARM_ANGLE_STD_2 25823

//yall
#define ARM_ANGLE_MAX_3 40000
#define ARM_ANGLE_MIN_3 10000
#define ARM_ANGLE_STD_3 24370


int32_t virtual_link(int32_t x);
//Ì§Éý-------------------------------------------------------

#define LIFT_POS_P 0.5
#define LIFT_POS_I 0
#define LIFT_POS_D 0
#define LIFT_SPD_P 24
#define LIFT_SPD_I 0
#define LIFT_SPD_D 0

PidTD pid_lift_spd;
PidTD pid_lift_pos;


void lift_init(){
	pidInit(&pid_lift_pos, 2000, 6000, LIFT_POS_P, LIFT_POS_I, LIFT_POS_D);
	pidInit(&pid_lift_spd, 2000, 10000, LIFT_SPD_P, LIFT_SPD_I, LIFT_SPD_D);
	MotoStateInit(&MotoState[4]);
}


void Update_Lift_Pos(){
	if(lift_inited){
		pid_calculate(&pid_lift_pos, (float)MotoState[4].angle_desired , (float)MotoState[4].angle);
		MotoState[4].speed_desired = (int)pid_lift_pos.outPID;
		pid_calculate(&pid_lift_spd, MotoState[4].speed_desired , MotoState[4].speed_actual);
		SetMotoCurrent(&hcan1, Back, pid_lift_spd.outPID + 2000, 0, 0 ,0);
	}
		
}

//Ì§Éý-------------------------------------------------------


//ËõÕÅ-------------------------------------------------------
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

//ËõÕÅ-------------------------------------------------------

//Ð¡Ì§Éý-------------------------------------------------------

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

//Ð¡Ì§Éý-------------------------------------------------------


//·­×ª-------------------------------------------------------

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
	pidInit(&pid_flip_pos, 300, 500, FLIP_POS_P, FLIP_POS_I, FLIP_POS_D);
	pidInit(&pid_flip_spd, 2000, 10000, FLIP_SPD_P, FLIP_SPD_I, FLIP_SPD_D);
	MotoStateInit(&MotoState[5]);
}


void Update_Flip_Pos(){
	if(flip_inited){
		pid_calculate(&pid_flip_pos, (float)MotoState[5].angle_desired , (float)MotoState[5].angle);
		MotoState[5].speed_desired = (int)pid_flip_pos.outPID;
		pid_calculate(&pid_flip_spd, MotoState[5].speed_desired , MotoState[5].speed_actual);
		// ¶¯Ì¬ÖØÁ¦²¹³¥
		para_gravity = -(2000.0 + 1500.0 * (float)MotoState[5].angle / -200000.0);
		Gravity_compensation = para_gravity * arm_cos_f32(((float)MotoState[5].angle) * (PI/2.0)/200000.0);
		
		dji_moto_current_to_send[0] = pid_flip_spd.outPID + Gravity_compensation;
		SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
	}
}

//·­×ª-------------------------------------------------------


void RoboArm_Pos_Init(){
	sync_data_to_c.data.theta1 = ARM_ANGLE_STD_1;
	sync_data_to_c.data.theta2 = ARM_ANGLE_STD_2;
	sync_data_to_c.data.theta3 = ARM_ANGLE_STD_3;
	sync_data_to_c.data.qs_pos = 10000;
	sync_data_to_c.data.hy_pos = -200000;
}

// Ò£¿Ø¿ØÖÆ6Öá
void RoboArm_RC_Ctrl(){
	if(RC_CtrlData.rc.sw1 == 1){
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
	
	
	else if(RC_CtrlData.rc.sw1 == 3){
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
		chassis_control();
		ChassisTask();
	}
	
	
	if(RC_CtrlData.rc.sw2 == 1){
		
		// ËõÕÅ
		MotoState[7].angle_desired += RC_CtrlData.rc.ch1;
		if(MotoState[7].angle_desired > -10000) MotoState[7].angle_desired = -10000;
		if(MotoState[7].angle_desired < -420000) MotoState[7].angle_desired = -420000;
	
		
		// ·­×ª
		MotoState[5].angle_desired += RC_CtrlData.rc.ch4/5;
		if(MotoState[5].angle_desired > 0) MotoState[5].angle_desired = 0;
		if(MotoState[5].angle_desired < -200000) MotoState[5].angle_desired = -200000;

		// Ð¡Ì§Éý
		//	MotoState[6].angle_desired += RC_CtrlData.rc.ch2;
		
		MotoState[6].angle_desired = virtual_link(MotoState[5].angle);
		if(MotoState[6].angle_desired > 380000) MotoState[6].angle_desired = 380000;
		if(MotoState[6].angle_desired < 0) MotoState[6].angle_desired = 0;
	}
}


int32_t virtual_link(int32_t x){
	  if (-30000< x && x <=0)           return (float)x            / -30.0;
    if (-40000 < x && x<= -30000)      return ((float)x + 3333.3) / -(1.0/0.55);
    if (-57500 < x && x<= -40000)     return ((float)x + 30683)  / -0.493;
    if (-79000 < x && x <= -57500)    return ((float)x + 31461)  / -0.478;
    if (-87000 < x && x <= -79000)   return ((float)x + 63244)  / -0.158;
    if (-103000 < x && x <= -87000)  return ((float)x + 31186)  / -0.372;
    if (-125000 < x && x<= -103000)   return ((float)x - 10246)  / -0.587;
    if (-170000 < x && x<= -125000)   return ((float)x - 298909) / -1.837;
    if (-191500 < x && x <= -170000)  return ((float)x + 41188)  / -0.506;
    if (-250000 < x && x<= -191500)   return ((float)x + 160311) / -0.105;
}

