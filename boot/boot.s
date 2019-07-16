MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

;Store our magic headers so that GRUB runs out kernel code
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
align 16
stack_bottom:
	resb 16384
stack_top:

section .text
global _start
global gdt_init_ret
extern kernel_main
extern term_init
extern gdt_init
extern idt_init

_start:
	;Turn off interrupts
	cli
	;Set up our initial stack
	mov esp, stack_top
	;Set up our terminal so we can print
	call term_init
	;Set up our GDT table
	jmp gdt_init
gdt_init_ret:
	;Set up our IDT table
	call idt_init
	;Turn on interrupts
	sti
	;Call main kernel code
	call kernel_main
	;We shouldnt be returning from kernel_main
hang:	hlt
	jmp hang
