#include "chassis.h"
#include "main.h"
#include "RcDriver.h"
#include "DJIMotoDriver.h"


#include <stdlib.h>
#include "arm_math.h"


int16_t vx = 0,vy = 0,vr = 0;

PidTD chassis_pid_spd_moto[4];


void chassis_pid_init(void)
{
	for(int i=0; i<4 ;i++){
		pidInit(&chassis_pid_spd_moto[i],10000, 10000, 20, 0, 0);
	}
}

void chassis_control(void)
{
	float norm = 0;
	float c1, c3, c4;
	
  
	vx = 0,vy = 0,vr = 0;
	norm = abs(RC_CtrlData.rc.ch3) + abs(RC_CtrlData.rc.ch4);
	c1 = (float)RC_CtrlData.rc.ch1/650.0f;
	c3 = (float)RC_CtrlData.rc.ch3 / norm;
	c4 = (float)RC_CtrlData.rc.ch4 / norm;
	arm_sqrt_f32(RC_CtrlData.rc.ch3 * RC_CtrlData.rc.ch3 + RC_CtrlData.rc.ch4 * RC_CtrlData.rc.ch4, &norm);
	if(norm > 650) norm = 650;
	norm /= 650;
	norm *= MAX_MOVE_RMP;
	vx = c3 * norm;	// 平移分量
	vy = c4 * norm;	// 前进分量
	vr = c1 * MAX_ROTATE_RMP;

	
  MotoState[0].speed_desired = -vy + vx + vr;		// 1  right front
  MotoState[1].speed_desired =  vy + vx + vr;		// 2  left front
  MotoState[2].speed_desired =  vy - vx + vr;		// 3	left back
  MotoState[3].speed_desired = -vy - vx + vr;		// 4	right back  
}


