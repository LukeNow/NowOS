#include <stdint.h>
#include <stddef.h>
#include "../include/kheap.h"
#include "../include/kprint.h"
#include "../include/kdef.h"

extern void heap_bottom();
extern void heap_top();
extern void early_heap_bottom();
extern void early_heap_top();
/* Start early kheap at bottom of the stack
 * It will grow towards our kernel stack */
uint32_t early_placement_address = 0;

uint32_t early_kmalloc(size_t size)
{	
	if (early_placement_address == 0)
		early_placement_address = (uint32_t)early_heap_bottom;
	
	uint32_t temp = early_placement_address;
	if (size > PAGE_SIZE)
		return -1;

	if (size == PAGE_SIZE) {
		temp += PAGE_SIZE;
		temp = ALIGN_UP(temp, PAGE_SIZE);
	}
	
	else /*if (size > sizeof(uint16_t))*/ {
		temp += size;
		temp = ALIGN_UP(temp, sizeof(uint32_t));
	}
	/*
	else if (size > sizeof(uint8_t)) {
		temp += size;
		temp = ALIGN_UP(temp, sizeof(uint16_t));

	}
	else {
		temp += size;
	}
	*/
	early_placement_address = temp;

	
	return early_placement_address;
}

/* return physical address */
uint32_t early_kmalloc_pages(int num_pages)
{
	uint32_t temp;
	uint32_t temp_bottom;
	
	if (early_placement_address == 0) {
		early_placement_address = (uint32_t)early_heap_bottom;
		early_placement_address = ALIGN_UP(early_placement_address, PAGE_SIZE);
	}
	
	//kprint(INFO, "Placement %x\n", early_placement_address);
	temp = early_placement_address;
	temp = ALIGN_UP(temp, PAGE_SIZE);
	temp_bottom = early_placement_address;
	
	
	if (!IS_ALIGNED(early_placement_address, PAGE_SIZE)) {
		PANIC("PAGE IS NOT ALLIGNED");
	}
	

	//PANIC("STOP1");

	for (int i = 0; i < num_pages; i++) {
		if (temp >= (uint32_t) early_heap_top){
			WARN("WEVE HIT THE TOP OF HEAP");
		}
		temp += PAGE_SIZE;
	}
	
	temp_bottom = temp_bottom - KERN_VIRTUAL_ADDR;
	early_placement_address += (num_pages * PAGE_SIZE);
	return temp_bottom;
}


