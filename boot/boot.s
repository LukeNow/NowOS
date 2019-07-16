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

;Lets reserve our stack space
section .bss
align 16
stack_bottom:
	resb 16384
stack_top:

section .text
global _start
extern kernel_main
extern term_init
extern gdt_init
extern idt_init

_start:
	cli
	;Set up our initial stack
	mov esp, stack_top
	call term_init
	call gdt_init
	call idt_init
	sti
	;Call main kernel code
	call kernel_main
hang:	hlt
	jmp hang
