#ifndef _MACHINE_H
#define _MACHINE_H
#include "../include/irq_handle.h"

void halt_system();
void enable_int();
void disable_int();
void register_dump();
void _panic();
void interrupt_register_dump(struct cpu_state cpu, struct stack_state stack,
			     unsigned int interrupt);

#endif
