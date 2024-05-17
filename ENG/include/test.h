#ifndef __TEST_H__
#define __TEST_H__
#include "pid.h"

extern PidTD pidtest;

void test_pid_spd(void);
void test_pid_pos(void);
void test_pid_pos_init(void);
void test_rc_moto(void);
void test_wd(void);
void test_reset_hy(void);
void test_reset_qs(void);
#endif
