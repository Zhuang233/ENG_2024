#include "RcDriver.h"
#include "usart.h"


#define USART1_RX_BUFFER_SIZE 128
#define RC_FRAME_LENGTH 18

extern DMA_HandleTypeDef hdma_usart1_rx;

RC_Ctl_t RC_CtrlData;
uint8_t		USART1_Rx_Buffer[USART1_RX_BUFFER_SIZE] = {0};

/**
  * @brief      enable global uart it and do not use DMA transfer done it
  * @param[in]  huart: uart IRQHandler id
  * @param[in]  pData: receive buff 
  * @param[in]  Size:  buff size
  * @retval     set success or fail
  */
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
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
  
  uart_receive_dma_no_it(&huart1, USART1_Rx_Buffer, USART1_RX_BUFFER_SIZE);
}


void DBUS_decode(uint8_t *buff)
{
	uint8_t temp_sw1 = ((buff[5] >> 4) & 0x000C) >> 2;
	uint8_t temp_sw2 = (buff[5] >> 4) & 0x0003;
	
	if(temp_sw1 != RC_CtrlData.rc.sw1){
		RC_CtrlData.rc.sw1_last = RC_CtrlData.rc.sw1;
	}
	if(temp_sw2 != RC_CtrlData.rc.sw2){
		RC_CtrlData.rc.sw2_last = RC_CtrlData.rc.sw2;
	}
	RC_CtrlData.rc.sw1 = temp_sw1;
	RC_CtrlData.rc.sw2 = temp_sw2;

	RC_CtrlData.rc.ch1 = (buff[0] | buff[1] << 8) & 0x07FF;
	RC_CtrlData.rc.ch1 -= 1024;
	RC_CtrlData.rc.ch2 = (buff[1] >> 3 | buff[2] << 5) & 0x07FF;
	RC_CtrlData.rc.ch2 -= 1024;
	RC_CtrlData.rc.ch3 = (buff[2] >> 6 | buff[3] << 2 | buff[4] << 10) & 0x07FF;
	RC_CtrlData.rc.ch3 -= 1024;
	RC_CtrlData.rc.ch4 = (buff[4] >> 1 | buff[5] << 7) & 0x07FF;
	RC_CtrlData.rc.ch4 -= 1024;

	RC_CtrlData.mouse.x = ((int16_t)buff[6]) | ((int16_t)buff[7] << 8);
	RC_CtrlData.mouse.y = ((int16_t)buff[8]) | ((int16_t)buff[9] << 8);
	RC_CtrlData.mouse.z = ((int16_t)buff[10]) | ((int16_t)buff[11] << 8);

	RC_CtrlData.mouse.press_l = buff[12];
	RC_CtrlData.mouse.press_r = buff[13];

	RC_CtrlData.key.info = ((int16_t)buff[14]) | ((int16_t)buff[15] << 8);
	
	if(RC_CtrlData.rc.ch1<=6 && RC_CtrlData.rc.ch1>=-6)	RC_CtrlData.rc.ch1=0;
	if(RC_CtrlData.rc.ch2<=6 && RC_CtrlData.rc.ch2>=-6)	RC_CtrlData.rc.ch2=0;
	if(RC_CtrlData.rc.ch3<=6 && RC_CtrlData.rc.ch3>=-6)	RC_CtrlData.rc.ch3=0;
	if(RC_CtrlData.rc.ch4<=6 && RC_CtrlData.rc.ch4>=-6)	RC_CtrlData.rc.ch4=0;

	if(RC_CtrlData.rc.ch1>650)	RC_CtrlData.rc.ch1 =  650;
	if(RC_CtrlData.rc.ch1<-650)	RC_CtrlData.rc.ch1 = -650;
	if(RC_CtrlData.rc.ch2>650)	RC_CtrlData.rc.ch2 =  650;
	if(RC_CtrlData.rc.ch2<-650)	RC_CtrlData.rc.ch2 = -650;
	if(RC_CtrlData.rc.ch3>650)	RC_CtrlData.rc.ch3 =  650;
	if(RC_CtrlData.rc.ch3<-650)	RC_CtrlData.rc.ch3 = -650;
	if(RC_CtrlData.rc.ch4>650)	RC_CtrlData.rc.ch4 =  650;
	if(RC_CtrlData.rc.ch4<-650)	RC_CtrlData.rc.ch4 = -650;

}


void Rc_IRQ(){
	if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_IDLE))
	{
    /* clear idle it flag avoid idle interrupt all the time */
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    
		/* clear DMA transfer complete flag */
		__HAL_DMA_DISABLE(&hdma_usart1_rx);

		/* handle dbus data dbus_buf from DMA */
		if ((USART1_RX_BUFFER_SIZE - hdma_usart1_rx.Instance->NDTR) == RC_FRAME_LENGTH)
		{
			DBUS_decode(USART1_Rx_Buffer);	
		}
		
		/* restart dma transmission */
		__HAL_DMA_SET_COUNTER(huart1.hdmarx, USART1_RX_BUFFER_SIZE);
		__HAL_DMA_ENABLE(&hdma_usart1_rx);

	}
}

