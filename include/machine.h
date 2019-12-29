#ifndef _MACHINE_H
#define _MACHINE_H
#include "../include/irq_handle.h"

void halt_system();
void enable_int();
void disable_int();
void nop();
void register_dump();
void _panic();
void interrupt_register_dump(int_state_t * state);

#endif
