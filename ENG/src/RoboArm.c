#include "RoboArm.h"
#include "uart_zbw.h"
#include "RCDriver.h"
#include "pid.h"
#include "DJIMotoDriver.h"
#include "JointReset.h"
#include "chassis.h"

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
}




