#ifndef _PROCESSOR_H
#define _PROCESSOR_H

void halt_system();
void enable_int();
void disable_int();

void __int_load_regs();
void __int_load_regs_error();
unsigned int int_get_eip();
unsigned int int_get_error();
unsigned int int_get_eflags();
unsigned int int_get_cs();

unsigned int get_eax();
unsigned int get_ebx();
unsigned int get_ecx();
unsigned int get_edx();
unsigned int get_esi();
unsigned int get_edi();
unsigned int get_ebp();
unsigned int get_esp();
unsigned int get_eip();
unsigned int get_eflags();

#endif
