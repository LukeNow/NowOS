global gdt_init
global flush_gdt
extern create_gdt_table
;extern gdt_init_ret

;GDT description with data and size
;gdt:
;	dw gdt_end_data - gdt_data - 1
;	dd gdt_data

;Declare gdt array
;gdt_data:
;	dq 0
;	dq 0
;	dq 0
;	dq 0
;	dq 0
;gdt_end_data:

;; flush_gdt(void * gdt_table_desc)
flush_gdt:
	mov eax, [esp + 4]
	lgdt [eax]
	ret

gdt_init:
	call create_gdt_table

	;load the table
	lgdt [eax]
	jmp 0x08:reload_CS
reload_CS:
; Reload registers to use our gdt
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax 
	ret
