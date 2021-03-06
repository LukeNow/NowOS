#include <kdef.h>
#include <apic.h>
#include <processor.h>
#include <kprint.h>

static processor_t processor_tbl[MAX_CPU_COUNT];

uint16_t processor_get_id()
{
    return lapic_get_id();
}

processor_t * processor_get_info_id(uint16_t cpu_id)
{
    /* If there hasnt been a thread set on this processor, 
       we havent ever cached anything so lets start that */
    if (processor_tbl[cpu_id].current_thread == NULL)
        processor_set_info();
    
    return &processor_tbl[cpu_id];
}

processor_t * processor_get_info()
{
    uint16_t id = processor_get_id();
    ASSERT(id < MAX_CPU_COUNT);

    return &processor_tbl[id];
}

void processor_set_info()
{
    processor_t * proc = processor_get_info();
    proc->id = processor_get_id();
    //kprint(INFO, "proc id set on proc %d\n", proc->id);
}