global __halt_system
global __enable_int
global __disable_int
global __nop
global get_int
global get_cr4
global get_cr3
global get_cr2
global get_cr0
global get_eax
global get_ebx
global get_ecx
global get_edx
global get_esi
global get_edi
global get_ebp
global get_esp
global get_eip
global get_eflags

__halt_system:
	hang:
	hlt
	jmp hang
	ret
__enable_int:
	sti
	ret
__disable_int:
	cli
	ret
__nop:
	nop
	nop
	nop
	ret
get_int:
	pushfd
	pop eax
	and eax, 0x0200
	shr eax, 9
	ret
get_cr4:
	mov eax, cr4
	ret
get_cr3:
	mov eax, cr3
	ret
get_cr2:
	mov eax, cr2
	ret
get_cr0:
	mov eax, cr0
	ret
get_eax:
	push eax
	pop eax
	ret
get_ebx:
	push ebx
	pop eax
	ret
get_ecx:
	push ecx
	pop eax
	ret
get_edx:
	push edx
	pop eax
	ret
get_esi:
	push esi
	pop eax
	ret
get_edi:
	push edi
	pop eax
	ret
get_ebp:
	push ebp
	pop eax
	ret
get_esp:
	mov eax, esp
	ret
get_eip:
	mov eax, [esp]
	ret
get_eflags:
	pushfd
	pop eax
	ret
