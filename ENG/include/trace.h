#ifndef __TRACE_H__
#define __TRACE_H__

#include "pid.h"


void trace_init(void);
void trace_current(int current);
void trace_speed(int speed);
void trace_pid(PidTD* pid);
#endif
