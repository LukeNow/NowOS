#include <stddef.h>
#include <stdint.h>
#include "../include/kdef.h"
#include "../include/kprint.h"
#include "../include/linked_list.h"
#include "../include/scheduler.h"
#include "../include/kheap.h"
#include "../include/kprint.h"
#include "../include/process.h"
#include "../include/ipc.h"

static void handle_messages()
{
	process_t * sys_proc = get_process_from_pid(SYSTEM_PID);
	ASSERT(sys_proc);

	for (;;) {
		message_t msg;
		id_t id;
		
		async_receive_msg(ANY_ID, &msg, DONT_BLOCK);

		switch(msg.command) {
			case SYS_CMD:
				id = (id_t)msg.body[0];
				unblock_process(id);
				kprint(INFO, "System proc: unblocked pid %d\n", GET_PROC_ID(id));
				break;
			default:
				kprint(INFO, "System proc: command is null\n");
		}
		
		yield_process();
	}
}

id_t create_system_process()
{
	int rc;
	proc_id_t pid = create_process(handle_messages, 0, 0, "System process");
	if (pid == FAIL_ID) {
		kprint(ERROR, "Failed creating system process\n");
		return FAIL_ID;
	}
	
	rc = start_process(pid);
	if (rc == FAILURE) {
		kprint(ERROR, "Failed to start system process\n");
		return FAIL_ID;
	}

	return MAKE_ID(pid, 0);
}
