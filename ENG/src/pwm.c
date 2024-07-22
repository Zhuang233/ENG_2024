#include "pwm.h"

extern TIM_HandleTypeDef htim4;

void PWM_Start(void)
{
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2);
  CAMERA_PITCH = CAMERA_PITCH_STD;
	CAMERA_YAW = CAMERA_YAW_STD;
}

