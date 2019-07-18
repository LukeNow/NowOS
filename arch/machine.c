#include "../include/kprint.h"
#include "../include/processor.h"
#include "../include/irq_handle.h"

void halt_system()
{
	__halt_system(); //hlt
}

void enable_int()
{
	__enable_int(); //sti
}

void disable_int()
{
	__disable_int(); //cli
}

void _panic()
{
	disable_int();
	halt_system();
	for (;;);

}
void register_dump()
{
	unsigned int eax, ebx, ecx, edx, esi, edi, 
		     ebp, esp, eip, eflags;
	
	eax = ebx = ecx = edx = esi = edi =
		ebp = esp = eip = eflags = 0;

	eax = get_eax();
	ebx = get_ebx();
	ecx = get_ecx();
	edx = get_edx();
	esi = get_esi();
	edi = get_edi();
	ebp = get_ebp();
	esp = get_esp();
	eip = get_eip();
	eflags = get_eflags();
	
	kprint(WARN, "** REGISTER DUMP ***\n");
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x EIP: %x\n",
	       eax, ebx, ecx, edx, eip);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x EFLAGS: %x\n",
	       esi, edi, ebp, esp, eflags);
}

void interrupt_register_dump(struct cpu_state cpu, struct stack_state stack,
			     unsigned int interrupt)
{
	kprint(WARN, "*** Register dump for interrupt %d ****\n", interrupt);
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x\n", cpu.eax, cpu.ebx,
	       cpu.ecx, cpu.edx);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x\n", cpu.esi, cpu.edi, 
	       cpu.ebp, cpu.esp);
	kprint(INFO, "ERROR CODE: %x EIP: %x CS %x EFLAGS %x\n", stack.error_code,
	       stack.eip, stack.cs, stack.eflags);
}
