#include <kdef.h>
#include <kprint.h>
#include <smp.h>
#include <string.h>
#include <processor.h>
#include <kheap.h>
#include <gdt.h>
#include <idt.h>
#include <acpi.h>
#include <apic.h>
#include <pit.h>
#include <atomic.h>
#include <machine.h>
#define AP_START_BASE 0x8000

extern uintptr_t * io_apic_addr;
extern uintptr_t * local_apic_addr;
extern uint32_t apic_cpu_count;
extern uint8_t apic_processor_ids[MAX_CPU_COUNT];
extern uint8_t apic_cpu_ids[MAX_CPU_COUNT];

extern void ap_init_stacks();
extern void ap_start();
extern void ap_cpu_cr0();
extern void ap_cpu_cr3();
extern void ap_cpu_cr4();
extern void ap_cpu_idtr();
extern void ap_cpu_gdtr();
extern void ap_cpu_id();

extern void ap_kmain();
void init_ap(uint32_t processor_id)
{
    kprint(INFO, "PROCESSOR INITIALIZED %d\n", processor_id);

    lapic_cpu_init(processor_id);

    kprint(INFO, "Processor with old id %d is now %d\n", processor_id, lapic_get_id());
    enable_int();


    ap_kmain();
    pause();

    PANIC("PROCESSOR STOPPED");
}

void init_smp()
{

    uint8_t * ap_start_code = (uint32_t) ap_start;
    uint8_t * init_stack_addr = (uint32_t) ap_init_stacks;
    size_t ap_start_code_len = init_stack_addr - ap_start_code;

    /* Copy the code to where the APs will know where to execute from on startup */
    memcpy((void *)AP_START_BASE, ap_start_code, ap_start_code_len);

    uint32_t * raw_stack_ptr = (uint32_t)(ap_init_stacks - ap_start) + AP_START_BASE;

    /* Just take a few pages from our static heap
       TODO flag these pages to be reclaimed somehow */
    uint32_t num_pages = (ALIGN_UP((THREAD_STACK_SIZE * apic_cpu_count), PAGE_SIZE)) / PAGE_SIZE;
    uint32_t stack_pages = early_kmalloc_pages(num_pages);
    /* Start at the top of the thread stack */
    uint32_t stack_ptr = stack_pages + THREAD_STACK_SIZE;
    for (int i = 0; i < MAX_CPU_COUNT; i++) {
        *raw_stack_ptr = stack_ptr;

        raw_stack_ptr = (uint32_t)raw_stack_ptr + sizeof(uint8_t *);
        stack_ptr += THREAD_STACK_SIZE;
    }

    uint32_t * raw_ap_cpu_cr0 = ((uint32_t)(ap_cpu_cr0 - ap_start)) + AP_START_BASE;
    uint32_t * raw_ap_cpu_cr3 = ((uint32_t)(ap_cpu_cr3 - ap_start)) + AP_START_BASE;
    uint32_t * raw_ap_cpu_cr4 = ((uint32_t)(ap_cpu_cr4 - ap_start)) + AP_START_BASE;
    *raw_ap_cpu_cr0 = get_cr0();
    *raw_ap_cpu_cr3 = get_cr3();
    *raw_ap_cpu_cr4 = get_cr4();

    uint8_t * raw_ap_cpu_gdtr = ((uint32_t)(ap_cpu_gdtr - ap_start)) + AP_START_BASE;
    uint8_t * raw_ap_cpu_idtr = ((uint32_t)(ap_cpu_idtr - ap_start)) + AP_START_BASE;
    uint16_t * raw_ap_cpu_id = ((uint32_t)(ap_cpu_id - ap_start)) + AP_START_BASE;
    memcpy(raw_ap_cpu_gdtr, get_gdt_desc(), sizeof(gdt_reg_desc_t));
    memcpy(raw_ap_cpu_idtr, get_idt_desc(), sizeof(idt_reg_desc_t));

    kprint(INFO, "Initializing %d cpus\n", apic_cpu_count);

    uint8_t bsp_id = lapic_get_id();
    /* MAKE SURE NOT TO CALL THE ID OF THE BSP */
    *raw_ap_cpu_id = 0;
    uint16_t active_cpus = 1;
    for(int i = 0; i < apic_cpu_count; i++) {
        uint8_t acpi_id = apic_cpu_ids[i];
        if (acpi_id == bsp_id) {
            continue;
        }

        //kprint(INFO, "SENDING INIT TO CPU %d\n", acpi_id);
        lapic_send_init(acpi_id);
        //kprint(INFO, "SENDING STARTUP TO CPU %d\n", acpi_id);
        lapic_send_startup(acpi_id, 0x08);
        //kprint(INFO, "INIT DONE on CPU %d\n", acpi_id);
        
        while (atomic_load_16(raw_ap_cpu_id, mem_order_consume) != active_cpus)
            ;

        kprint(INFO, "BSP: AP %d initialized\n", *raw_ap_cpu_id, active_cpus);
        active_cpus++;
    }
}