#ifndef _PROCESSOR_H
#define _PROCESSOR_H

void __halt_system();
void __enable_int();
void __disable_int();

unsigned int get_cr2();
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
