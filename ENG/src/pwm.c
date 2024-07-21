#include "pwm.h"

extern TIM_HandleTypeDef htim4;

void PWM_Start(void)
{
  HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
  TIM4->CCR1 = 900;
}

