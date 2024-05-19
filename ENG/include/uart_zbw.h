#ifndef __UART_ZBW_H__
#define __UART_ZBW_H__


#include "main.h"

#define FRAME_HEAD 0x55
#define FRAME_TAIL 0x56
#define FRAME_SIZE 10 // ����֡����
#define SYNC_TO_C_SIZE 8// �������ݳ���
#define SYNC_FROM_C_SIZE 8// �������ݳ���

extern uint8_t sync_data_to_c[SYNC_TO_C_SIZE];
extern uint8_t sync_data_from_c[SYNC_FROM_C_SIZE];

void usart_init(void);
void data_sync_uart(void);
#endif