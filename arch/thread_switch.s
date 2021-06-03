
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
	cli

	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread

	mov ecx, esp
	add ecx, 4 ; subtrac the eip that we wont be popping

	mov [eax + cpu_esp], ecx
	mov [eax + cpu_ebp], ebp
	mov ecx, esp
	mov edx, [esp]
	mov [eax + cpu_eip], edx ;Return address that needs to be saved

	; We leave scheduler here because we are done with from_thread
	; To_thread is safe because it is already marked as running
	push eax
	call scheduler_leave_and_queue
	add esp, 4
	
	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread

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

	sti
	; Pops the eip of to_thread that we placed
	ret


; In this function we are manipulating values past the SP of to_thread, 
; we can only do this because we use interrupt gates, where only one interrupt
; ss executing at a time (no nested interrupts), so if we put values past the 
; SP they wont be changing
; Also note, that we are switching from an interrupt context, and we are essentially
; moving the interrupts "frame" to the thread we are switching to

;void thread_switch_async(tib_t * from_thread, tib_t * to_thread, 
;						  int_state_t * int_state)
extern scheduler_async_leave
global thread_switch_async
thread_switch_async:
	
	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread
	mov ecx, [esp + 12] ; int_state
	mov edx, [ebx + cpu_esp] ;to_thread's sp

	mov esi, [ecx + int_eflags] ; We might need to load the eflags not from
								; the int structure because of user space stuff
	mov [edx + thread_prep_eflags], esi

	mov esi, [ecx + int_cs]		; ^ same here
	mov [edx + thread_prep_cs], esi


	call scheduler_async_leave
	
	mov eax, [esp + 4] ;from_thread
	mov ebx, [esp + 8] ;to_thread
	mov ecx, [esp + 12] ; int_state
	mov edx, [ebx + cpu_esp] ;to_thread's sp
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

	mov esi, [ebx + cpu_esi]
	mov edi, [ebx + cpu_edi]
	mov ebp, [ebx + cpu_ebp]
	;mov eflags, [ebx + cpu_eflags] ;Gets popped by iret

	mov esi, [ebx + cpu_cr0]
	mov cr0, esi
	mov esi, [ebx + cpu_cr2]
	mov cr2, esi

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