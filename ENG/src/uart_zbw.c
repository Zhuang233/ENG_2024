#include "uart_zbw.h"
#include "RoboArm.h"
#include "JointReset.h"
#include "usart.h"
#include <stdbool.h>
#include "cmsis_os.h"
#include "string.h"
// AC板通信协议
/*
帧头 0x55
中间8字节数据
帧尾 0xaa
*/

uint8_t		USART6_Rx_Buffer[USART6_RX_BUFFER_SIZE] = {0};
uint8_t		USART1_Rx_Buffer[USART1_RX_BUFFER_SIZE] = {0};
uint8_t		UART7_Rx_Buffer[UART7_RX_BUFFER_SIZE] = {0};
uint8_t		UART8_Rx_Buffer[UART8_RX_BUFFER_SIZE] = {0};
extern uint8_t power_less_flag;

DataUnion sync_data_to_c;
BackDataUnion sync_data_from_c;
void sync_data_to_c_init(){
	HY = HY_STD;
	QS = QS_STD;
	PITCH = ARM_ANGLE_STD_1;
	ROLL = ARM_ANGLE_STD_2;
	YAW = ARM_ANGLE_STD_3;
	sync_data_to_c.data.resetable = 0;
	sync_data_to_c.data.power_less_flag = 0;
	sync_data_to_c.data.reset_qs_flag = 0;
	sync_data_to_c.data.last_roll_pos = 0;
}

void decode_five_jiont_back_data(){
	if((USART6_Rx_Buffer[0] == 0x55) && (USART6_Rx_Buffer[16] == 0xAA)){ 
		memcpy(&sync_data_from_c, &USART6_Rx_Buffer, sizeof(sync_data_from_c));
	}
}
void data_sync_uart(){
	if(lift_inited && LIFT_READ > LIFT_MIN_ANGLE_ALLOW_FIVE_JOINT_RESET){
		sync_data_to_c.data.resetable = 1;
	}
	else{
		sync_data_to_c.data.resetable = 0;
	} 
	sync_data_to_c.data.power_less_flag = power_less_flag;
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
	__HAL_UART_CLEAR_IDLEFLAG(&huart8);
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart7, UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart8, UART_IT_IDLE);
  
	
	uart_receive_dma_no_it(&huart6, USART6_Rx_Buffer, USART6_RX_BUFFER_SIZE);
	uart_receive_dma_no_it(&huart1, USART1_Rx_Buffer, USART1_RX_BUFFER_SIZE);
	uart_receive_dma_no_it(&huart7, UART7_Rx_Buffer, UART7_RX_BUFFER_SIZE);
	uart_receive_dma_no_it(&huart8, UART8_Rx_Buffer, UART8_RX_BUFFER_SIZE);
}
