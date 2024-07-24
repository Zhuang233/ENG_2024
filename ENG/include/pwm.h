#ifndef __PWM_H
#define __PWM_H
#include "main.h"

// 2600�� ��1580 ��800 ��̨yaw G�� PD13 tim4ch2
// ��1100 ��1500 ��1850 ��̨pitch H�� PD12 tim4ch1 
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
