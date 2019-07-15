#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
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

struct idt_entry idt[256];

extern void load_idt();

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

void idt_init(void) {
	
	unsigned long idt_address;
	unsigned long idt_ptr[2];
			;
	/* remapping the PIC */
	outb(0x20, 0x11);
        outb(0xA0, 0x11);
        outb(0x21, 0x20);
        outb(0xA1, 40);
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0x0);
        outb(0xA1, 0x0);
	
	set_kern_interrupt(0, (unsigned long) interrupt_handler_0);
	set_kern_interrupt(2, (unsigned long) interrupt_handler_1);
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

	/* fill the IDT descriptor */
	idt_address = (unsigned long) idt;
	idt_ptr[0] = (sizeof (struct idt_entry) * 256) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
	kprint(INFO, "IDT table loaded\n");
}

	

