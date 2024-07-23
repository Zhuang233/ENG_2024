#ifndef __UART_ZBW_H__
#define __UART_ZBW_H__


#include "main.h"

#define FRAME_HEAD 0x55
#define FRAME_TAIL 0xaa
#define SYNC_TO_C_SIZE (sizeof(DataUnion)) //发送数据长度
#define SYNC_FROM_C_SIZE 26// 接收数据长度

#define FRAME_SIZE2 30 // 接收帧长度

//MyCtrlUnion my_ctrl_data;
#define FRAME_SIZE 30
#define DATA_SIZE 28

# define USART6_RX_BUFFER_SIZE 128
# define USART1_RX_BUFFER_SIZE 128
# define UART7_RX_BUFFER_SIZE 128
extern uint8_t		USART6_Rx_Buffer[USART6_RX_BUFFER_SIZE];
extern uint8_t		USART1_Rx_Buffer[USART1_RX_BUFFER_SIZE];
extern uint8_t		UART7_Rx_Buffer[UART7_RX_BUFFER_SIZE];

typedef struct __attribute__((packed)) {
	uint8_t key1;
	uint8_t key2;
	float vx;
	float vy;
	float vz;
	float roll;
	float pitch;
	float yaw;
} MyCtrlTD;

typedef union{
	MyCtrlTD data;
	uint8_t bytes[sizeof(MyCtrlTD)];
}MyCtrlUnion;

typedef struct __attribute__((packed)){
	uint8_t head;
	int32_t qs_pos;
	int32_t hy_pos;
	uint16_t theta1;
	uint16_t theta2;
	uint16_t theta3;
	uint8_t resetable;
	uint8_t power_less_flag;
	uint8_t tail;
}FiveJointCtrlDataTD;

typedef struct __attribute__((packed)){
	uint8_t key1;
	uint8_t key2;
	uint8_t temp[2];
	float vx;
	float vy;
	float vz;
	float roll;
	float pitch;
	float yaw;
}MyCtrlDataTD;

//48bit(9+30+9)

typedef union{
	MyCtrlDataTD data;
	uint8_t bytes[sizeof(MyCtrlDataTD)];
}MyCtrlDataUnion;


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

typedef union {
	MyCtrlDataTD data;
    uint8_t bytes[DATA_SIZE]; // 控制数据
} ControlData;


extern DataUnion sync_data_to_c;
extern uint8_t sync_data_from_c[SYNC_FROM_C_SIZE];
extern ControlData my_ctrl_data;

void usart_init(void);
int uart_receive_dma_no_it(UART_HandleTypeDef* huart, uint8_t* pData, uint32_t Size);
void usart_dma_init(void);
void data_sync_uart(void);
void sync_data_to_c_init(void);
#endif
