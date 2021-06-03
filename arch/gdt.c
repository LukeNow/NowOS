#include <kdef.h>
#include <kprint.h>
#include <gdt.h>
#include <string.h>

#define NUM_GDT_DESC 5

uint64_t gdt_table[NUM_GDT_DESC];

static gdt_reg_desc_t gdt_desc;

static uint64_t
create_descriptor(uint32_t base, uint32_t limit, uint16_t flag)
{
	uint64_t descriptor;

	// Create the high 32 bit segment
	descriptor  =  limit       & 0x000F0000;         // set limit bits 19:16
	descriptor |= (flag <<  8) & 0x00F0FF00;         // set type, p, dpl, s, g, d/b, l and avl fields
	descriptor |= (base >> 16) & 0x000000FF;         // set base bits 23:16
	descriptor |=  base        & 0xFF000000;         // set base bits 31:24

	// Shift by 32 to allow for low part of segment
	descriptor <<= 32;

	// Create the low 32 bit segment
	descriptor |= base  << 16;                       // set base bits 15:0
	descriptor |= limit  & 0x0000FFFF;               // set limit bits 15:0

	return descriptor;
}

gdt_reg_desc_t * get_gdt_desc()
{
	return &gdt_desc;
}

gdt_reg_desc_t * create_gdt_table()
{
	memset(gdt_table, 0, sizeof(gdt_table));

	gdt_table[0] = create_descriptor(0, 0, 0);
	gdt_table[1] = create_descriptor(0, 0xFFFFFFFF, (GDT_CODE_PL0));
	gdt_table[2] = create_descriptor(0, 0xFFFFFFFF, (GDT_DATA_PL0));
	gdt_table[3] = create_descriptor(0, 0xFFFFFFFF, (GDT_CODE_PL3));
	gdt_table[4] = create_descriptor(0, 0xFFFFFFFF, (GDT_DATA_PL3));

	kprint(INFO, "CODE1 %x\n", gdt_table[1]);
	kprint(INFO, "CODE2 %x\n", gdt_table[1] >> 32);
	kprint(INFO, "DATA1 %x\n", gdt_table[2]);
	kprint(INFO, "DATA2 %x\n", gdt_table[2] >> 32);
	kprint(INFO, "CODE1-3 %x\n", gdt_table[3]);
	kprint(INFO, "CODE2-3 %x\n", gdt_table[3] >> 32);
	kprint(INFO, "DATA1-3 %x\n", gdt_table[4]);
	kprint(INFO, "DATA2-3 %x\n", gdt_table[4] >> 32);

	gdt_desc.base = &gdt_table;
	gdt_desc.limit = sizeof(gdt_table) - 1;

	kprint(INFO, "PRINTING DESC at addr %x with size %d\n", &gdt_desc, sizeof(gdt_table));
	return &gdt_desc;
}
