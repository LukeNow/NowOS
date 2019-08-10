KERNEL_VIRTUAL_BASE equ 0xC0000000 ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22) 
KERNEL_PAGE_ADDR equ ((0x01 << 12) | 0x00000083)

MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

;Store our magic headers so that GRUB runs our kernel code
section .multiboot.data
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

align 0x1000 ;Align on page boundary
boot_page_directory:
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

; Multiboot code entry point in lower memory
; We are doing this jump from multiboot to higher memory
; To satisfy a problem with grub directly linking our higher
; Half kernel.
section .multiboot.text progbits alloc exec nowrite align=16
global entry
entry:
    mov ecx, boot_page_directory
    mov cr3, ecx
    mov ecx, cr4
    ; Set PSE bit in to enable 4MB pages
    or ecx, 0x00000010     
    mov cr4, ecx
    mov ecx, cr0
    ; Set PG bit to enable paging.
    or ecx, 0x80000000     
    mov cr0, ecx
    ;Jump to higher half
    jmp _start 

; Higher mem stack
section .bss
align 4096

global stack_bottom
global stack_top

global heap_bottom
global heap_top

global early_heap_bottom
global early_heap_top

global page_heap_bottom
global page_heap_top
;Early heap for kernel page mappings
;And kheap lists
early_heap_bottom:
	resb 32768
early_heap_top:

;We will give heap pages
page_heap_bottom:
	resb 65536 ;BIGGER????
page_heap_top:

;Actual heap
heap_bottom:
	resb 102400
heap_top:

stack_bottom:
	resb 16384
stack_top:

; Higher mem code
section .text
align 16
global _start
global gdt_init_ret
extern kmain
extern term_init
extern gdt_init
extern idt_init
extern init_kern_paging
extern init_pit

_start:
	;Turn off interrupts just in case
	cli
	;Set up our initial stack
	mov esp, stack_top
	;Push multiboot info from bootloader
	push eax
	push ebx
	;Set up our terminal so we can print
	call term_init
	;Set up our GDT table
	;call gdt_init
	jmp gdt_init
gdt_init_ret:
	;Set up our IDT table
	call idt_init
	;Turn on interrupts
	
	call init_pit

	sti
	;Call main kernel code
	call kmain
	;We shouldnt be returning from kernel_main
hang:	hlt
	jmp hang
