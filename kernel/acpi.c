#include <acpi.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <kprint.h>
#include <mm.h>
#include <apic.h>

void * io_apic_addr;
void * local_apic_addr;
uint32_t gsi_base;
uint32_t apic_cpu_count;
uint8_t apic_processor_ids[MAX_CPU_COUNT];
uint8_t apic_cpu_ids[MAX_CPU_COUNT];

static void check_rsdp(rsdp_desc * desc)
{
	unsigned char sum = 0;
	
	for (int i = 0; i < sizeof(rsdp_desc); i++) {
		sum += ((char *) desc)[i];
	}
	
	if (sum != 0) {
		PANIC("RSDP CHECKSUM FAILED\n");
	}
}

static void check_acpi_header(acpi_header * header)
{
	unsigned char sum = 0;

	for (unsigned int i = 0; i < header->length; i++) {
		sum += ((char *) header)[i];
	}

	if (sum != 0) {
		PANIC("ACPI_HEADER CHECKSUM FAILED\n");
	}
}
static int acpi_header_checksum(acpi_header * header) 
{
	unsigned char sum = 0;
	
	for (unsigned int i = 0; i < header->length; i++) {
		sum += ((char *) header)[i];
	}
	
	return sum == 0;
}

static parse_processor_table(apic_local_apic * entry)
{
	if (apic_cpu_count < MAX_CPU_COUNT) {
		apic_cpu_ids[apic_cpu_count] = entry->apic_id;
		apic_processor_ids[apic_cpu_count] = entry->acpi_processor_id;
		apic_cpu_count++;
		kprint(INFO, "**FOUND CPU** APIC_ID: %d processorID: %d\n", entry->apic_id, entry->acpi_processor_id);
	}
}

static parse_io_table(apic_io_apic * entry)
{
	io_apic_addr = entry->ioApicAddress;
	gsi_base = entry->globalSystemInterruptBase;
	kprint(INFO, "ioAPIC addr %x, ioAPIC intr base %d\n", io_apic_addr, entry->globalSystemInterruptBase);
}

static parse_interrupt_override(apic_int_override * entry)
{
	apic_int_override ** overrides = ioapic_get_redirection_overrides();
	for (int i = 0; i < MAX_IOAPIC_OVERRIDES; i++) {
		if (overrides[i] == NULL) {
			overrides[i] = entry;
			return;
		}
	}

	kprint(ERROR, "Max IOAPIC entries reached\n");
}

static void parse_apic_table(acpi_madt * madt)
{
	check_acpi_header((acpi_header *)&madt->header);
	memset(apic_processor_ids, 0, sizeof(apic_processor_ids));
	memset(apic_cpu_ids, 0, sizeof(apic_cpu_ids));
	/* We need to set all entries to null so we dont override other entries */
	memset(ioapic_get_redirection_overrides(), 0, sizeof(apic_int_override *) * MAX_IOAPIC_OVERRIDES);

	local_apic_addr = madt->localApicAddr;
	
	uint8_t * entry = ((uint32_t)madt) + sizeof(acpi_madt);
	uint8_t * end = ((uint32_t)madt) + madt->header.length;
	while (entry < end) {
		apic_header * header = entry;
		if (header->type == APIC_TYPE_LOCAL_APIC) {
			parse_processor_table((apic_local_apic *) (header + 1));
		} else if (header->type == APIC_TYPE_IO_APIC) {
			parse_io_table((apic_io_apic *) (header + 1));
		} else if (header->type == APIC_TYPE_INTERRUPT_OVERRIDE) {
			parse_interrupt_override((apic_int_override *) (header + 1));
		} else {
			kprint(INFO, "APIC VERSION %d FOUND\n", header->type);
		}

		entry = (apic_header *)((uint32_t)(entry) + header->length);
	}
}

static void parse_acpi_tables(acpi_header * rsdt) 
{
	/* The first header is the RSDT header. SO lets
       check it and then find how many entries there are */
	check_acpi_header(rsdt);
	
	int entries = (rsdt->length - sizeof(acpi_header)) / 
					sizeof(acpi_header *);

	acpi_header ** h = ((uint32_t)rsdt) + sizeof(acpi_header);

	kprint(INFO, "%d ACPI entries found\n", entries);
	for (int i = 0; i < entries; i++) {
		acpi_header * entry = h[i];
		check_acpi_header(entry);
		
		if (strncmp(entry->signature, "APIC", 4) == 0) {
			parse_apic_table((acpi_madt *)entry);
		}
		else {
			char name[5];
			memcpy(name, entry->signature, 4);
			name[4] = '\0';
			kprint(INFO, "OTHER ACPI TABLE FOUND: %s\n", name);
		}

	}
}

uint32_t get_apic_localaddr()
{
	return local_apic_addr;
}

uint32_t get_apic_ioaddr()
{
	return io_apic_addr;
}

uint32_t get_gsibase()
{
	return gsi_base;
}

void init_acpi()
{
	const char * rsdp_str = "RSD PTR ";
	rsdp_desc * rsdp = NULL;
	for (uint32_t i = 0; i < MB(1); i += 16) {
		if (strncmp(rsdp_str, (char *) i, 8) == 0) {
			rsdp = (uint32_t *) i;
			break;
		}
	}
	
	if (rsdp == NULL) {
		PANIC("RSDP NOT FOUND!\n");
	}
	
	char version = rsdp->revision;
	kprint(INFO, "ACPI version: %d\n", (int)version);
	
	mem_alloc_region(rsdp->rsdt_addr, PAGE_SIZE, MEM_IDENTITYREGION | MEM_ALREADYEXISTS);
	kprint(INFO, "ALLOCED\n");
	check_rsdp(rsdp);
	acpi_header * rsdt = (acpi_header *) rsdp->rsdt_addr;
	if (!acpi_header_checksum(rsdt)) {
		PANIC("rsdt checksum fail\n");
	}

	apic_cpu_count = 0;
	memset(apic_cpu_ids, 0, sizeof(apic_cpu_ids));
	memset(apic_processor_ids, 0, sizeof(apic_processor_ids));

	parse_acpi_tables(rsdt);
}

