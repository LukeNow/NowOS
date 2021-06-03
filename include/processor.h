#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include <kdef.h>
#include <thread.h>
#include <timer.h>

typedef struct processor_t {
	tib_t * current_thread;
	uint16_t id; //APIC id
	bool in_scheduler;
	timer_t timer;
} processor_t;


processor_t * processor_get_info_id(uint16_t cpu_id);
uint16_t processor_get_id();
processor_t * processor_get_info();
void processor_set_info();

#endif
