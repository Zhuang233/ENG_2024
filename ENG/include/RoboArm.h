#ifndef __ROBOARM_H__
#define __ROBOARM_H__
#include "main.h"
//pitch
#define ARM_ANGLE_MAX_1 45077
#define ARM_ANGLE_MIN_1 12650
//#define ARM_ANGLE_STD_1 28492
#define ARM_ANGLE_STD_1 ARM_ANGLE_MIN_1
//roll
#define ARM_ANGLE_MAX_2 42583
#define ARM_ANGLE_MIN_2 9858
#define ARM_ANGLE_STD_2 25823


//yall
#define ARM_ANGLE_MAX_3 40000
#define ARM_ANGLE_MIN_3 10000
#define ARM_ANGLE_STD_3 24370


void RoboArm_Pos_Init(void);
void RoboArm_RC_Ctrl(void);
void RoboArm_RC_Ctrl_Fixed_Point(void);
void lift_init(void);
void expand_init(void);
void flip_init(void);
void small_lift_init(void);
void Update_Lift_Pos(void);
void Update_Expand_Pos(void);
void Update_Small_Lift_Pos(void);
void Update_Flip_Pos(void);
int32_t virtual_link(int32_t x);
#endif