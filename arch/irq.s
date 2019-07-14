global irq0
extern irq0_handler
irq0:
	pusha
	call irq0_handler
	popa
	iret

global irq1
extern irq1_handler
irq1:
	pusha
	call irq1_handler
	popa
	iret

global irq2
extern irq2_handler
irq2:
	pusha
	call irq2_handler
	popa
	iret

global irq3
extern irq3_handler
irq3:
	pusha
	call irq3_handler
	popa
	iret

global irq4
extern irq4_handler
irq4:
	pusha
	call irq4_handler
	popa
	iret

global irq5
extern irq5_handler
irq5:
	pusha
	call irq5_handler
	popa
	iret

global irq6
extern irq6_handler
irq6:
	pusha
	call irq6_handler
	popa
	iret

global irq7
extern irq7_handler
irq7:
	pusha
	call irq7_handler
	popa
	iret

global irq8
extern irq8_handler
irq8:
	pusha
	call irq0_handler
	popa
	iret

global irq9
extern irq9_handler
irq9:
	pusha
	call irq9_handler
	popa
	iret

global irq10
extern irq10_handler
irq10:
	pusha
	call irq10_handler
	popa
	iret

global irq11
extern irq11_handler
irq11:
	pusha
	call irq11_handler
	popa
	iret

global irq12
extern irq12_handler
irq12:
	pusha
	call irq12_handler
	popa
	iret

global irq13
extern irq13_handler
irq13:
	pusha
	call irq13_handler
	popa
	iret

global irq14
extern irq14_handler
irq14:
	pusha
	call irq14_handler
	popa
	iret

global irq15
extern irq15_handler
irq15:
	pusha
	call irq15_handler
	popa
	iret

global load_idt
load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret
