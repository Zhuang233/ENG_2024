#include "uart_zbw.h"
#include "usart.h"
#include <stdbool.h>
// AC��ͨ��Э��
/*
֡ͷ 0x55
�м�8�ֽ�����
֡β 0xaa
*/





typedef struct {
    uint8_t buffer[FRAME_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} RingBuffer;
typedef struct {
    uint8_t buffer[FRAME_SIZE2];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} RingBuffer2;
RingBuffer uart_ring_buffer;
RingBuffer2 my_ctrl_ring_buffer;


uint8_t data_rev_byte = 0;
DataUnion sync_data_to_c;
uint8_t sync_data_from_c[SYNC_FROM_C_SIZE] = {0};
uint8_t data_rev_byte_2 = 0;



void RingBuffer_Init(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
		for(int i=0; i<FRAME_SIZE-2;i++){
			rb->buffer[i] = 0;
		}
		
}

void RingBuffer_Init2(RingBuffer2 *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
		for(int i=0; i<FRAME_SIZE2-2;i++){
			rb->buffer[i] = 0;
		}
		
}

bool RingBuffer_IsFull(RingBuffer *rb) {
    return rb->count == FRAME_SIZE;
}

bool RingBuffer_IsEmpty(RingBuffer *rb) {
    return rb->count == 0;
}
bool RingBuffer_IsEmpty2(RingBuffer2 *rb) {
    return rb->count == 0;
}

bool RingBuffer_Put(RingBuffer *rb, uint8_t data) {
    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail + 1) % FRAME_SIZE;
    rb->count++;
		rb->count = rb->count % (FRAME_SIZE+1);
    return true;
}

