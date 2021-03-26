;extern current_task
;extern next_task
extern destroy_task
extern bootstrap_task

; These are the offsets into the cpu struct that is gaurenteed to be at the
; top of the tib_t struct. So any offsets from the tib_t pointer will be
; these values
%define cpu_cr0 0
%define cpu_cr2 4
%define cpu_cr3 8
%define cpu_eflags 12
%define cpu_eip 16
%define cpu_esp 20
%define cpu_ebp 24
%define cpu_edi 28
%define cpu_esi 32
%define cpu_edx 36
%define cpu_ecx 40
%define cpu_ebx 44
%define cpu_eax 48

; Offsets into the int_state_t struct
%define int_cr0 0
%define int_cr2 4
%define int_cr3 8
%define int_esp 12
%define int_ebp 16
%define int_edi 20
%define int_esi 24
%define int_edx 28
%define int_ecx 32
%define int_ebx 36
%define int_eax 40
%define int_int_num 44
%define int_error_code 48
%define int_eip 52
%define int_cs 56
%define int_eflags 60

; These are the offsets to the thread we are switching to's sp
; We are going to modify the stack so that an iret will switch
; to the eip of the thread, just as if it was always returning from an
; interrupt
; They are negative numbers because we are putting these infront of the 
; stack of to_thread's sp
%define thread_prep_eflags -4
%define thread_prep_cs -8
%define thread_prep_eip -12
%define thread_prep_eax -16
%define thread_prep_ebx -20
%define thread_prep_ecx -24
%define thread_prep_edx -28


; void prep_stack_frame(tib_t * tib, void (*enter)(), 
;		        uint32_t stack_addr) 
global prep_stack_frame
prep_stack_frame:

	mov eax, [esp + 4] ;tib
	mov ebx, [esp + 8] ;enter address
	mov ecx, [esp + 12] ;stack addr
	
	mov [ecx - 0], dword 0xDEADBEEF ;eip, zero because we dont return here
	

	ret


extern thread_enter
;void _thread_enter(tib_t * thread)
global _thread_enter
_thread_enter:
	mov eax, [esp + 4]
	mov esp, [eax + cpu_esp]
	push eax	;Push the ptr so it can be used by thread_enter
	call thread_enter

extern scheduler_leave_and_queue
;void thread_switch_sync(tib_t * from_thread, tib_t * to_thread)
global thread_switch_sync
thread_switch_sync:
	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread

	mov [eax + cpu_esp], esp
	mov [eax + cpu_ebp], ebp
	mov ecx, esp
	mov edx, [esp]
	mov [eax + cpu_eip], edx ;Return address that needs to be saved

	; We leave scheduler here because we are done with from_thread
	; To_thread is safe because it is already marked as running
	push eax
	call scheduler_leave_and_queue
	add esp, 4

	; Change stacks
	mov esp, [ebx + cpu_esp]
	; Push the new eip of to_thread

	mov eax, [ebx + cpu_eip]
	push eax
	
	mov eax, [ebx + cpu_eax]
	push eax
	mov eax, [ebx + cpu_ebx]
	push eax
	mov eax, [ebx + cpu_ecx]
	push eax
	mov eax, [ebx + cpu_edx]
	push eax

	mov esi, [ebx + cpu_esi]
	mov edi, [ebx + cpu_edi]
	mov ebp, [ebx + cpu_ebp]
	;mov eflags, [ebx + cpu_eflags]
	mov eax, [ebx + cpu_cr0]
	mov cr0, eax
	
	mov eax, [ebx + cpu_cr2]
	mov cr2, eax

	mov eax, cr3
	mov ebx, [ebx + cpu_cr3]
	cmp eax, ebx
	; If the cr3s are the same, dont reload! Save the tlb being flushed
	je .samecr3
	mov cr3, ebx
.samecr3:

	pop edx
	pop ecx
	pop ebx
	pop eax
	; Pops the eip of to_thread that we placed
	ret


; In this function we are manipulating values past the SP of to_thread, 
; We can only do this because we use interrupt gates, where only one interrupt
; Is executing at a time (no nested interrupts), so if we put values past the 
; SP they wont be changing
; Also note, that we are switching from an interrupt context, and we are essentially
; moving the interrupts "frame" to the thread we are switching to

;void thread_switch_async(tib_t * from_thread, tib_t * to_thread, 
;						  int_state_t * int_state)
global thread_switch_async
thread_switch_async:
	
	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread
	mov ecx, [esp + 12] ; int_state
	mov edx, [ebx + cpu_esp] ;to_thread's sp

	mov esi, [ecx + int_eflags]
	mov [edx + thread_prep_eflags], esi

	mov esi, [ecx + int_cs]
	mov [edx + thread_prep_cs], esi

	; This is to_threads eip that we will be switching to
	; when we execute an iret
	mov esi, [ebx + cpu_eip]
	mov [edx + thread_prep_eip], esi

	mov esi, [ebx + cpu_eax]
	mov [edx + thread_prep_eax], esi

	mov esi, [ebx + cpu_ebx]
	mov [edx + thread_prep_ebx], esi

	mov esi, [ebx + cpu_ecx]
	mov [edx + thread_prep_ecx], esi

	mov esi, [ebx + cpu_edx]
	mov [edx + thread_prep_edx], esi

	; The last three values will be popped by iret
	;mov [edx + thread_prep_eax], [ebx + cpu_eax]
	;mov [edx + thread_prep_ebx], [ebx + cpu_ebx]
	;mov [edx + thread_prep_ecx], [ebx + cpu_ecx]
	;mov [edx + thread_prep_edx], [ebx + cpu_edx]

	mov esi, [ebx + cpu_esi]
	mov edi, [ebx + cpu_edi]
	mov ebp, [ebx + cpu_ebp]
	;mov eflags, [ebx + cpu_eflags]

	mov esi, [ebx + cpu_cr0]
	mov cr0, esi
	;mov cr0, [ebx + cpu_cr0]
	mov esi, [ebx + cpu_cr2]
	mov cr2, esi
	;mov cr2, [ebx + cpu_cr2]

	mov eax, cr3
	mov ebx, [ebx + cpu_cr3]
	cmp eax, ebx
	; If the cr3s are the same, dont reload! Save the tlb being flushed
	je .samecr3
	mov cr3, ebx
.samecr3:
	; Move the stack pointer of from_thread to where we placed
	; the values
	add edx, thread_prep_edx
	mov esp, edx

	pop edx
	pop ecx
	pop ebx
	pop eax

	iret

	;mov eax, [current_task]
	;mov ecx, esp
	;mov [eax + 20], ecx
	;mov ecx, ebp
	;mov [eax + 24], ecx
	;;Get next task and start loading it
	;mov ecx, [next_task] ;get next task
	;mov [current_task], ecx ;move next task into current task
	;mov esp, [ecx + 20] ;load next esp
	;mov ebp, [ecx + 24]
	;mov eax, [ecx + 8]  ;load cr3
	;mov ecx, cr3 ;previous tasks virtual address space (page dir)
	
	;cmp eax, ecx
	;je .sameVAS ;same virtual address space
	
	;mov cr3, eax
;.sameVAS:

	;ret ;Because we changed the SP, we will pop the return addr
	    ;of the function we are switching to
	    ;This means that this function is not an asyncrounous
	    ;task switch, something we will need to implement later
