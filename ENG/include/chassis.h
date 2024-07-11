#include "pid.h"
#define KP_CHASSIS 20
#define KI_CHASSIS 0
#define KD_CHASSIS 0
#define MAX_MOVE_RMP	7700          //平移最大速度
#define MAX_ROTATE_RMP  4000   		  //自转最大速度
#define MOUSE_ROTATE_RMP 3000         //鼠标控制自转速度最大值
#define MOUSE_ROTATE_PARA 10          //鼠标控制自转系数


extern PidTD chassis_pid_spd_moto[4];
extern int16_t vx,vy,vr;
void chassis_pid_init(void);
void chassis_control_RC(void);
void ChassisTask(void);
