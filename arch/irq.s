%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword 0
	push dword %1
	jmp common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword %1
	jmp common_interrupt_handler
%endmacro


extern interrupt_handler
common_interrupt_handler:
	
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	push ebp
	push esp ;This isnt the stack pointer of the interrupt we want just the context
	mov eax, cr3
	push eax
	mov eax, cr2
	push eax
	mov eax, cr0
	push eax
	
	; Push CPU struct ptr
	mov eax, esp
	push eax

	call interrupt_handler
	
	; pop The pointer
	pop eax

	pop eax
	pop eax
	pop eax ;Pop cr0, cr2, cr3
	pop esp
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax	
	add esp, 8
	iret

no_error_code_interrupt_handler 0 ;divide-by-zero
no_error_code_interrupt_handler 1 ;debug
no_error_code_interrupt_handler 2 ;NMI
no_error_code_interrupt_handler 3 ;breakpoint
no_error_code_interrupt_handler 4 ;overflow
no_error_code_interrupt_handler 5 ;bound range exceeded
no_error_code_interrupt_handler 6 ;invalid opcode
no_error_code_interrupt_handler 7 ;device not availabel
error_code_interrupt_handler 8 ;double fault
; 9 Obsolete for x86
error_code_interrupt_handler 10 ;invalid tss
error_code_interrupt_handler 11 ;segment not present
error_code_interrupt_handler 12 ;stack segment fault
error_code_interrupt_handler 13 ;GPF
error_code_interrupt_handler 14 ;page fault
; 15 reserved
no_error_code_interrupt_handler 16 ;x86 floating point exception 
no_error_code_interrupt_handler 17 ;alignment check
no_error_code_interrupt_handler 18 ;machine check
no_error_code_interrupt_handler 19 ;SIMD floating point exception
no_error_code_interrupt_handler 20 ;virtualization exception
; 21 - 29 RESERVED
error_code_interrupt_handler 30 ;security exception
; 31 reserved

no_error_code_interrupt_handler 32 ;PIT
no_error_code_interrupt_handler 33 ;Keyboard
no_error_code_interrupt_handler 34 ;Cascade (internal)
no_error_code_interrupt_handler 35 ;COM 2
no_error_code_interrupt_handler 36 ;COM 1
no_error_code_interrupt_handler 37 ;LPT2
no_error_code_interrupt_handler 38 ;Floppy disk
no_error_code_interrupt_handler 39 ;LPTI (spurious)
no_error_code_interrupt_handler 40 ;CMOS clock
no_error_code_interrupt_handler 41 ;Free
no_error_code_interrupt_handler 42 ;Free
no_error_code_interrupt_handler 43 ;Free
no_error_code_interrupt_handler 44 ;PS2 Mouse
no_error_code_interrupt_handler 45 ;FPU / Coprocessor
no_error_code_interrupt_handler 46 ;Primary ATA hard disk
no_error_code_interrupt_handler 47 ;Secodary ATA
