#ifndef __ROBOARM_H__
#define __ROBOARM_H__
#include "main.h"

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