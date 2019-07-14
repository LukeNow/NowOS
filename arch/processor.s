global enable_int
global disable_int
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

enable_int:
	sti
	ret

disable_int:
	nmi
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
	push esp
	pop eax
	ret
get_eip:
	mov eax, [esp]
	ret
get_eflags:
	pushfd
	pop eax
	ret
