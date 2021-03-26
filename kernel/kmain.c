#include <kdef.h>
#include <tty.h>
#include <kprint.h>
#include <machine.h>
#include <processor.h>
#include <io.h>
#include <multiboot.h>
#include <kheap.h>
#include <string.h>
#include <mm.h>
#include <kdef.h>
#include <paging.h>
#include <sorted_array_list.h>
#include <pit.h>
#include <linked_list.h>
#include <thread.h>
#include <scheduler.h>
#include <string.h>
#include <circ_buf.h>
#include <ktest.h>
#include <acpi.h>
#include <smp.h>
#include <apic.h>
#include <atomic.h>
/* Use the label addresses as the addresses for the start and end points of 
 * important areas of memory */
extern void heap_top();
extern void heap_bottom();
extern void stack_top();

static void reserve_memorymap_pages(multiboot_info_t* mbt)
{
	multiboot_memory_map_t* entry = mbt->mmap_addr;
	while(entry < mbt->mmap_addr + mbt->mmap_length) {
		/* For now we reserve every type that is not available
		   Even ACPI reclaimable spaces, lets just reserve for now */
		if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
			uint32_t mem_addr_lower = entry->addr_lower;
			uint32_t mem_addr_upper = mem_addr_lower + entry->size;
			mem_addr_lower = ALIGN_DOWN(mem_addr_lower, PAGE_SIZE);
			mem_addr_upper = ALIGN_UP(mem_addr_upper, PAGE_SIZE);
			kprint(INFO, "MEMORY TYPE at addr %x :%d\n", (uint32_t)mem_addr_lower, entry->type);
			while (mem_addr_lower < mem_addr_upper) {
				
				mem_alloc_region(mem_addr_lower, PAGE_SIZE, MEM_IDENTITYREGION);
				//mem_set_tbl(mem_addr_lower);
				//map_kern_page(mem_addr_lower, mem_addr_lower);

				mem_addr_lower += PAGE_SIZE;
			}
		}

		entry = (multiboot_memory_map_t*) ((unsigned int) entry + entry->size + sizeof(entry->size));
	}
}


void ap_kmain()
{	
	scheduler_init();

	VERIFY_UNREACHED();
}


void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	kprint(INFO, "KMAIN entered\n");
	/* Verify interrupts are disabled until we can
       set up the apic */
	disable_int();
	
	int rc;
	uint32_t mem_limit = mbt->mem_upper * 1024;
	kprint(INFO, "MEM SIZE: %x\n", mem_limit);

	multiboot_memory_map_t* mmap = mbt->mmap_addr;
	init_early_heap();
	init_mem_manager(mem_limit); //Physical page alloc
	
	/* Initialize page tables so that we identity map the first
       2 MBs and then have the first 4 MBs mapped to the higher half */
	init_kern_paging();
	
	//reserve_memorymap_pages(mbt);
	/* TODO init_acpi() must come before init_local_apic() */
	init_acpi();

	lapic_bsp_init();
	
	init_kheap(); //Regular kheap
	init_page_heap();
	init_threading();
	init_smp();

	ioapic_init();
	
	//lapic_setup_timer(32, PERIODIC, true);

	//kprint(INFO, "WE MADE IT\n");
	ioapic_unmask_all();
	//enable_int();
	
	//
	//for (;;)
	//kprint(INFO, "INT MADE IT\n");
	   
	processor_set_info();
	scheduler_init();
	//lapic_send_ipi(1);
	//pause();
	//for (;;);
	//lapic_broadcast_ipi();
	//register_dump();

	run_ktest_suite();
	
	PANIC("KMAIN STOP"); 
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
