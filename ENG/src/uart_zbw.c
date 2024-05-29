#include "uart_zbw.h"
#include "usart.h"
#include <stdbool.h>
// AC��ͨ��Э��
/*
֡ͷ 0x55
�м�8�ֽ�����
֡β 0xaa
*/


uint8_t		USART6_Rx_Buffer[USART6_RX_BUFFER_SIZE] = {0};
uint8_t		USART1_Rx_Buffer[USART1_RX_BUFFER_SIZE] = {0};
uint8_t		UART7_Rx_Buffer[USART6_RX_BUFFER_SIZE] = {0};

typedef struct {
    uint8_t buffer[FRAME_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} RingBuffer;

RingBuffer uart_ring_buffer;


uint8_t data_rev_byte = 0;
DataUnion sync_data_to_c;
uint8_t sync_data_from_c[SYNC_FROM_C_SIZE] = {0};



void RingBuffer_Init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
		for(int i=0; i<FRAME_SIZE-2;i++){
			rb->buffer[i] = 0;
		}
		
}

bool RingBuffer_IsFull(RingBuffer *rb) {
    return rb->count == FRAME_SIZE;
}

bool RingBuffer_IsEmpty(RingBuffer *rb) {
    return rb->count == 0;
}


bool RingBuffer_Put(RingBuffer *rb, uint8_t data) {
    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail + 1) % FRAME_SIZE;
    rb->count++;
		rb->count = rb->count % (FRAME_SIZE+1);
    return true;
}


bool RingBuffer_Get(RingBuffer *rb, uint8_t *data) {
    if (RingBuffer_IsEmpty(rb)) {
        return false; // ��������
    }
    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % FRAME_SIZE;
    rb->count--;
    return true;
}






//uint8_t my_ctrl_buffer[FRAME_SIZE]; // ������յ�����
//uint8_t rev_remaid = 0; // ʣ����Ҫ���յ��ֽ���
//uint8_t data_rev_byte; // ������յ���һ���ֽڣ����������Ҫ�ڱ����ã�




//ControlData my_ctrl_data; // �������ݽṹ

//void decode_my_ctrl_data(void) {
//    if (data_rev_byte == FRAME_HEAD && rev_remaid == 0) {
//        // ��ʼ��֡
//        rev_remaid = FRAME_SIZE - 1; // Ԥ�ڽ��յ�ʣ��֡����
//				my_ctrl_buffer[0] = data_rev_byte;
//    } else if (rev_remaid > 0) {
//        // �ȵݼ����Ա��Ӧ����ȷ����������������0��
//        rev_remaid--;
//        my_ctrl_buffer[FRAME_SIZE -1- rev_remaid] = data_rev_byte;

//        // ����Ƿ����������֡
//        if (rev_remaid == 0) {
//            // ������һ���ֽ��Ƿ�Ϊ֡β
//            if (data_rev_byte == FRAME_TAIL) {
//                // �����ݸ��Ƶ��������ݽṹ

//                    my_ctrl_data.bytes[0] = my_ctrl_buffer[1];
//							      my_ctrl_data.bytes[1] = my_ctrl_buffer[2];
//							      my_ctrl_data.bytes[2] = my_ctrl_buffer[27];
//							      my_ctrl_data.bytes[3] = my_ctrl_buffer[28];
//							
//										my_ctrl_data.bytes[4] = my_ctrl_buffer[3];
//							      my_ctrl_data.bytes[5] = my_ctrl_buffer[4];
//							      my_ctrl_data.bytes[6] = my_ctrl_buffer[5];
//							      my_ctrl_data.bytes[7] = my_ctrl_buffer[6];
//							
//                    my_ctrl_data.bytes[8] = my_ctrl_buffer[7];
//							      my_ctrl_data.bytes[9] = my_ctrl_buffer[8];
//							      my_ctrl_data.bytes[10] = my_ctrl_buffer[9];
//							      my_ctrl_data.bytes[11] = my_ctrl_buffer[10];
//							
//										my_ctrl_data.bytes[12] = my_ctrl_buffer[11];
//							      my_ctrl_data.bytes[13] = my_ctrl_buffer[12];
//							      my_ctrl_data.bytes[14] = my_ctrl_buffer[13];
//							      my_ctrl_data.bytes[15] = my_ctrl_buffer[14];
//										
//                    my_ctrl_data.bytes[16] = my_ctrl_buffer[15];
//							      my_ctrl_data.bytes[17] = my_ctrl_buffer[16];
//							      my_ctrl_data.bytes[18] = my_ctrl_buffer[17];
//							      my_ctrl_data.bytes[19] = my_ctrl_buffer[18];
//										
//										my_ctrl_data.bytes[20] = my_ctrl_buffer[19];
//							      my_ctrl_data.bytes[21] = my_ctrl_buffer[20];
//							      my_ctrl_data.bytes[22] = my_ctrl_buffer[21];
//							      my_ctrl_data.bytes[23] = my_ctrl_buffer[22];
//										
//                    my_ctrl_data.bytes[24] = my_ctrl_buffer[23];
//							      my_ctrl_data.bytes[25] = my_ctrl_buffer[24];
//							      my_ctrl_data.bytes[26] = my_ctrl_buffer[25];
//							      my_ctrl_data.bytes[27] = my_ctrl_buffer[26];
//                // ���������Ե���һ������������my_ctrl_data
//            } else {
//                // ������δ��Ԥ��λ�ý��յ�֡β
//                // �������������Ӵ��������־��¼
//            }
//        }
//    }
//}

void data_sync_uart(){
	uint8_t head = FRAME_HEAD;
	uint8_t tail = FRAME_TAIL;
	
	HAL_UART_Transmit(&huart6, &head, 1, 10);
	HAL_UART_Transmit(&huart6, sync_data_to_c.bytes, SYNC_TO_C_SIZE, 100);
	HAL_UART_Transmit(&huart6, &tail, 1, 10);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	if(huart->Instance==USART6)
	{
//		decode_my_ctrl_data();
//		HAL_UART_Receive_IT(&huart6,&data_rev_byte,1);//�ָ������ж�
	}
	if(huart->Instance==UART7){
		
//
	}
}



//void usart_init(){
//	HAL_UART_Receive_IT(&huart6,&data_rev_byte,1);
//	HAL_UART_Receive_IT(&huart7,&USART7_Rx_Buffer,1);
//	RingBuffer_Init(&uart_ring_buffer);
//	RingBuffer_Init2(&my_ctrl_ring_buffer);
//}

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