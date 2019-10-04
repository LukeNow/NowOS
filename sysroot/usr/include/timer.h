#ifndef _TIMER_H
#define _TIMER_H

#include <stddef.h>
#include "../include/scheduler.h"

void timer_task();

void timer_add_sleep_list(wait_control_block_t *task);
void update_time_used();
void sleep_ms(unsigned int ms);

size_t timer_get_ms();
size_t timer_get_ns();

#endif
