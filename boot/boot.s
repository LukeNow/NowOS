KERNEL_VIRTUAL_BASE equ 0xC0000000 ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22) 
KERNEL_PAGE_ADDR equ ((0x01 << 12) | 0x00000083)

MBALIGN equ 1 << 0
MEMINFO equ 1 << 1
FLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

%macro qemu_print 1
	mov ax, %1
	mov dx, 0x3F8 
	out dx, al
%endmacro

;Common label declarations
global stack_bottom
global stack_top
global heap_bottom
global heap_top
global early_heap_bottom
global early_heap_top
global page_heap_bottom
global page_heap_top

;Function delcarations
global _start
extern kmain
extern term_init
extern gdt_init
extern idt_init
extern init_kern_paging
extern init_pit

;Store our magic headers so that GRUB runs our kernel code
section .multiboot.data
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

;Our initial page directory so that we can work in our higher
;half kernel immediately
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

;Early heap for kernel page mappings
;And kheap lists
early_heap_bottom:
	resb 32768
early_heap_top:

;We will give heap pages
page_heap_bottom:
	resb 65536 ;TODO should we give more?
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

extern pic_disable
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
	call gdt_init
	;jmp gdt_init
	;Set up our IDT table
	call idt_init
	;Start clock
	
	call init_pit
	call pic_disable
	sti
	;Call main kernel code
	call kmain
	;We shouldnt be returning from kernel_main
hang:	hlt
	jmp hang



AP_START_BASE equ 0x8000

; Initializaion start for APs, in real mode
; We are going to copy all of the data/code starting from
; here all the way to the end so that the APs in real mode
; can access the data. We can unmap these structures later.
;
; REMEMBER this code is getting copied at run time by the BSP
; and then waking up the APs to start executing this code.

global ap_start
extern init_ap
ap_start:
bits 16
	
	cli
	
	mov ax, cs 
	mov ds, ax
	xor ax, ax
	mov sp, ax

	;lgdt [ap_initial_gdtr - ap_start + AP_START_BASE]
	lgdt [ap_initial_gdtr - ap_start]
	mov eax, cr0
	or eax, 0x01
	mov cr0, eax

	jmp 0x08:(ap_start_32 - ap_start + AP_START_BASE)

ap_start_32:
bits 32
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax 

	xor eax, eax
	inc eax
	
	; Add 1 to cpu ID and save the id so we can keep it
	; We use this ID to get stacks that we assign to each processor
	lock xadd [ap_cpu_id - ap_start + AP_START_BASE], eax
	
	mov esi, eax
	mov ecx, ap_init_stacks - ap_start + AP_START_BASE 
	
	mov ebx, 4
	mov eax, esi
	mul ebx
	; eax = 4 * cpu_id
	add eax, ecx
	; eax = 4 * cpu_id + stack_address
	mov esp, [eax]
	
	; Load the BSPs cr3
	mov eax, [ap_cpu_cr3 - ap_start + AP_START_BASE]
	mov cr3, eax

	; enable Paging
	mov ebx, cr0
	or ebx, 0x80000000
	mov cr0, ebx

	; Load the BSPS gdtr
	mov eax, ap_cpu_gdtr - ap_start + AP_START_BASE
	lgdt [eax]

	; Load the BSPs idtr
	mov eax, ap_cpu_idtr - ap_start + AP_START_BASE
	lidt [eax]

	; Load the BSPs cr0
	mov eax, [ap_cpu_cr0 - ap_start + AP_START_BASE]
	mov cr0, eax

	; Load the BSPs cr4
	mov eax, [ap_cpu_cr4 - ap_start + AP_START_BASE]
	mov cr4, eax
	
	jmp 0x08:.reload_cs
.reload_cs:

	; Incremend the cpu id, 0 being the BSP now
	inc esi
	push esi

	call init_ap

ap_initial_gdtr:
	dw ap_init_gdt_end - ap_init_gdt_start - 1
	dd (ap_init_gdt_start - ap_start) + AP_START_BASE
ap_init_gdt_start:
	dq 0x0
	; Intial code segments
	dd 0x0000FFFF
    dd 0x00cf9a00
    ; initial data segments
    dd 0x0000FFFF
    dd 0x00cf9200
ap_init_gdt_end:

global ap_cpu_cr0
ap_cpu_cr0:
	dd 0
global ap_cpu_cr3
ap_cpu_cr3:
	dd 0
global ap_cpu_cr4
ap_cpu_cr4:
	dd 0
global ap_cpu_idtr
ap_cpu_idtr:
	dw 0
	dd 0
global ap_cpu_gdtr
ap_cpu_gdtr:
	dw 0
	dd 0
global ap_cpu_id
ap_cpu_id:
	dd 0
global ap_init_stacks
; This label is the end of the ap boot section where we will
; assign temporary stacks so that we can self initialize in C land
ap_init_stacks: