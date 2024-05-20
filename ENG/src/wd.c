#include "wd.h"
#include "main.h"

bool lift_reset = false;

void wd_init(){
	if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0) == GPIO_PIN_SET) lift_reset = true;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	if(GPIO_Pin == GPIO_PIN_0){
		// 检测到上升沿
		if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0) == GPIO_PIN_SET){
			lift_reset = true;
		}
		// 检测到下降沿
		else if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_0) == GPIO_PIN_RESET){
			lift_reset = false;
		}
	}
}