bool RingBuffer_Put2(RingBuffer2 *rb, uint8_t data) {
    rb->buffer[rb->tail] = data;
    rb->tail = (rb->tail + 1) % FRAME_SIZE2;
    rb->count++;
		rb->count = rb->count % (FRAME_SIZE2+1);
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
bool RingBuffer_Get2(RingBuffer2 *rb, uint8_t *data) {
    if (RingBuffer_IsEmpty2(rb)) {
        return false; // ��������
    }
    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % FRAME_SIZE2;
    rb->count--;
    return true;
}


//void decode_uart_rev_data(){
//	uint8_t test_byte = 0;
//	RingBuffer_Put(&uart_ring_buffer,data_rev_byte);
//	if(data_rev_byte == FRAME_TAIL){
//		RingBuffer_Get(&uart_ring_buffer,&test_byte);
//		if(test_byte == FRAME_HEAD){
//			for(int i=0; i<FRAME_SIZE-2;i++){
//				RingBuffer_Get(&uart_ring_buffer,(my_ctrl_data.bytes+i));
//			}
//			RingBuffer_Get(&uart_ring_buffer,&test_byte);
//		}
//		else{
//			RingBuffer_Init(&uart_ring_buffer);
//		}
//		
//	}
//}

//uint8_t my_ctrl_buffer[30] = {0};
//uint8_t rev_remaid = 0;
//void decode_my_ctrl_data(){
////	uint8_t test_byte = 0;
////	RingBuffer_Put2(&my_ctrl_ring_buffer,data_rev_byte_2);
////	if(data_rev_byte == FRAME_TAIL){
////		RingBuffer_Get2(&my_ctrl_ring_buffer,&test_byte);
////		if(test_byte == FRAME_HEAD){
////			for(int i=0; i<FRAME_SIZE2-2;i++){
////				RingBuffer_Get2(&my_ctrl_ring_buffer,(my_ctrl_data.bytes+i));
////			}
////			RingBuffer_Get2(&my_ctrl_ring_buffer,&test_byte);
////		}
////		else{
////			RingBuffer_Init2(&my_ctrl_ring_buffer);
////		}
////	}
//	if(data_rev_byte == FRAME_HEAD && rev_remaid==0){
//		rev_remaid = 29;
//	}
//	else if(rev_remaid > 0){
//		my_ctrl_buffer[30-rev_remaid] = data_rev_byte;
//		rev_remaid--;
//		if(rev_remaid == 0){
//			if(data_rev_byte==FRAME_TAIL){
//				for(int i=0; i<26;i++){
//					my_ctrl_data.bytes[i] = my_ctrl_buffer[i];
//				}
//				
//			}
//		}
//	}
//}





uint8_t my_ctrl_buffer[FRAME_SIZE]; // ������յ�����
uint8_t rev_remaid = 0; // ʣ����Ҫ���յ��ֽ���
uint8_t data_rev_byte; // ������յ���һ���ֽڣ����������Ҫ�ڱ����ã�




ControlData my_ctrl_data; // �������ݽṹ

void decode_my_ctrl_data(void) {
    if (data_rev_byte == FRAME_HEAD && rev_remaid == 0) {
        // ��ʼ��֡
        rev_remaid = FRAME_SIZE - 1; // Ԥ�ڽ��յ�ʣ��֡����
				my_ctrl_buffer[0] = data_rev_byte;
    } else if (rev_remaid > 0) {
        // �ȵݼ����Ա��Ӧ����ȷ����������������0��
        rev_remaid--;
        my_ctrl_buffer[FRAME_SIZE -1- rev_remaid] = data_rev_byte;

        // ����Ƿ����������֡
        if (rev_remaid == 0) {
            // ������һ���ֽ��Ƿ�Ϊ֡β
            if (data_rev_byte == FRAME_TAIL) {
                // �����ݸ��Ƶ��������ݽṹ

                    my_ctrl_data.bytes[0] = my_ctrl_buffer[1];
							      my_ctrl_data.bytes[1] = my_ctrl_buffer[2];
							      my_ctrl_data.bytes[2] = my_ctrl_buffer[27];
							      my_ctrl_data.bytes[3] = my_ctrl_buffer[28];
							
										my_ctrl_data.bytes[4] = my_ctrl_buffer[3];
							      my_ctrl_data.bytes[5] = my_ctrl_buffer[4];
							      my_ctrl_data.bytes[6] = my_ctrl_buffer[5];
							      my_ctrl_data.bytes[7] = my_ctrl_buffer[6];
							
                    my_ctrl_data.bytes[8] = my_ctrl_buffer[7];
							      my_ctrl_data.bytes[9] = my_ctrl_buffer[8];
							      my_ctrl_data.bytes[10] = my_ctrl_buffer[9];
							      my_ctrl_data.bytes[11] = my_ctrl_buffer[10];
							
										my_ctrl_data.bytes[12] = my_ctrl_buffer[11];
							      my_ctrl_data.bytes[13] = my_ctrl_buffer[12];
							      my_ctrl_data.bytes[14] = my_ctrl_buffer[13];
							      my_ctrl_data.bytes[15] = my_ctrl_buffer[14];
										
                    my_ctrl_data.bytes[16] = my_ctrl_buffer[15];
							      my_ctrl_data.bytes[17] = my_ctrl_buffer[16];
							      my_ctrl_data.bytes[18] = my_ctrl_buffer[17];
							      my_ctrl_data.bytes[19] = my_ctrl_buffer[18];
										
										my_ctrl_data.bytes[20] = my_ctrl_buffer[19];
							      my_ctrl_data.bytes[21] = my_ctrl_buffer[20];
							      my_ctrl_data.bytes[22] = my_ctrl_buffer[21];
							      my_ctrl_data.bytes[23] = my_ctrl_buffer[22];
										
                    my_ctrl_data.bytes[24] = my_ctrl_buffer[23];
							      my_ctrl_data.bytes[25] = my_ctrl_buffer[24];
							      my_ctrl_data.bytes[26] = my_ctrl_buffer[25];
							      my_ctrl_data.bytes[27] = my_ctrl_buffer[26];
                // ���������Ե���һ������������my_ctrl_data
            } else {
                // ������δ��Ԥ��λ�ý��յ�֡β
                // �������������Ӵ��������־��¼
            }
        }
    }
}

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
		decode_my_ctrl_data();
		HAL_UART_Receive_IT(&huart6,&data_rev_byte,1);//�ָ������ж�
	}
	if(huart->Instance==UART7){

//
	}
}

void usart_init(){
	HAL_UART_Receive_IT(&huart6,&data_rev_byte,1);
//	HAL_UART_Receive_IT(&huart7,aaa,100);
	RingBuffer_Init(&uart_ring_buffer);
	RingBuffer_Init2(&my_ctrl_ring_buffer);
}