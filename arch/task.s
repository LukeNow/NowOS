extern current_task
extern next_task
extern destroy_task
extern start_task
; void prep_stack_frame(task_control_block *task, void (*main)(), 
;		        uint32_t stack_addr) 
global prep_stack_frame
prep_stack_frame:

	;pusha
	;pushfd
	mov eax, esp ;save current esp
	mov ebx, [esp + 4] ;task
	mov ecx, [esp + 8] ;main
	mov edx, [esp + 12] ;stack addr
	
	;mov esp, edx ;move to new stack to set it up
	
	mov [edx + 0], dword 0xDEADBEEF ;eip, zero because we dont return here
	
	xchg bx, bx
	push dword 0xDEADBEEF ;ebp
	push ebx ;TCB
	push ecx ;main
	push start_task; ;new eip
	push edx ;new ebp
	
	mov esp, eax ;move back to previous stack
	
.hlt
	hlt
	jmp .hlt

	
	;popfd
	;popa
	ret

;TODO 
; We need to save segments and TSS for switching between userspace
; and kernel space

load_regs_to_task:
	;; Save current register state
;	push eax
	push ebx
;	push ecx
;	push edx
	push esi
	push edi
	push ebp
	push esp
	mov eax, [ebp + 4] ;saved eip
	pushfd
	mov eax, cr3
	push eax
	mov eax, cr2
	push eax
	mov eax, cr0
	push eax
	;;Get current TCB
	mov edi, [current_task]
	;;Start loading registers into TCB
	pop eax
	mov [esi + 0], eax ;cr0
	pop eax
	mov [esi + 4], eax ;cr2
	pop eax
	mov [esi + 8], eax ;cr3
	pop eax 
	mov [esi + 12], eax ;eflags
	pop eax
	mov [esi + 16], eax ;eip
	pop eax
	mov [esi + 20], eax ;esp
	pop eax
	mov [esi + 24], eax ;ebp
	pop eax
	mov [esi + 28], eax ;edi
	pop eax 
	mov [esi + 32], eax ;esi
	pop eax 
;	mov [esi + 36], eax ;edx
;	pop eax
;	mov [esi + 40], eax ;ecx
	pop eax 
	mov [esi + 44], eax ;ebx;
;	pop eax
;	mov [esi + 48], eax ;eax
	jmp load_regs_to_task

		


; void switch_task(task_control_block *next_thread)
; We need to disable IRQs before and renable after this function
global switch_task
switch_task:
	;jmp load_regs_to_task
	;load_regs_to_task_ret:

	;;Get next task and start loading it
	mov esi, [next_task] ;get next task
	mov [current_task], esi ;move next task into current task
	
	mov esp, [esi + 20] ;load next esp
	mov eax, [esi + 8]  ;load cr3
	mov ecx, cr3 ;previous tasks virtual address space (page dir)

.hlt
	hlt
	jmp .hlt
	
	;xchg bx, bx
	cmp eax, ecx
	je .sameVAS ;same virtual address space
	mov cr3, eax
.sameVAS
	
	ret
