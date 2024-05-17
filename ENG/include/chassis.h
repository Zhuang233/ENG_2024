#include "pid.h"
#define MAX_MOVE_RMP	7700                //平移最大速度
#define MAX_ROTATE_RMP  4000   						//自转最大速度


extern PidTD chassis_pid_spd_moto[4];

void chassis_pid_init(void);
void chassis_control(void);
