global gdt_init
extern create_table

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

gdt_init:
	mov eax, gdt_data
	push eax
	call create_table
	
	;load the table
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

	ret
