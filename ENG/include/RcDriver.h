#ifndef __RC_DRIVER_H
#define __RC_DRIVER_H
#include "main.h"

typedef __packed struct
{
	/* rocker channel information */
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int16_t ch4;

	/* left and right lever information */
	uint8_t sw1;
	uint8_t sw2;
	uint8_t sw1_last;
	uint8_t sw2_last;
} rc_info_t;

typedef __packed struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t last_press_l;
	uint8_t last_press_r;
	uint8_t press_l;
	uint8_t press_r;
} rc_info_mouse;

typedef	__packed struct
{
	uint16_t info;
}rc_info_key;

typedef __packed struct
{
	rc_info_t rc;
	rc_info_mouse mouse;
	rc_info_key key;
} RC_Ctl_t;

extern RC_Ctl_t RC_CtrlData;

void Rc_IRQ(void);
void usart_dma_init(void);

#endif
