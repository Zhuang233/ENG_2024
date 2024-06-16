#include "uart_zbw.h"
#include "usart.h"
#include <stdbool.h>
#include "cmsis_os.h"
// AC板通信协议
/*
帧头 0x55
中间8字节数据
帧尾 0xaa
*/

uint8_t		USART6_Rx_Buffer[USART6_RX_BUFFER_SIZE] = {0};
uint8_t		USART1_Rx_Buffer[USART1_RX_BUFFER_SIZE] = {0};
uint8_t		UART7_Rx_Buffer[USART6_RX_BUFFER_SIZE] = {0};

DataUnion sync_data_to_c;
uint8_t sync_data_from_c[SYNC_FROM_C_SIZE] = {0};

void data_sync_uart(){
	HAL_UART_Transmit_DMA(&huart6, sync_data_to_c.bytes, SYNC_TO_C_SIZE);
}

void usart_dma_init(void)
{
   	/* open uart idle it */
	__HAL_UART_CLEAR_IDLEFLAG(&huart6);
  __HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_CLEAR_IDLEFLAG(&huart7);
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
  
	UART_Start_Receive_DMA(&huart6, USART6_Rx_Buffer, USART6_RX_BUFFER_SIZE);
	UART_Start_Receive_DMA(&huart1, USART1_Rx_Buffer, USART1_RX_BUFFER_SIZE);
	UART_Start_Receive_DMA(&huart7, UART7_Rx_Buffer, UART7_RX_BUFFER_SIZE);
}
