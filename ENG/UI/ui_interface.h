//
// Created by bismarckkk on 2024/2/17.
//

#ifndef SERIAL_TEST_UI_INTERFACE_H
#define SERIAL_TEST_UI_INTERFACE_H

#include <stdio.h>
#include "ui_types.h"
#include "usart.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "queue.h"

extern int ui_self_id;

void print_message(const uint8_t* message, int length);

//#define SEND_MESSAGE(message, length) print_message(message, length)

typedef struct{
	uint8_t* ui_msg;
	int len;
}ui_msg_t;
extern ui_msg_t ui_msg_send_buf;
extern QueueHandle_t ui_queue;

#define SEND_MESSAGE(message, length) {HAL_UART_Transmit(&huart7, message, length, 100); osDelay(150);}
// #define SEND_MESSAGE(message, _len)  \
// 	ui_msg_send_buf.ui_msg = message; \
// 	ui_msg_send_buf.len = _len; \
// 	xQueueSend(ui_queue, &ui_msg_send_buf, portTICK_RATE_MS * 100);

void ui_proc_1_frame(ui_1_frame_t *msg);
void ui_proc_2_frame(ui_2_frame_t *msg);
void ui_proc_5_frame(ui_5_frame_t *msg);
void ui_proc_7_frame(ui_7_frame_t *msg);
void ui_proc_string_frame(ui_string_frame_t *msg);
void ui_proc_delete_frame(ui_delete_frame_t *msg);

void hind_line(ui_interface_line_t* line);

#endif //SERIAL_TEST_UI_INTERFACE_H
