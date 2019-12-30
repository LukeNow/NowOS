#include <kdef.h>
#include <ipc.h>
#include <scheduler.h>
#include <process.h>
#include <kprint.h>

extern process_t * process_tbl[MAX_PROC_NUM];

void print_message(message_t * msg)
{
	kprint(INFO, "--Printing message--\n");
	kprint(INFO, "Sender ID %d\n", msg->sender_id);
	kprint(INFO, "Command %d\n", msg->command);
	kprint(INFO, "BODY: %d\n", msg->body[0]);
}

void async_send_msg(id_t to_id, message_t * msg, flags_t flags)
{
	hard_lock_scheduler();
	
	process_t * recv_proc = process_tbl[GET_PROC_ID(to_id)];
	if (recv_proc == NULL) {
		kprint(ERROR, "Async_send_Msg: could not find recv proc\n");
		hard_unlock_scheduler();
		return;
	}
	
	task_control_block_t * recv_task = recv_proc->proc_tasks[GET_TASK_ID(to_id)];
	if (recv_task == NULL) {
		kprint(ERROR, "Async_send_msg: Could not find recv task\n");
		hard_unlock_scheduler();
		return;
	}
	
	message_buf_t * recv_buf = &recv_task->msg_buf;
	
	/* Our message buf is full but we dont want to block on this so
	 * we just have to ignore this message 
	 *
	 * TODO is there a way to inform people that we missed their message?*/
	if (recv_buf->curr_size == recv_buf->size && flags & DONT_BLOCK) {
		hard_unlock_scheduler();
		return;
	}
	/* The message buf is full, we must block and wait to be woken
	 * up to send this message */

	if (recv_buf->curr_size == recv_buf->size) {
	//while (recv_buf->curr_size == recv_buf->size) {
		/* Queue the sender task onto the recver taks wait queue */
		linked_list_enqueue(current_task, &recv_task->msg_wait_queue);
		hard_unlock_scheduler();
		/* We block and wait to be woken up to send the message */
		block_process();
		hard_lock_scheduler();
	}
	
	ASSERT(recv_buf->curr_size != recv_buf->size);
	
	/* Push the message to the receiver */
	push_circ_buf(msg, &recv_task->msg_buf);
	
	int recv_task_index = 
		linked_list_search(recv_task, &current_task->msg_wait_queue);
	/* If the task was waiting for us to send to it, unblock it */
	if (recv_task_index != -1) {
		linked_list_remove(recv_task_index, &current_task->msg_wait_queue);
		unblock_process(to_id);
	}

	//TODO implement ACL to see if this receiver wants to receive messages or not
	hard_unlock_scheduler();
}

/* TODO there mus be a better way to implement this control flow */
static void any_receive_msg(message_t * buf, flags_t flags)
{
	
	message_buf_t * recv_buf = &get_current_task()->msg_buf;
	message_t * temp_buf;
	
	if (recv_buf->curr_size == 0) {
		memset(buf, 0, sizeof(message_t));
		return;
	}
		
	pop_circ_buf(temp_buf, recv_buf);
}

void async_receive_msg(id_t from_id, message_t * buf, flags_t flags) 
{
	hard_lock_scheduler();
	
	/*
	kprint(INFO, "aysnc_receive_msg pid %d: from pid %d\n", 
			GET_PROC_ID(get_current_id()), GET_PROC_ID(from_id));
	*/
	message_buf_t * recv_buf = &get_current_task()->msg_buf;
	
	if (from_id == ANY_ID) {
		/* HARD LOCK SCHEDULER HELD */
		any_receive_msg(buf, flags);
		hard_unlock_scheduler();
		return;
	}

	process_t * sender_proc = process_tbl[GET_PROC_ID(from_id)];
	if (sender_proc == NULL) {
		kprint(ERROR, "Async_send_Msg: could not find recv proc\n");
		hard_unlock_scheduler();
		return;
	}
	
	task_control_block_t * sender_task = 
		sender_proc->proc_tasks[GET_TASK_ID(from_id)];
	if (sender_task == NULL) {
		kprint(ERROR, "Async_send_msg: Could not find recv task\n");
		hard_unlock_scheduler();
		return;
	}

	//TODO implement ACL to see if this receiver wants to receive messages or not
	//TODO right now we are assuming that a proc is waiting to send or to receive
	//from a proc, NOT both. we should account for this
	
	/* If the task was waiting for us to receive from it, unblock it */
	int sender_task_index = 
		linked_list_search(sender_task, &current_task->msg_wait_queue);
	if (sender_task_index != -1) {
		linked_list_remove(sender_task_index, &current_task->msg_wait_queue);
		unblock_process(from_id);
	}
	
	
	/* If our message buf is empty, that means we have not received from
	 * the person we were waiting to receive from, so we block or return*/

	/* Our message buf is empty so there is nothing to receive!
	 * TODO is there a way to inform people that we missed their message?*/
	if (recv_buf->curr_size == 0 && (flags & DONT_BLOCK)) {
		hard_unlock_scheduler();
		return;
	}
	
	if (recv_buf->curr_size == 0) {
	//while (recv_buf->curr_size == 0) {
		/* Queue the receiver task onto the recver taks wait queue */
		linked_list_enqueue(current_task, &sender_task->msg_wait_queue);
		hard_unlock_scheduler();
		/* We block and wait to be woken up to send the message */

		block_process();
		hard_lock_scheduler();
	}
	
	ASSERT(recv_buf->curr_size != 0);
	pop_circ_buf(buf, recv_buf);

	hard_unlock_scheduler();
}
