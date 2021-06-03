#include <timer.h>
#include <apic.h>
#include <processor.h>
#include <machine.h>

#define LAPIC_TIMER_TICKS (128000 * 64)
#define LAPIC_TIMER_TICKS_PER_CALL (LAPIC_TIMER_TICKS / LAPIC_TIME_DIVISOR)

void timer_setup_lapic_timer(timer_t * timer, void (*handler)(int_state_t * state))
{
    timer->handler = handler;
    timer->ticks_per_call = LAPIC_TIMER_TICKS_PER_CALL;   
    lapic_setup_timer(LAPIC_TIMER_TICKS, PERIODIC, false);
}

void timer_lapic_start()
{
    lapic_timer_enable(true);
}
