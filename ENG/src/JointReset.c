#include "JointReset.h"
#include "pid.h"
#include "DJIMotoDriver.h"
#include "wd.h"
#include "cmsis_os.h"
#include "RoboArm.h"
#include "time.h"
#include "JointReset.h"

int16_t dji_moto_current_to_send[4] = {0};


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
	MotoState[4].angle_desired = -1273474;
}
// 抬升----------------------------------------------

# ifdef OLD_CAR
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
#else

void duzhuan_TimeInit(TimeTD *time){
	TimeInit(time);
}

// 小yaw----------------------------------------------
#define SMALL_YAW_RESET_SPD_P 1
#define SMALL_YAW_RESET_SPD_I 0
#define SMALL_YAW_RESET_SPD_D 0

bool small_yaw_inited = false;
PidTD pid_small_yaw_reset_spd;
TimeTD t_duzhuan;
float detect_time = 0;
int last_detect_angle = 0;
int detect_angle = 0;
uint8_t start_stage = 1;


void small_yaw_init_reset_stage(){
	pidInit(&pid_small_yaw_reset_spd, 3000, 10000, SMALL_YAW_RESET_SPD_P, SMALL_YAW_RESET_SPD_I, SMALL_YAW_RESET_SPD_D);
	MotoStateInit(&MotoState[7]);
}

// 小yaw位置初始化 堵转检测
void reset_small_yaw(){
	float small_yaw_reset_speed = 3000;
	small_yaw_init_reset_stage();
	duzhuan_TimeInit(&t_duzhuan);
	GetDt(&t_duzhuan,MILLISECOND);
	while(small_yaw_inited == false){
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
		if((MotoState[7].angle - last_detect_angle < -8000) || start_stage){
				// 速度环
				pid_calculate(&pid_small_yaw_reset_spd, -small_yaw_reset_speed, MotoState[7].speed_actual);
				dji_moto_current_to_send[2] = pid_small_yaw_reset_spd.outPID;
				SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			
		}
		else{
			dji_moto_current_to_send[2] = 0;
			SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			osDelay(20);
			small_yaw_init();
			MotoStateInit(&MotoState[7]);
			small_yaw_inited = true;
		}
		osDelay(1);
	}
}

// 小yaw----------------------------------------------

// 小抬升----------------------------------------------
#define SMALL_LIFT_RESET_SPD_P 1
#define SMALL_LIFT_RESET_SPD_I 0
#define SMALL_LIFT_RESET_SPD_D 0

bool small_lift_inited = false;
PidTD pid_small_lift_reset_spd;
TimeTD t_duzhuan_small_lift;
float detect_time_small_lift = 0;
int last_detect_angle_small_lift = 0;
int detect_angle_small_lift = 0;
uint8_t start_stage_small_lift = 1;


void small_lift_init_reset_stage(){
	pidInit(&pid_small_lift_reset_spd, 3000, 10000, SMALL_LIFT_RESET_SPD_P, SMALL_LIFT_RESET_SPD_I, SMALL_LIFT_RESET_SPD_D);
	MotoStateInit(&MotoState[6]);
}

// 小抬升位置初始化 堵转检测
void reset_small_lift(){
	float small_lift_reset_speed = 3000;
	small_lift_init_reset_stage();
	duzhuan_TimeInit(&t_duzhuan_small_lift);
	GetDt(&t_duzhuan_small_lift,MILLISECOND);
	while(small_lift_inited == false){
		// 判断堵转
		GetDt(&t_duzhuan_small_lift,MILLISECOND);
		detect_time_small_lift +=t_duzhuan_small_lift.dt;
		if (detect_time_small_lift > 300){
			detect_time_small_lift = 0;
			last_detect_angle_small_lift =  detect_angle_small_lift;
			detect_angle_small_lift = MotoState[6].angle;
			start_stage_small_lift = 0;
		}
		
		// 不转了
		if((MotoState[6].angle - last_detect_angle_small_lift < -8000) || start_stage_small_lift){
				// 速度环
				pid_calculate(&pid_small_lift_reset_spd, -small_lift_reset_speed, MotoState[6].speed_actual);
				dji_moto_current_to_send[1] = pid_small_lift_reset_spd.outPID;
				SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			
		}
		else{
			dji_moto_current_to_send[1] = 0;
			SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			osDelay(20);
			small_lift_init();
			MotoStateInit(&MotoState[6]);
			small_lift_inited = true;
		}
		osDelay(1);
	}
}

// 小抬升----------------------------------------------

// 小前伸----------------------------------------------
#define SMALL_QS_RESET_SPD_P 1
#define SMALL_QS_RESET_SPD_I 0
#define SMALL_QS_RESET_SPD_D 0

