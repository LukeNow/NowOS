#include <stdint.h>
#include <stddef.h>
#include "../include/pit.h"
#include "../include/io.h"
#include "../include/pic.h"
#include "../include/kprint.h"

size_t ns_ticks;
size_t ns_counter;
size_t ticks;

void (*pit_handler)() = NULL;

void pit_interrupt_handler()
{
	ns_counter += ns_ticks;
	ticks++;
	
	if (pit_handler != NULL)
		pit_handler();
	
}

void register_pit_handler(void (*handler)()) {
	pit_handler = handler;
}

void init_pit()
{
	size_t divisor = 1193181;
	divisor /= 1000;
	ns_ticks = 1000;
	ticks = 0;
	ns_counter = 0;

	outb(0x43, PIT_COMMAND_MODE3 | PIT_COMMAND_FULL | PIT_COMMAND_COUNTER_0);
	outb(0x40, divisor & 0xFF);
	outb(0x40, (divisor >> 8) & 0xFF);
	
}

void pit_reset_ticks()
{
	ticks = 0;
}

size_t pit_get_ticks()
{
	return ticks;
}

size_t pit_get_ns()
{
	return ns_counter;
}

size_t pit_get_ms()
{
	return ns_counter / 1000000;
}
