#ifndef _ACPI_H
#define _ACPI_H

#include <stdint.h>
#include <kdef.h>

struct rsdp_desc {
	char signature[8];
	uint8_t checksum;
	char OEMID[6];
	uint8_t revision;
	uint32_t rsdt_addr;
} __attribute__ ((packed));
typedef struct rsdp_desc rsdp_desc;

typedef struct acpi_header {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t creatorID;
	uint32_t creatorRevision;
} __attribute__ ((packed));
typedef struct acpi_header acpi_header;

typedef struct acpi_fadt {
    acpi_header header;
    uint32_t firmwareControl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t preferredPMProfile;
    uint16_t sciInterrupt;
    uint32_t smiCommandPort;
    uint8_t  acpiEnable;
    uint8_t  acpiDisable;
    // TODO - fill in rest of data
} __attribute__ ((packed));
typedef struct acpi_fadt acpi_fadt;

typedef struct acpi_madt {
    acpi_header header;
    uint32_t localApicAddr;
    uint32_t flags;
} __attribute__ ((packed));
typedef struct acpi_madt acpi_madt;

typedef struct rsdt {
	acpi_header header;
	/* This pointer is a dummy pointer that signifies that
	 * we have headers below the rsdt's header. 
	 * Because we don't know everything at compile time, we 
	 * are just using this as a starting off point to search for
	 * all the other headers */
	uint32_t * dummy_other_headers; 
} rsdt;

/*** APIC definitons ***/

typedef struct apic_header {
    uint8_t type;
    uint8_t length;
} __attribute__ ((packed));
typedef struct apic_header apic_header;

typedef struct apic_local_apic {
    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__ ((packed));
typedef struct apic_local_apic apic_local_apic;

typedef struct apic_io_apic {
    uint8_t ioApicId;
    uint8_t reserved;
    uint32_t ioApicAddress;
    uint32_t globalSystemInterruptBase;
} __attribute__ ((packed));
typedef struct apic_io_apic apic_io_apic;

typedef struct apic_int_override {
    uint8_t bus;
    uint8_t source;
    uint32_t interrupt;
    uint16_t flags;
} __attribute__ ((packed));
typedef struct apic_int_override apic_int_override;

// APIC structure types
#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

uint32_t get_apic_ioaddr();
uint32_t get_gsibase();
uint32_t get_apic_localaddr();

void init_acpi();

#endif
