#include "chassis.h"
#include "main.h"
#include "RcDriver.h"
#include "DJIMotoDriver.h"


#include <stdlib.h>
#include "arm_math.h"
#include "key.h"


int16_t vx = 0,vy = 0,vr = 0;

PidTD chassis_pid_spd_moto[4];


void chassis_pid_init(void)
{
	for(int i=0; i<4 ;i++){
		pidInit(&chassis_pid_spd_moto[i],10000, 10000, 20, 0, 0);
	}
}

//void chassis_control_Key(void){
//	
//	int16_t chassis_x = 0,chassis_y = 0,chassis_r = 0;
//					if(Key_Check_Hold(& Keys.KEY_W))
//						chassis_y =  KEY_MOVE_RMP;
//					if(Key_Check_Hold(& Keys.KEY_S))
//						chassis_y = -KEY_MOVE_RMP;
//					if(Key_Check_Hold(& Keys.KEY_A))
//						chassis_x = -KEY_MOVE_RMP;
//					if(Key_Check_Hold(& Keys.KEY_D))
//						chassis_x =  KEY_MOVE_RMP;   
//					if(RC_CtrlData.mouse.x!=0)   
//					{
//						chassis_r = RC_CtrlData.mouse.x*MOUSE_ROTATE_PARA;
//						if(abs(chassis_r) > MOUSE_ROTATE_RMP)
//							chassis_r = MOUSE_ROTATE_RMP * (chassis_r < 0 ? -1:1);
//					}
//					
//				ramp_calculate(&chassis_ramp[0],chassis_y);       //键盘控制移动使用斜坡函数
//				ramp_calculate(&chassis_ramp[1],chassis_x);
//				
//				if(key_flag == true)
//				{
//					vy = chassis_ramp[0].output;
//					vx = chassis_ramp[1].output;
//					vr = chassis_r;
//				}
//		}
//	
//  motor_msg[0].speed_desired = -vy + vx + vr;		// 1  right front
//  motor_msg[1].speed_desired =  vy + vx + vr;		// 2  left front
//  motor_msg[2].speed_desired =  vy - vx + vr;		// 3	left back
//  motor_msg[3].speed_desired = -vy - vx + vr;		// 4	right back 

//}


#define MOUSE_ROTATE_RMP 3000         //鼠标控制自转速度
#define MOUSE_ROTATE_PARA 10                	//设置鼠标控制自转系数

void chassis_control_RC(void)
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
	if(RC_CtrlData.mouse.x!=0)   
	{
		vr = RC_CtrlData.mouse.x*MOUSE_ROTATE_PARA;
		if(abs(vr) > MOUSE_ROTATE_RMP)
		vr = MOUSE_ROTATE_RMP * (vr < 0 ? -1:1);
	}
	
  MotoState[0].speed_desired = -vy + vx + vr;		// 1  right front
  MotoState[1].speed_desired =  vy + vx + vr;		// 2  left front
  MotoState[2].speed_desired =  vy - vx + vr;		// 3	left back
  MotoState[3].speed_desired = -vy - vx + vr;		// 4	right back  
}


// 底盘任务
//1.控制数据接收与处理
//2.pid解算
//3.给电流
void ChassisTask(void)
{
	chassis_control_RC();
	for(int i=0;i<4;i++){
		pid_calculate(&chassis_pid_spd_moto[i], MotoState[i].speed_desired, MotoState[i].speed_actual);
  }
	SetMotoCurrent(&hcan1,Ahead,chassis_pid_spd_moto[0].outPID,
															chassis_pid_spd_moto[1].outPID,
															chassis_pid_spd_moto[2].outPID,
															chassis_pid_spd_moto[3].outPID);
}

