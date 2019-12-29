#ifndef _PIT_H
#define _PIT_H

#include <stddef.h>

#define PIT_COMMAND_BCD				0x1

#define PIT_COMMAND_MODE0			0x0		/* Interrupt on terminal count */
#define PIT_COMMNAD_MODE1			0x2		/* hardware re-triggerable one-shot */
#define PIT_COMMAND_MODE2			0x4		/* rate generator */
#define PIT_COMMAND_MODE3			0x6		/* square wave generator */
#define PIT_COMMNAD_MODE4			0x8		/* software triggered strobe */
#define PIT_COMMAND_MODE5			0xA		/* hardware triggered strobe */
#define PIT_COMMAND_MODE6			0xC		/* rate generator, same as 010b */
#define PIT_COMMAND_MODE7			0xF		/* square wave generator, same as 011b */

#define PIT_COMMAND_LATCHCOUNT		0x0		/* Latch count value command */
#define PIT_COMMAND_LOWBYTE			0x10	/* Access mode: lobyte only */
#define PIT_COMMAND_HIGHBYTE		0x20	/* Access mode: hibyte only */
#define	PIT_COMMAND_FULL			0x30	/* Access mode: lobyte/hibyte */

#define	PIT_COMMAND_COUNTER_0		0
#define	PIT_COMMAND_COUNTER_1		0x40
#define	PIT_COMMAND_COUNTER_2		0x80

void pit_interrupt_handler();
void register_pit_handler(void (*handler)());
void init_pit();
void pit_reset_ticks();
size_t pit_get_ticks();
size_t pit_get_ns();
size_t pit_get_ms();

#endif
