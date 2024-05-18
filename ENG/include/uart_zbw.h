#ifndef __UART_ZBW_H__
#define __UART_ZBW_H__


#include "main.h"
#define FRAME_SIZE 10

extern uint8_t real_data[FRAME_SIZE-2];
void usart_init(void);

#endif