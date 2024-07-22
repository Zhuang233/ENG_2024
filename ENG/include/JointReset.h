#ifndef __JOINTRESET_H__
#define __JOINTRESET_H__
#include <stdbool.h>
#include "main.h"
extern bool lift_inited;
extern int16_t dji_moto_current_to_send[4];
void reset_lift(void);
#ifdef OLD_CAR
extern bool expand_inited;
void reset_expand(void);
#else

extern bool small_yaw_inited;
extern bool lift_camera_inited;
extern bool small_lift_inited;
extern bool small_qs_inited;

void reset_small_yaw(void);
void reset_small_lift(void);
void reset_small_qs(void);
void reset_lift_camera(void);
#endif
#endif
