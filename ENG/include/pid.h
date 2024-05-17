#ifndef __PID_H
#define __PID_H

#include <stdbool.h>
#include "main.h"
#include "tim.h"
/*
	必须提供符合以下条件的定时器：
		1. 频率1MHz
		2. 向上计数
		3. 重装载值0xFFFFFFFF
*/
#define PID_TIMER_HANDEL htim2

typedef struct 
{
	uint32_t 	timer_cnt_now;
	uint32_t 	timer_cnt_last;
	float	 		timer_cnt_total;
	float			dt;
}TimeTD;

typedef struct Pid_Object
{
	float desired;      //< set point
  
	float error;        //< error
	float lastError;		//上次
	float prevError;    //上上次
  
	float integ;        //< integral
	float deriv;        //< derivative
  
	float kp;           //< proportional gain
	float ki;           //< integral gain
	float kd;           //< derivative gain
  
	float outP;         //< proportional output (debugging)
	float outI;         //< integral output (debugging)
	float outD;         //< derivative output (debugging)
  float outPID;       //< pid total output
  
	float iLimit;       //< integral limit
	float iLimitLow;    //< integral limit
  
  float outLimit;      //< output limit
  float outLimitLow;   //< output limit
  
  TimeTD time;          //calculate dt
  bool first_cal;     //is first calculate pid
  
  void (*pid_calculate)(struct Pid_Object *pid,float desired,float measured); //< pid calculate
  
}PidTD;

void pidInit (PidTD* pid, 
							const float iLimit,
							const float outLimit,
							const float kp,
							const float ki,
							const float kd);
							
void pid_calculate(PidTD* pid,float desired,float measured);
void pid_calculate_inc(PidTD* pid,float desired,float measured);
void pidClearIntegral(PidTD *pid);

#endif
