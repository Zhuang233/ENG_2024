#include "relay.h"
#include "main.h"

// U A2 吸盘左 
// V A3 吸盘中
// W I5 吸盘右
// X I6 气泵下
// Y I7 吸盘上
// Z I2 气泵上

void pump_top_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_2, GPIO_PIN_SET);
}
void pump_top_close(){
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
void pump_bottom_close(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
}
void xipan_left_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_SET);
}
void xipan_left_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_5, GPIO_PIN_RESET);
}
void xipan_right_open(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
}
void xipan_right_close(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
}
void xipan_top_open(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_RESET);
}
void xipan_top_close(){
	HAL_GPIO_WritePin(GPIOI, GPIO_PIN_7, GPIO_PIN_SET);
}
