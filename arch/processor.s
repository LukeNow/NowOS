global halt_system
global enable_int
global disable_int
global __int_load_regs
global __int_load_regs_error
global int_get_eip
global int_get_error
global int_get_eflags
global int_get_cs
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

;bp + 52 [eflags]
;bp + 48 [cs]
;bp + 44 [eip]
;bp + 40 [the registers]
; (ERROR)
;bp + 4 [load_registers]
;bp

halt_system:
	hang: hlt
	jmp hang
	ret

enable_int:
	sti
	ret

disable_int:
	nmi
	ret

; CRINGE CODE TERRITORY!!!!!
__int_load_regs:
	mov ebx, [bp + 44] ;EIP
	mov ecx, [bp + 48] ;CS
	mov edx, [bp + 52] ;EFLAGS
	ret

__int_load_regs_error:
	mov esi, [bp + 44] ;ERROR
	mov ebx, [bp + 48] ;EIP
	mov ecx, [bp + 52] ;CS
	mov edx, [bp + 56] ;EFLAGS
	ret

int_get_eip:
	mov eax, [ebx]
	ret

int_get_error:
	mov eax, [esi]
	ret

int_get_eflags:
	mov eax, [edx]
	ret

int_get_cs:
	mov eax, [ecx]
	ret

;;;;CRINGEEEE ended

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
	mov eax, [ebp + 4]
	ret
get_eflags:
	pushfd
	pop eax
	ret
