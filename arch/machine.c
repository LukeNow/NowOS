#include "../include/kprint.h"
#include "../include/processor.h"

void register_dump()
{
	unsigned int eax, ebx, ecx, edx, esi, edi, 
		     ebp, esp, eip, eflags;
	

	kprint(INFO, "PRINTING!\n");
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
	
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x EIP: %x\n",
	       eax, ebx, ecx, edx, eip);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x EFLAGS: %x\n",
	       esi, edi, ebp, esp, eflags);
}
