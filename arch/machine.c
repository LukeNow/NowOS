#include <kdef.h>
#include <kprint.h>
#include <processor.h>
#include <irq_handle.h>
#include <scheduler.h>

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

void nop()
{
	__nop();
}

void _panic()
{
	disable_int();
	halt_system();
	for (;;);

}

void pause()
{
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
	kprint(INFO, "interrupts: %s\n", (get_int() == INT_SET) ? "ACTIVE" : "DISABLED");
}

void interrupt_register_dump(int_state_t * state)
{
	kprint(WARN, "*** Register dump for interrupt %d ****\n", state->int_num);
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x\n", state->eax, state->ebx,
	       state->ecx, state->edx);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x\n", state->esi, state->edi, 
	       state->ebp, state->esp);
	kprint(INFO, "ERROR CODE: %x EIP: %x CS %x EFLAGS %x\n", state->error_code,
	       state->eip, state->cs, state->eflags);
	kprint(INFO, "CR0: %x CR2: %x CR3: %x\n", get_cr0(), get_cr2(), get_cr3());
}
