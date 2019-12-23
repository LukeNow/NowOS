#ifndef _IPC_H
#define _IPC_H

#include "../include/kdef.h"
#include "../include/string.h"

#define BODY_LEN 16

#define MAKE_ID(proc_id, task_id) \
	((proc_id) << 8) | (task_id)

#define MAKE_MESSAGE(name, id, cmd, body_ptr) \
	message_t (name); \
	(name).sender_id = (id); \
	(name).command = (cmd); \
	memcpy(&(name).body, (body_ptr), BODY_LEN)

typedef unsigned int command_t;

typedef struct message_t {
	id_t sender_id;
	command_t command;
	char body[BODY_LEN];
}message_t;

void send_msg(id_t receiver, message_t msg);
void receive_msg();

#endif
