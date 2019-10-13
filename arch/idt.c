#include <stdbool.h>
#include <stddef.h>
#include "../include/stdint.h"
#include "../include/io.h"
#include "../include/kprint.h"
#include "../include/irq_handle.h"

#define KERN_CODE_SEGMENT 0x08
#define TYPE_32_INT_GATE 0x8E

struct idt_entry{
	unsigned short int offset_lower;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type;
	unsigned short int offset_higher;
}__attribute__((packed));

extern void load_idt(unsigned long* idt_ptr);

static struct idt_entry idt[256];

static void set_idt_entry(unsigned int int_num, uint32_t address,
			  uint16_t selector, uint8_t type)
{
	struct idt_entry entry;

	entry.offset_lower = address & 0xFFFF;
	entry.selector = selector;
	entry.zero = 0;
	entry.type = type;
	entry.offset_higher = (address & 0xFFFF0000) >> 16;
	
	idt[int_num] = entry;
}

static void set_kern_interrupt(unsigned int int_num, uint32_t address)
{
	set_idt_entry(int_num, address, KERN_CODE_SEGMENT, 
		      TYPE_32_INT_GATE);
}

void set_idt(void) {
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	set_kern_interrupt(0, (unsigned long) interrupt_handler_0);
	set_kern_interrupt(1, (unsigned long) interrupt_handler_1);
	set_kern_interrupt(2, (unsigned long) interrupt_handler_2);
	set_kern_interrupt(3, (unsigned long) interrupt_handler_3);
	set_kern_interrupt(4, (unsigned long) interrupt_handler_4);
	set_kern_interrupt(5, (unsigned long) interrupt_handler_5);
	set_kern_interrupt(6, (unsigned long) interrupt_handler_6);
	set_kern_interrupt(7, (unsigned long) interrupt_handler_7);
	set_kern_interrupt(8, (unsigned long) interrupt_handler_8);
	set_kern_interrupt(10, (unsigned long) interrupt_handler_10);
	set_kern_interrupt(11, (unsigned long) interrupt_handler_11);
	set_kern_interrupt(12, (unsigned long) interrupt_handler_12);
	set_kern_interrupt(13, (unsigned long) interrupt_handler_13);
	set_kern_interrupt(14, (unsigned long) interrupt_handler_14);
	set_kern_interrupt(16, (unsigned long) interrupt_handler_16);
	set_kern_interrupt(17, (unsigned long) interrupt_handler_17);
	set_kern_interrupt(18, (unsigned long) interrupt_handler_18);
	set_kern_interrupt(19, (unsigned long) interrupt_handler_19);
	set_kern_interrupt(20, (unsigned long) interrupt_handler_20);
	set_kern_interrupt(30, (unsigned long) interrupt_handler_30);
	/* PIC INTERRUPTS */
	set_kern_interrupt(32, (unsigned long) interrupt_handler_32);
	set_kern_interrupt(33, (unsigned long) interrupt_handler_33);
	set_kern_interrupt(34, (unsigned long) interrupt_handler_34);
	set_kern_interrupt(35, (unsigned long) interrupt_handler_35);
	set_kern_interrupt(36, (unsigned long) interrupt_handler_36);
	set_kern_interrupt(37, (unsigned long) interrupt_handler_37);
	set_kern_interrupt(38, (unsigned long) interrupt_handler_38);
	set_kern_interrupt(39, (unsigned long) interrupt_handler_39);
	set_kern_interrupt(40, (unsigned long) interrupt_handler_40);
	set_kern_interrupt(41, (unsigned long) interrupt_handler_41);
	set_kern_interrupt(42, (unsigned long) interrupt_handler_42);
	set_kern_interrupt(43, (unsigned long) interrupt_handler_43);
	set_kern_interrupt(44, (unsigned long) interrupt_handler_44);
	set_kern_interrupt(45, (unsigned long) interrupt_handler_45);
	set_kern_interrupt(46, (unsigned long) interrupt_handler_46);
	set_kern_interrupt(47, (unsigned long) interrupt_handler_47);
	
	/* Fill all NULL idt entries with an entry, so that we can debug it if 
	 * it was to fire (for some unknown reasons that we should have handled */
	for (int i = 0; i < 256; i++) {
		struct idt_entry temp = idt[i];
		if ( ((uint64_t)temp.offset_lower) == 0) {
			set_kern_interrupt(i, (unsigned long) interrupt_handler_1);
		}
	}

	/* fill the IDT descriptor */
	idt_address = (unsigned long) idt;
	idt_ptr[0] = (sizeof (struct idt_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;
	
	/* Call asm funcion to load table */
	load_idt(idt_ptr);
	kprint(INFO, "IDT table loaded\n");
}

	

