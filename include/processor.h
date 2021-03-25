#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include <kdef.h>
#include <thread.h>

typedef struct processor_t {
	tib_t * current_thread;
	uint16_t id; //APIC id
	bool in_scheduler;
} processor_t;

uint16_t processor_get_id();
processor_t * processor_get_info();
void processor_set_info();
#endif
