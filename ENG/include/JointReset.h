#ifndef __JOINTRESET_H__
#define __JOINTRESET_H__
#include <stdbool.h>
#include "main.h"
extern bool lift_inited;
extern bool expand_inited;
extern int16_t dji_moto_current_to_send[3];
void reset_lift(void);
void reset_expand(void);

#endif
