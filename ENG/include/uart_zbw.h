#ifndef __UART_ZBW_H__
#define __UART_ZBW_H__


#include "main.h"

#define FRAME_HEAD 0x55
#define FRAME_TAIL 0xaa
#define FRAME_SIZE 10 // 接收帧长度
#define SYNC_TO_C_SIZE (sizeof(DataUnion)) //发送数据长度
#define SYNC_FROM_C_SIZE 8// 接收数据长度

typedef struct {
	int32_t qs_pos;
	int32_t hy_pos;
	uint16_t theta1;
	uint16_t theta2;
	uint16_t theta3;
}FiveJointCtrlDataTD;


typedef struct {
	int32_t a;
	int32_t b;
}TestDataTD;

typedef union{
	TestDataTD data;
	uint8_t bytes[sizeof(TestDataTD)];
}TestDataUnion;



typedef union{
	FiveJointCtrlDataTD data;
	uint8_t bytes[sizeof(FiveJointCtrlDataTD)];
}DataUnion;

extern DataUnion sync_data_to_c;
extern uint8_t sync_data_from_c[SYNC_FROM_C_SIZE];

void usart_init(void);
void data_sync_uart(void);
#endif
