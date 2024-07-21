#ifndef __PWM_H
#define __PWM_H
#include "main.h"


#define RESCUE_L TIM4->CCR3
#define RESCUE_R TIM4->CCR4
#define RESCUE_DOWN_L 810
#define RESCUE_DOWN_R 770
#define RESCUE_UP_L 1250
#define RESCUE_UP_R 300


#define BARRIAR_R TIM4->CCR1
#define BARRIAR_L TIM4->CCR2
#define BARRIAR_DEAUULT_L 650
#define BARRIAR_DEAUULT_R 900
#define BARRIAR_DOWN_L 1230
#define BARRIAR_DOWN_R 330
#define BARRIAR_UP_L 800
#define BARRIAR_UP_R 760


#define CAMERA TIM8->CCR1
#define DEFAULT_CAMERA_ANGEL 850
#define HOLE_CAMERA_ANGEL 850
#define RESCUE_CAMERA_ANGEL 600
#define CARD_CAMERA_ANGEL 600
#define SCREEN_CAMERA_ANGEL 400//tochange
#define GND_CAMERA_ANGEL 600

void PWM_Start(void);

#endif
