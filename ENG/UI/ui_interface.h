//
// Created by bismarckkk on 2024/2/17.
//

#ifndef SERIAL_TEST_UI_INTERFACE_H
#define SERIAL_TEST_UI_INTERFACE_H

#include <stdio.h>
#include "ui_types.h"
#include "usart.h"
#include "cmsis_os2.h"

extern int ui_self_id;

void print_message(const uint8_t* message, int length);

//#define SEND_MESSAGE(message, length) print_message(message, length)
#define SEND_MESSAGE(message, len) {HAL_UART_Transmit(&huart7, message, len, 100); osDelay(100);}

void ui_proc_1_frame(ui_1_frame_t *msg);
void ui_proc_2_frame(ui_2_frame_t *msg);
void ui_proc_5_frame(ui_5_frame_t *msg);
void ui_proc_7_frame(ui_7_frame_t *msg);
void ui_proc_string_frame(ui_string_frame_t *msg);
void ui_proc_delete_frame(ui_delete_frame_t *msg);

#endif //SERIAL_TEST_UI_INTERFACE_H
