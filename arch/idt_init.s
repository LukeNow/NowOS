extern set_idt
extern pic_remap
global idt_init
idt_init:
	call pic_remap
	call set_idt
	ret

; Gets called in set_idt
global load_idt
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	ret
