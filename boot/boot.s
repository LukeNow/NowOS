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

;Declare gdt array
gdt_data:
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
gdt_end_data:



stack_bottom:
	resb 16384
stack_top:


section .text
global _start

_start:
	mov esp, stack_top
	
	mov eax, gdt_data
	push eax
	extern create_table
	call create_table
	
gdt:
	dw gdt_end_data - gdt_data - 1
	dd gdt_data

	cli	
	lgdt [gdt]

	jmp 0x08:reload_CS

reload_CS:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	sti
	extern kernel_main
	call kernel_main

	cli
hang:	hlt
	jmp hang
