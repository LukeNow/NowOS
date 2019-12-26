#ifndef _IPC_H
#define _IPC_H

#include "../include/kdef.h"
#include "../include/string.h"
#include "../include/circ_buf.h"

#define BODY_LEN 16

#define MAKE_MESSAGE(name, id, cmd, body_ptr) \
	message_t (name); \
	(name).sender_id = (id); \
	(name).command = (cmd); \
	memcpy(&(name).body, (body_ptr), BODY_LEN)

typedef unsigned int command_t;
typedef circ_buf_t message_buf_t;

typedef struct message_t {
	id_t sender_id;
	command_t command;
	char body[BODY_LEN];
}message_t;

void async_send_msg(id_t to_id, message_t *msg, flags_t flags);
void async_receive_msg(id_t from_id, message_t * buf, flags_t flags);

#endif
