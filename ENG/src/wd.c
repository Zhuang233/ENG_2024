#include "wd.h"
#include "main.h"

bool hy_reset = false;
bool qsn_reset = false;
bool qsw_reset = false;

void wd_init(){
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET) hy_reset = true;
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET) qsn_reset = true;
	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET) qsw_reset = true;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// 横移微动
	if(GPIO_Pin == GPIO_PIN_9){
		// 检测到上升沿
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_SET){
			hy_reset = true;
		}
		// 检测到下降沿
		else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_9) == GPIO_PIN_RESET){
			hy_reset = false;
		}
	}
	// 前伸微动内
	else if(GPIO_Pin == GPIO_PIN_11){
		// 检测到上升沿
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_SET){
			qsn_reset = true;
		}
		// 检测到下降沿
		else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11) == GPIO_PIN_RESET){
			qsn_reset = false;
		}
	}
	// 前伸微动外
	else if(GPIO_Pin == GPIO_PIN_13){
		// 检测到上升沿
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == GPIO_PIN_SET){
			qsw_reset = true;
		}
		// 检测到下降沿
		else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_13) == GPIO_PIN_RESET){
			qsw_reset = false;
		}
	}
}
