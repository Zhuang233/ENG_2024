#ifndef __RELAY_H__
#define __RELAY_H__

#define NO_PUMP

#ifdef OLD_CAR
void pump_top_open(void);
void pump_top_close(void);
void pump_bottom_open(void);
void pump_bottom_close(void);
void xipan_left_open(void);
void xipan_left_close(void);
void xipan_right_open(void);
void xipan_right_close(void);
void xipan_middle_open(void);
void xipan_middle_close(void);
void xipan_top_open(void);
void xipan_top_close(void);
#else
void xipan_left_open(void);
void xipan_left_close(void);
void xipan_right_open(void);
void xipan_right_close(void);
void xipan_bottom_open(void);
void xipan_bottom_close(void);
void xipan_top_open(void);
void xipan_top_close(void);

#endif


#endif
