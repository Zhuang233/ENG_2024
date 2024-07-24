#ifndef __PWM_H
#define __PWM_H
#include "main.h"

// 2600左 中1580 右800 云台yaw G脚 PD13 tim4ch2
// 下1100 中1500 上1850 云台pitch H脚 PD12 tim4ch1 
#define CAMERA_YAW_MAX 2600
#define CAMERA_YAW_MIN 800
#define CAMERA_YAW_STD 1580

#define CAMERA_PITCH_MAX 1850
#define CAMERA_PITCH_MIN 1100
#define CAMERA_PITCH_STD 1500

#define CAMERA_PITCH_SLIVER 1100
#define CAMERA_YAW_SLIVER 1700

#define CAMERA_PITCH_GOLD 1100
#define CAMERA_YAW_GOLD 1700

#define CAMERA_PITCH TIM4->CCR1
#define CAMERA_YAW TIM4->CCR2

void PWM_Start(void);

#endif
