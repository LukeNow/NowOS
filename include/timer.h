#ifndef _TIMER_H
#define _TIMER_H

#include <kdef.h>
#include <machine.h>

typedef struct timer_t {
	void (*handler)(int_state_t * state);
    uint32_t ticks_per_call;
} timer_t;

void timer_setup_lapic_timer(timer_t * timer, void (*handler)(int_state_t * state));
void timer_lapic_start();

#endif