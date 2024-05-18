#include "uart_zbw.h"
#include "usart.h"
#include <stdbool.h>
// AC板通信协议
/*
帧头 0x55
中间8字节数据
帧尾 0xaa
*/




#define FRAME_HEAD 0x55
#define FRAME_TAIL 0x56
int16_t success_count=0;
int16_t fail_count=0;

typedef struct {
    uint8_t buffer[FRAME_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} RingBuffer;

RingBuffer uart_ring_buffer;
uint8_t data_rev_byte = 0;
uint8_t real_data[FRAME_SIZE-2] = {0};


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
        return false; // 缓冲区空
    }
    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % FRAME_SIZE;
    rb->count--;
    return true;
}


void decode_uart_rev_data(){
	uint8_t test_byte = 0;
	RingBuffer_Put(&uart_ring_buffer,data_rev_byte);
	if(data_rev_byte == FRAME_TAIL){
		RingBuffer_Get(&uart_ring_buffer,&test_byte);
		if(test_byte == FRAME_HEAD){
			for(int i=0; i<FRAME_SIZE-2;i++){
				RingBuffer_Get(&uart_ring_buffer,(real_data+i));
			}
			RingBuffer_Get(&uart_ring_buffer,&test_byte);
			success_count++;
		}
		else{
			RingBuffer_Init(&uart_ring_buffer);
			fail_count++;
		}
		
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	
	if(huart->Instance==UART7)
	{
		decode_uart_rev_data();
		HAL_UART_Receive_IT(&huart7,&data_rev_byte,1);//恢复接受中断
	}
}

void usart_init(){
	HAL_UART_Receive_IT(&huart7,&data_rev_byte,1);
	RingBuffer_Init(&uart_ring_buffer);
}