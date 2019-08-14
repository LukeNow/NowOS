extern scheduler_lock_counter
extern scheduler_postponed_counter
extern scheduler_postponed_flag
extern current_task
extern next_task
extern destroy_task
extern start_task
; void prep_stack_frame(task_control_block *task, void (*main)(), 
;		        uint32_t stack_addr) 
global prep_stack_frame
prep_stack_frame:

	mov eax, esp ;save current esp
	mov ebx, [esp + 4] ;task
	mov ecx, [esp + 8] ;main
	mov edx, [esp + 12] ;stack addr
	mov esi, bootstrap_task ;return addr
	
	mov [edx - 0], dword 0xDEADBEEF ;eip, zero because we dont return here
	mov [edx - 4], dword 0xDEADBEEF
	mov [edx - 8], ebx
	mov [edx - 12], ecx
	mov [edx - 16], esi

	ret

;TODO 
; We need to save segments and TSS for switching between userspace
; and kernel space

bootstrap_task:
	call start_task
	xchg bx, bx

load_regs_to_task:
	;; Save current register state
	mov eax, [current_task]

	;;Start loading registers into TCB
	;mov ecx, cr0
	;mov [eax + 0], ecx ;cr0
	;mov ecx, cr2
	;mov [eax + 4], ecx ;cr2
	;mov ecx, cr3
	;mov [eax + 8], ecx ;cr3
	;pushfd
	;pop ecx
	;mov [eax + 12], ecx ;eflags
	;mov ecx, [esp]
	;mov [eax + 16], ecx ;eip
	mov ecx, esp
	mov [eax + 20], ecx ;esp

	;mov ecx, ebp
	;mov [eax + 24], ecx ;ebp
	;mov ecx, edi
	;mov [eax + 28], ecx ;edi
	;mov ecx, esi
	;mov [eax + 32], ecx ;esi
	;mov ecx, edx
	;mov [eax + 36], ecx ;edx
	;mov ecx, ecx
	;mov [eax + 40], ecx ;ecx
	;mov ecx, ebx
	;mov [eax + 44], ecx ;ebx;
	;mov ecx, eax
	;mov [eax + 48], ecx ;eax
	jmp load_regs_to_task_ret

; We need to disable IRQs before and renable after this function
global switch_task
switch_task:
	;jmp load_regs_to_task
load_regs_to_task_ret:
	
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

	ret
