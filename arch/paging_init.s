global paging_init
paging_init:
	mov eax, [esp + 4]
	mov cr3, eax
	
	mov eax, cr4
	and eax, 0xFFFFFFEF
	mov cr4, eax

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret
