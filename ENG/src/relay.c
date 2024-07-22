#include "relay.h"
#include "main.h"


uint8_t pump_flag = 0;

#ifdef OLD_CAR
// U A2 吸盘左 
// V A3 吸盘中
// W I5 吸盘右
// X I6 气泵下
// Y I7 吸盘上
// Z I2 气泵上


uint8_t xipan_left_closed = 1;
uint8_t xipan_right_closed = 1; 


void pump_top_open(){
	pump_flag = 1;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_2, GPIO_PIN_SET);
}
void pump_top_close(){
	pump_flag = 0;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_2, GPIO_PIN_RESET);
}
void xipan_middle_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, GPIO_PIN_SET);
}
void xipan_middle_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, GPIO_PIN_RESET);
}
void pump_bottom_open(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
}

// 关下面泵函数被调用时，只有两个吸盘全关了才能真的关
void pump_bottom_close(){
	if(xipan_left_closed == 1 && xipan_right_closed == 1){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
	}
}

void xipan_left_open(){
	xipan_left_closed = 0;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);
}
void xipan_left_close(){
	xipan_left_closed = 1;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);
}
void xipan_right_open(){
	xipan_right_closed = 0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}
void xipan_right_close(){
	xipan_right_closed = 1;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}
void xipan_top_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_RESET);
}
void xipan_top_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_SET);
}
#else
// W I5 左吸盘
// X I6 右吸盘
// Y I7 小臂吸盘
// Z I2 大臂吸盘

void xipan_left_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);
}
void xipan_left_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);
}
void xipan_right_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, GPIO_PIN_SET);
}
void xipan_right_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_6, GPIO_PIN_RESET);
}
void xipan_top_open(){
	pump_flag = 1;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_2, GPIO_PIN_SET);
}
void xipan_top_close(){
	pump_flag = 0;
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_2, GPIO_PIN_RESET);
}
void xipan_bottom_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_SET);
}
void xipan_bottom_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_RESET);
}
#endif