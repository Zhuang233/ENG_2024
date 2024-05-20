#include "JointReset.h"
#include "pid.h"
#include "DJIMotoDriver.h"
#include "wd.h"
#include "cmsis_os.h"
#include "RoboArm.h"


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
	MotoState[0].angle_desired = -10000;
}

