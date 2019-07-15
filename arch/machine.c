#include "../include/kprint.h"
#include "../include/processor.h"

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
	
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x EIP: %x\n",
	       eax, ebx, ecx, edx, eip);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x EFLAGS: %x\n",
	       esi, edi, ebp, esp, eflags);
}

void interrupt_register_dump(int int_num)
{
	unsigned int eax, ebx, ecx, edx, esi, edi, 
		     ebp, esp, eip, eflags = 0;

	unsigned int int_eflags, int_cs, int_eip, 
		     int_error = 0;
	
	int_eflags = int_get_eflags();
	int_cs = int_get_cs();
	int_eip = int_get_eip();
	int_error = int_get_error();

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
	
	kprint(WARN, "INT NUMBER %d register dump\n", int_num);
	kprint(INFO, "INT_EFLAGS: %x INT_CS: %x INT_EIP %x INT_ERROR %x\n",
	       int_eflags, int_cs, int_eip, int_error);
	kprint(INFO, "EAX: %x EBX: %x ECX: %x EDX: %x EIP: %x\n",
	       eax, ebx, ecx, edx, eip);
	kprint(INFO, "ESI: %x EDI: %x EBP: %x ESP: %x EFLAGS: %x\n",
	       esi, edi, ebp, esp, eflags);

}
