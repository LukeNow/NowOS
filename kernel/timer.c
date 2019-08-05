#include <stddef.h>
#include "../include/timer.h"
#include "../include/pit.h"

size_t timer_get_ms()
{
	return pit_get_ms();
}

size_t timer_get_ns()
{
	return pit_get_ns();
}

void sleep_ms(unsigned int ms)
{
	size_t start_time = timer_get_ms();
	for (;;) {
		size_t curr_time = timer_get_ms();
		if (curr_time - start_time >= ms)
			break;
	}
}
