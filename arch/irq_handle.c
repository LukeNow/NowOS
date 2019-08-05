#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/io.h"
#include "../include/pic.h"
#include "../include/kprint.h"
#include "../include/machine.h"
#include "../include/processor.h"
#include "../include/irq_handle.h"
#include "../include/pit.h"

/* INT vec 0 */
void divide_by_zero_handler(struct cpu_state cpu, struct stack_state stack)
{
	
	kprint(ERROR, "Divide by zero interrupt called\n");
	//interrupt_register_dump(0);
	halt_system();
}

/* INT vec 2 */
void nmi_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(WARN, "NMI interrupt called\n");
}

/* INT vec 4 */
void overflow_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "Overflow interrupt called\n");
	halt_system();
}

/* INT vec 5 */
void bound_range_exceeded_handler(struct cpu_state cpu, 
				  struct stack_state stack)
{
	kprint(ERROR, "Bound range exceeded interrupt called\n");
	halt_system();
}

/* INT vec 6 */
void invalid_opcode_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "Invalid op code interrupt called\n");
	halt_system();
}

/* INT vec 7 */
void device_not_available_handler(struct cpu_state cpu, 
				  struct stack_state stack)
{
	kprint(WARN, "Device not available interrupt called\n");
}

/* INT vec 8 */
void double_fault_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "Double fault interrupt called\n");
	halt_system();
}

/* INT vec 10 */
void invalid_tss_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "Invalid TSS handler called\n");
	halt_system();
}

/* INT vec 11 */
void segment_not_present_handler(struct cpu_state cpu, 
				 struct stack_state stack)
{
	kprint(ERROR, "Segment not present handler called\n");
	halt_system();
}

/* INT vec 12 */
void stack_segment_fault_handler(struct cpu_state cpu,
				 struct stack_state stack)
{
	kprint(ERROR, "Stack segment fault handler called\n");
	halt_system();
}

/* INT vec 13 */
void gpf_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "GPF handler called \n");
	halt_system();
}

/* INT vec 14 */
void page_fault_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(ERROR, "Page fault handler called\n");
	interrupt_register_dump(cpu, stack, 14);
	kprint(INFO, "Faulting addr: %x\n", get_cr2());
	PANIC("YEET\n");
}

void default_handler(struct cpu_state cpu, struct stack_state stack,
		     unsigned int interrupt)
{
	kprint(ERROR, "Default handler called for interrupt %d\n",
	       interrupt);
}

/*PIC HANDLERS*/

void irq0_handler(struct cpu_state cpu, struct stack_state stack)
{
	pit_interrupt_handler();
	pic_sendEOI(0);
}

void irq1_handler(struct cpu_state cpu, struct stack_state stack)
{
	interrupt_register_dump(cpu, stack, 33);
	pic_sendEOI(1);
}

 
void irq2_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 2 FIRED\n");
	pic_sendEOI(2);
}
 
void irq3_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 3 FIRED\n");
	pic_sendEOI(3);
}
 
void irq4_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 4 FIRED\n");
	pic_sendEOI(4);
}
 
void irq5_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 5 FIRED\n");
	pic_sendEOI(5);
}
 
void irq6_handler(struct cpu_state cpu, struct stack_state stack)
{
        kprint(INFO, "IRQ 6 FIRED\n");  
	pic_sendEOI(6);
}
 
void irq7_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 7 FIRED\n");
	pic_sendEOI(7);
}
 
void irq8_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 8 FIRED\n"); 
	pic_sendEOI(8);        
}
 
void irq9_handler(struct cpu_state cpu, struct stack_state stack) 
{
	kprint(INFO, "IRQ 9 FIRED\n");
	pic_sendEOI(9);
}
 
void irq10_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 10 FIRED\n");
	pic_sendEOI(10);
}
 
void irq11_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 11 FIRED\n");
	pic_sendEOI(11);
}
 
void irq12_handler(struct cpu_state cpu, struct stack_state stack)
{
        kprint(INFO, "IRQ 12 FIRED\n");  
	pic_sendEOI(12);
}
 
void irq13_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 13 FIRED\n");
        pic_sendEOI(13);
}
 
void irq14_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 14 FIRED\n");
	pic_sendEOI(14);
}
 
void irq15_handler(struct cpu_state cpu, struct stack_state stack)
{
	kprint(INFO, "IRQ 15 FIRED\n");
	pic_sendEOI(15);
}

void interrupt_handler(struct cpu_state cpu, unsigned int interrupt,
		       struct stack_state stack)
{
	switch (interrupt) {
		case 0: divide_by_zero_handler(cpu, stack);
			break;
		case 2: nmi_handler(cpu, stack);
			break;
		case 4: overflow_handler(cpu, stack);
			break;
		case 5: bound_range_exceeded_handler(cpu, stack);
			break;
		case 6: invalid_opcode_handler(cpu, stack);
			break;
		case 7: device_not_available_handler(cpu, stack);
			break;
		case 8: double_fault_handler(cpu, stack);
			break;
		case 10: invalid_tss_handler(cpu, stack);
			 break;
		case 11: segment_not_present_handler(cpu, stack);
			 break;
		case 12: stack_segment_fault_handler(cpu, stack);
			 break;
		case 13: gpf_handler(cpu, stack);
			 break;
		case 14: page_fault_handler(cpu, stack);
			 break;
		case 32: irq0_handler(cpu, stack);
			 break;
		case 33: irq1_handler(cpu, stack);
			 break;
		case 34: irq2_handler(cpu, stack);
			 break;
		case 35: irq3_handler(cpu, stack);
			 break;
		case 36: irq4_handler(cpu, stack);
			 break;
		case 37: irq5_handler(cpu, stack);
			 break;
		case 38: irq6_handler(cpu, stack);
			 break;
		case 39: irq7_handler(cpu, stack);
			 break;
		case 40: irq8_handler(cpu, stack);
			 break;
		case 41: irq9_handler(cpu, stack);
			 break;
		case 42: irq10_handler(cpu, stack);
			 break;
		case 43: irq11_handler(cpu, stack);
			 break;
		case 44: irq12_handler(cpu, stack);
			 break;
		case 45: irq13_handler(cpu, stack);
			 break;
		case 46: irq14_handler(cpu, stack);
			 break;
		case 47: irq15_handler(cpu, stack);
			 break;
		default: default_handler(cpu, stack, interrupt);
	}
}
