#ifndef _MACHINE_H
#define _MACHINE_H

#include <kdef.h>

#define INT_SET 1
#define INT_CLEARED 0

struct int_state {
	register_t cr0;
	register_t cr2;
	register_t cr3;
    register_t esp;
	register_t ebp;
	register_t edi;
	register_t esi;
	register_t edx;
	register_t ecx;
	register_t ebx;
	register_t eax;
	register_t int_num;
	register_t error_code;
	register_t eip;
	register_t cs;
	register_t eflags;
} __attribute__((packed));
typedef struct int_state int_state_t;

struct cpu_state {
	register_t cr0;
	register_t cr2;
	register_t cr3;
	register_t eflags;
	register_t eip;
    register_t esp;
	register_t ebp;
	register_t edi;
	register_t esi;
	register_t edx;
	register_t ecx;
	register_t ebx;
	register_t eax;
} __attribute__((packed));
typedef struct cpu_state cpu_state_t;

register_t get_int();
register_t get_cr4();
register_t get_cr3();
register_t get_cr2();
register_t get_cr0();
register_t get_eax();
register_t get_ebx();
register_t get_ecx();
register_t get_edx();
register_t get_esi();
register_t get_edi();
register_t get_ebp();
register_t get_esp();
register_t get_eip();
register_t get_eflags();

void __halt_system();
void __enable_int();
void __disable_int();
void __nop();
void _panic();

void halt_system();
void enable_int();
void disable_int();
void nop();
void register_dump();
void pause();
void interrupt_register_dump(int_state_t * state);

#endif
