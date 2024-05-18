#include "uart_zbw.h"
#include "usart.h"

uint8_t data_rev[8] = {0};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	if(huart->Instance==UART7)
	{
		HAL_UART_Receive_IT(&huart7,data_rev,8);//ª÷∏¥Ω” ‹÷–∂œ
	}
}

void usart_init(){
	__HAL_UART_CLEAR_IDLEFLAG(&huart6);
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
	__HAL_UART_CLEAR_IDLEFLAG(&huart7);
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
	HAL_UART_Receive_IT(&huart7,data_rev,8);
}