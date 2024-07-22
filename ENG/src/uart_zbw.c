#include "uart_zbw.h"
#include "RoboArm.h"
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
void sync_data_to_c_init(){
	sync_data_to_c.data.hy_pos = -194951;
	sync_data_to_c.data.qs_pos = 0;
	sync_data_to_c.data.theta1 = ARM_ANGLE_STD_1;
	sync_data_to_c.data.theta2 = ARM_ANGLE_STD_2;
	sync_data_to_c.data.theta3 = ARM_ANGLE_STD_3;
}

void data_sync_uart(){
	HAL_UART_Transmit_DMA(&huart6, sync_data_to_c.bytes, SYNC_TO_C_SIZE);
}

int uart_receive_dma_no_it(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size)
{
  uint32_t tmp1 = 0;

  tmp1 = huart->RxState;
	
	if (tmp1 == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0))
		{
			return HAL_ERROR;
		}

		huart->pRxBuffPtr = pData;
		huart->RxXferSize = Size;
		huart->ErrorCode  = HAL_UART_ERROR_NONE;

		/* Enable the DMA Stream */
		HAL_DMA_Start(huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)pData, Size);

		/* 
		 * Enable the DMA transfer for the receiver request by setting the DMAR bit
		 * in the UART CR3 register 
		 */
		SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
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
  
	
	uart_receive_dma_no_it(&huart6, USART6_Rx_Buffer, USART6_RX_BUFFER_SIZE);
	uart_receive_dma_no_it(&huart1, USART1_Rx_Buffer, USART1_RX_BUFFER_SIZE);
	uart_receive_dma_no_it(&huart7, UART7_Rx_Buffer, UART7_RX_BUFFER_SIZE);
}
