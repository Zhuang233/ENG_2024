#include "JointReset.h"
#include "pid.h"
#include "DJIMotoDriver.h"
#include "wd.h"
#include "cmsis_os.h"
#include "RoboArm.h"
#include "time.h"
#include "JointReset.h"

int16_t dji_moto_current_to_send[3] = {0};


// 抬升----------------------------------------------
#define LIFT_RESET_SPD_P 15
#define LIFT_RESET_SPD_I 0
#define LIFT_RESET_SPD_D 0


PidTD pid_lift_reset_spd;
bool lift_inited = false;

void lift_init_reset_stage(){
	pidInit(&pid_lift_reset_spd, 3000, 10000, LIFT_RESET_SPD_P, LIFT_RESET_SPD_I, LIFT_RESET_SPD_D);
	MotoStateInit(&MotoState[4]);
}



// 抬升位置初始化
void reset_lift(){
	float speed = 3000;
	lift_init_reset_stage();
	while(lift_inited == false){
		if(lift_reset == false){
			// 速度环
			pid_calculate(&pid_lift_reset_spd, speed, MotoState[4].speed_actual);
			SetMotoCurrent(&hcan1, Back, (pid_lift_reset_spd.outPID + 2000), 0, 0, 0);
		}
		else{
			SetMotoCurrent(&hcan1, Back, 0, 0, 0, 0);
			osDelay(20);
			lift_init();
			MotoStateInit(&MotoState[4]);
			lift_inited = true;
		}
		osDelay(1);	
	}
	MotoState[4].angle_desired = -1850000;
}
// 抬升----------------------------------------------


// 缩张----------------------------------------------
#define EXPAND_RESET_SPD_P 1
#define EXPAND_RESET_SPD_I 0
#define EXPAND_RESET_SPD_D 0

bool expand_inited = false;
PidTD pid_expand_reset_spd;
TimeTD t_duzhuan;
float detect_time = 0;
int last_detect_angle = 0;
int detect_angle = 0;
uint8_t start_stage = 1;
int a =0;


void expand_init_reset_stage(){
	pidInit(&pid_expand_reset_spd, 3000, 10000, EXPAND_RESET_SPD_P, EXPAND_RESET_SPD_I, EXPAND_RESET_SPD_D);
	MotoStateInit(&MotoState[7]);
}

void duzhuan_TimeInit(TimeTD *time){
	TimeInit(time);
}




// 缩张位置初始化 堵转检测
void reset_expand(){
	float expand_reset_speed = 3000;
	expand_init_reset_stage();
	duzhuan_TimeInit(&t_duzhuan);
	GetDt(&t_duzhuan,MILLISECOND);
	while(expand_inited == false){
		// 判断堵转
		GetDt(&t_duzhuan,MILLISECOND);
		detect_time +=t_duzhuan.dt;
		if (detect_time > 300){
			detect_time = 0;
			last_detect_angle =  detect_angle;
			detect_angle = MotoState[7].angle;
			start_stage = 0;
		}
		
		// 不转了
		if((MotoState[7].angle - last_detect_angle > 8000) || start_stage){
				// 速度环
			a = MotoState[7].angle - last_detect_angle;
				pid_calculate(&pid_expand_reset_spd, expand_reset_speed, MotoState[7].speed_actual);
				dji_moto_current_to_send[2] = pid_expand_reset_spd.outPID;
				SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
			
		}
		else{
			dji_moto_current_to_send[2] = 0;
			SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], 0);
			osDelay(20);
			expand_init();
			MotoStateInit(&MotoState[7]);
			expand_inited = true;
		}
		osDelay(1);
	}
}

// 缩张----------------------------------------------