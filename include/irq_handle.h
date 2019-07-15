#ifndef _IRQ_HANDLE_H
#define _IRQ_HANDLE_H

struct cpu_state {
        unsigned int eax;
        unsigned int ebx;
        unsigned int ecx;
        unsigned int edx;
	unsigned int esi;
	unsigned int edi;
	unsigned int ebp;
        unsigned int esp;
} __attribute__((packed));

struct stack_state {
	unsigned int error_code;
	unsigned int eip;
	unsigned int cs;
	unsigned int eflags;
} __attribute__((packed));

void interrupt_handler(struct cpu_state cpu, struct stack_state stack,
		       unsigned int interrupt);

void interrupt_handler_0();
void interrupt_handler_1();
void interrupt_handler_2();
void interrupt_handler_4();
void interrupt_handler_5();
void interrupt_handler_6();
void interrupt_handler_7();
void interrupt_handler_8();
void interrupt_handler_10();
void interrupt_handler_11();
void interrupt_handler_12();
void interrupt_handler_13();
void interrupt_handler_14();
void interrupt_handler_32();
void interrupt_handler_33();
void interrupt_handler_34();
void interrupt_handler_35();
void interrupt_handler_36();
void interrupt_handler_37();
void interrupt_handler_38();
void interrupt_handler_39();
void interrupt_handler_40();
void interrupt_handler_41();
void interrupt_handler_42();
void interrupt_handler_43();
void interrupt_handler_44();
void interrupt_handler_45();
void interrupt_handler_46();
void interrupt_handler_47();

#endif
