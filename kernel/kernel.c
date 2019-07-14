#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/tty.h"
#include "../include/kprint.h"
#include "../include/machine.h"

void kernel_main(void)
{
	//term_init();
	//kprint("The number is %d\n", 10);
	kprint(INFO, "Hello world!\n");
	kprint(INFO, "This would be an error!!!!\n");
	kprint(WARN, "The number is %d, the string is %s\n", 10, "Hello world!");
	kprint(ERROR, "The hex number is %x, hehe\n", 0xABCD);
	register_dump();
	
	/* Hang, we dont return from this function */
	for(;;) 
	
	return;
}
