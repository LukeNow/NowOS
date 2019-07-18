#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"
#include "../include/kprint.h"
#include "../include/machine.h"
#include "../include/processor.h"
#include "../include/io.h"
#include "../include/multiboot.h"

void kmain(multiboot_info_t* mbt, unsigned int magic)
{
	
	int i = 0;
	multiboot_memory_map_t* mmap = mbt->mmap_addr;
	
	kprint(INFO, "KMAIN mmap_addr: %x\n", mmap);
	kprint(INFO, "MAGIC: %x\n", magic);
	kprint(INFO, "Hello world!\n");
	kprint(INFO, "This would be an error!!!!\n");
	kprint(WARN, "The number is %d, the string is %s\n", 10, "Hello world!");
	kprint(ERROR, "The hex number is %x, hehe\n", 0xABCD);
	
	kprint(INFO, "ADDR IS %x\n", &i);
	
	PANIC("we fucked up")

	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
