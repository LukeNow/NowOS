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
extern create_table
extern term_init
extern idt_init
extern irq0
_start:
	
	cli
	;Set up our initial stack
	mov esp, stack_top
	
	call term_init
	;Create GDT table
	mov eax, gdt_data
	push eax
	call create_table
	
	
	;Load our table
	lgdt [gdt]

	; Reload registers to use our gdt
	jmp 0x08:reload_CS
reload_CS:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax 
	


	;call test1
	;hlt

	call idt_init
	
	;Call main kernel code
	call kernel_main

hang:	hlt
	jmp hang

;GDT description with data and size
gdt:
	dw gdt_end_data - gdt_data - 1
	dd gdt_data

;Declare gdt array
gdt_data:
	dq 0
	dq 0
	dq 0
	dq 0
	dq 0
gdt_end_data:


 
 idt:
    resd 50*2
 
 idtr:
    dw (50*8)-1
    dd idt
 
 test1:
    lidt [idtr]
    mov eax,irq0
    mov [idt+49*8],ax
    mov word [idt+49*8+2],8
    mov word [idt+49*8+4],0x8E00
    shr eax,16
    mov [idt+49*8+6],ax
    int 49
