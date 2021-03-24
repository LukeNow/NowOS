#ifndef _IDT_H
#define _IDT_H

#include <kdef.h>

#define KERN_CODE_SEGMENT 0x08
#define TYPE_32_INT_GATE 0x8E

struct idt_entry{
	unsigned short int offset_lower;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type;
	unsigned short int offset_higher;
}__attribute__((packed));
typedef struct idt_entry idt_entry;

struct idt_reg_desc_t {
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed));
typedef struct idt_reg_desc_t idt_reg_desc_t;

idt_reg_desc_t * get_idt_desc();
#endif