global paging_init
paging_init:
	mov eax, [esp + 4]
	mov cr3, eax
	
	mov ebx, cr4
	and ebx, 0xFFFFFFEF
	mov cr4, ebx

	mov ebx, cr0
	or ebx, 0x80000000
	mov cr0, ebx
	ret

global flush_tlb
flush_tlb:
	mov eax, cr3
	mov cr3, eax

