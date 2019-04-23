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
_start:
	mov esp, stack_top
	
	
	extern kernel_main
	call kernel_main

	cli
hang:	hlt
	jmp hang
