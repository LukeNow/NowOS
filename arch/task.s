extern scheduler_lock_counter
extern scheduler_postponed_counter
extern scheduler_postponed_flag
extern current_task
extern next_task
extern destroy_task
extern bootstrap_task

; void prep_stack_frame(task_control_block *task, void (*main)(), 
;		        uint32_t stack_addr) 
global prep_stack_frame
prep_stack_frame:

	mov eax, esp ;save current esp
	mov ebx, [esp + 4] ;task
	mov ecx, [esp + 8] ;main
	mov edx, [esp + 12] ;stack addr

	mov esi, dummy_bootstrap_task ;return addr
	
	mov [edx - 0], dword 0xDEADBEEF ;eip, zero because we dont return here
	mov [edx - 4], dword 0xDEADBEEF
	mov [edx - 8], ebx
	mov [edx - 12], ecx
	mov [edx - 16], esi
	

	ret

;TODO 
; We need to save segments and TSS for switching between userspace
; and kernel space

; Dummy label that a tasks jumps to when it is given its prepped stack
dummy_bootstrap_task:
	call bootstrap_task

; We need to disable IRQs before and renable after this function
global switch_task
switch_task:
	
	; Check that a scheduler lock isnt being held
	; If it is we dont switch tasks, else continue
	cmp dword [scheduler_postponed_counter], 0
	je .continue
	mov dword [scheduler_postponed_flag], 1
	ret
.continue:
	mov eax, [current_task]
	mov ecx, esp
	mov [eax + 20], ecx
	mov ecx, ebp
	mov [eax + 24], ecx
	;;Get next task and start loading it
	mov ecx, [next_task] ;get next task
	mov [current_task], ecx ;move next task into current task
	mov esp, [ecx + 20] ;load next esp
	mov ebp, [ecx + 24]
	mov eax, [ecx + 8]  ;load cr3
	mov ecx, cr3 ;previous tasks virtual address space (page dir)
	
	cmp eax, ecx
	je .sameVAS ;same virtual address space
	
	mov cr3, eax
.sameVAS:

	xchg bx, bx
	ret ;Because we changed the SP, we will pop the return addr
	    ;of the function we are switching to
	    ;This means that this function is not an asyncrounous
	    ;task switch, something we will need to implement later
