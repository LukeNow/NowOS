#include <kdef.h>
#include <apic.h>
#include <processor.h>
#include <kprint.h>

static processor_t processor_tbl[MAX_CPU_COUNT];

uint16_t processor_get_id()
{
    return lapic_get_id();
}

processor_t * processor_get_info()
{
    uint16_t id = processor_get_id();
    ASSERT(id < MAX_CPU_COUNT);

    return &processor_tbl[id];
}