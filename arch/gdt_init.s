global gdt_init
extern create_table
extern gdt_init_ret
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
	add esp, 4

	;load the table
	lgdt [gdt]

; Reload registers to use our gdt
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax 
	jmp 0x08:reload_CS
reload_CS:
	;ret
	jmp gdt_init_ret