bool small_qs_inited = false;
PidTD pid_small_qs_reset_spd;
TimeTD t_duzhuan_small_qs;
float detect_time_small_qs = 0;
int last_detect_angle_small_qs = 0;
int detect_angle_small_qs = 0;
uint8_t start_stage_small_qs = 1;


void small_qs_init_reset_stage(){
	pidInit(&pid_small_qs_reset_spd, 3000, 10000, SMALL_QS_RESET_SPD_P, SMALL_QS_RESET_SPD_I, SMALL_QS_RESET_SPD_D);
	MotoStateInit(&MotoState[5]);
}

// 小前伸位置初始化 堵转检测
void reset_small_qs(){
	float small_qs_reset_speed = 3000;
	small_qs_init_reset_stage();
	duzhuan_TimeInit(&t_duzhuan_small_qs);
	GetDt(&t_duzhuan_small_qs,MILLISECOND);
	while(small_qs_inited == false){
		// 判断堵转
		GetDt(&t_duzhuan_small_qs,MILLISECOND);
		detect_time_small_qs +=t_duzhuan_small_qs.dt;
		if (detect_time_small_qs > 300){
			detect_time_small_qs = 0;
			last_detect_angle_small_qs =  detect_angle_small_qs;
			detect_angle_small_qs = MotoState[5].angle;
			start_stage_small_qs = 0;
		}
		
		// 不转了
		if((MotoState[5].angle - last_detect_angle_small_qs < -8000) || start_stage_small_qs){
				// 速度环
				pid_calculate(&pid_small_qs_reset_spd, -small_qs_reset_speed, MotoState[5].speed_actual);
				dji_moto_current_to_send[0] = pid_small_qs_reset_spd.outPID;
				SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			
		}
		else{
			dji_moto_current_to_send[0] = 0;
			SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			osDelay(20);
			small_qs_init();
			MotoStateInit(&MotoState[5]);
			small_qs_inited = true;
		}
		osDelay(1);
	}
}

// 云台抬升----------------------------------------------

#define LIFT_CAMERA_RESET_SPD_P 1
#define LIFT_CAMERA_RESET_SPD_I 0
#define LIFT_CAMERA_RESET_SPD_D 0

bool lift_camera_inited = false;
PidTD pid_lift_camera_reset_spd;
TimeTD t_duzhuan_lift_camera;
float detect_time_lift_camera = 0;
int last_detect_angle_lift_camera = 0;
int detect_angle_lift_camera = 0;
uint8_t start_stage_lift_camera = 1;


void lift_camera_init_reset_stage(){
	pidInit(&pid_lift_camera_reset_spd, 3000, 10000, LIFT_CAMERA_RESET_SPD_P, LIFT_CAMERA_RESET_SPD_I, LIFT_CAMERA_RESET_SPD_D);
	MotoStateInit(&MotoState[8]);
}

// 云台抬升位置初始化 堵转检测
void reset_lift_camera(){
	float lift_camera_reset_speed = 3000;
	lift_camera_init_reset_stage();
	duzhuan_TimeInit(&t_duzhuan_lift_camera);
	GetDt(&t_duzhuan_lift_camera,MILLISECOND);
	while(lift_camera_inited == false){
		// 判断堵转
		GetDt(&t_duzhuan_lift_camera,MILLISECOND);
		detect_time_lift_camera +=t_duzhuan_lift_camera.dt;
		if (detect_time_lift_camera > 300){
			detect_time_lift_camera = 0;
			last_detect_angle_lift_camera =  detect_angle_lift_camera;
			detect_angle_lift_camera = MotoState[8].angle;
			start_stage_lift_camera = 0;
		}
		
		// 不转了
		if((MotoState[8].angle - last_detect_angle_lift_camera < -8000) || start_stage_lift_camera){
				// 速度环
				pid_calculate(&pid_lift_camera_reset_spd, -lift_camera_reset_speed, MotoState[8].speed_actual);
				dji_moto_current_to_send[3] = pid_lift_camera_reset_spd.outPID;
				SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			
		}
		else{
			dji_moto_current_to_send[3] = 0;
			SetMotoCurrent(&hcan2, Ahead, dji_moto_current_to_send[0], dji_moto_current_to_send[1], dji_moto_current_to_send[2], dji_moto_current_to_send[3]);
			osDelay(20);
			lift_camera_init();
			MotoStateInit(&MotoState[8]);
			lift_camera_inited = true;
		}
		osDelay(1);
	}
}

// 云台抬升----------------------------------------------
#endif