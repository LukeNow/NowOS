global outb
global inb
global debug_print
global div_zero

outb:
	mov al, [esp + 8]
	mov dx, [esp + 4]
	out dx, al
	ret

inb:	
	xor eax, eax
	mov dx, [esp + 8]
	in al, dx
	ret

debug_print:
	mov al, [esp + 4]
	mov dx, 0x3F8 
	out dx, al

div_zero
	mov eax, 100
	mov ecx, 0
	div ecx
	ret
	
