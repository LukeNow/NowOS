#include "../include/kdef.h"
#include "../include/linked_list.h"
#include "../include/process.h"

LINKED_LIST_INIT(process_list);

process_t *create_process(void (*main)(), char *name);

void start_process(process_t *process);

void fork_process_task(process_t *process);
