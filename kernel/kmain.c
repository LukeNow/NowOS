#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"
#include "../include/kprint.h"
#include "../include/machine.h"
#include "../include/processor.h"
#include "../include/io.h"
#include "../include/multiboot.h"
#include "../include/kheap.h"
#include "../include/string.h"

#define PAGE_SIZE 4096

void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	
	extern void heap_top();
	extern void heap_bottom();
	extern void stack_top();

	unsigned int addr = 0;
	int i = 0;

	
	multiboot_memory_map_t* mmap = mbt->mmap_addr;
	
	//kprint(INFO, "HELLO Beautiful world :)\n");
	//kprint(INFO, "MAGIC = %x\n", magic);
	
	
	//kprint(INFO, "HEAP BOTTOM: %x\n", (uint32_t)heap_bottom);
	
	//kprint(INFO, "BINARY 3: %x\n", 0x0FFFFFFF);
	/*
	addr = early_kmalloc(PAGE_SIZE);
	kprint(INFO, "ADDR: %x\n", (unsigned int) addr);
	memset((uint32_t*) addr, 0x01, PAGE_SIZE * 3); */
	PANIC("WE DID IT");
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
