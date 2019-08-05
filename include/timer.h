#ifndef _TIMER_H
#define _TIMER_H

#include <stddef.h>

void sleep_ms(unsigned int ms);
size_t timer_get_ms();
size_t timer_get_ns();

#endif
