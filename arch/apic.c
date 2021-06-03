#include <apic.h>
#include <acpi.h>
#include <io.h>
#include <kprint.h>
#include <paging.h>
#include <pit.h>
#include <pic.h>

static uint32_t lapic_in(uint16_t reg)
{
    return volatile_read32(((uint32_t)get_apic_localaddr()) + reg);
}

static void lapic_out(uint16_t reg, uint32_t data)
{
    volatile_write32(((uint32_t)get_apic_localaddr() + reg), data);
}

static void lapic_write_icr(uint32_t low, uint32_t high)
{
    /* ICR gets registered when we write the low register */
    lapic_out(LAPIC_ICRHI, high);
    lapic_out(LAPIC_ICRLO, low);
}

static void lapic_waitfor_pending_icr()
{
    while (lapic_in(LAPIC_ICRLO) & ICR_SEND_PENDING)
        ;
}


static void lapic_set_lvt(uint16_t reg, uint8_t vector)
{
    lapic_out(reg, (lapic_in(reg) & 0xffffffff) | vector);
}

void lapic_cpu_init(uint8_t cpu)
{
    lapic_out(LAPIC_LDR, (lapic_in(LAPIC_LDR) & 0x00FFFFFF) | (cpu << 24));
    uint32_t apic_id = lapic_in(LAPIC_LDR) >> 24;

    if (cpu == 0) {
        /* Set the error IRQ only on BSP */
        lapic_set_lvt(LAPIC_ERROR, APIC_ERR_IRQ);
    }

    /* Set the spurious irq */
    lapic_set_lvt(LAPIC_SVR, APIC_SPURIOUS_IRQ);

    /* Set the destination mode to flat mode */
    lapic_out(LAPIC_DFR, 0xf0000000);

    lapic_out(LAPIC_TIMER, LAPIC_LVT(0, 0) | LAPIC_LVT_MASKED);
    lapic_out(LAPIC_THERMAL, LAPIC_LVT(0, 0) | LAPIC_LVT_MASKED);
    lapic_out(LAPIC_PERF, LAPIC_LVT(0, 0) | LAPIC_LVT_MASKED);
    lapic_out(LAPIC_LINT0, LAPIC_LVT(0, 7) | LAPIC_LVT_MASKED);
    lapic_out(LAPIC_LINT1, LAPIC_LVT(0, 0) | LAPIC_LVT_TRIGGER_LEVEL);

    lapic_out(LAPIC_TPR, 0);

    lapic_out(LAPIC_SVR, lapic_in(LAPIC_SVR) | LAPIC_ENABLE);
}

void lapic_bsp_init()
{
    pic_disable();
    map_kern_page_flags(get_apic_localaddr(), get_apic_localaddr(),
                        PAGE_READWRITE | PAGE_CACHEDISABLE);
    flush_tlb();

    // Clear task priority to enable all interrupts
    lapic_out(LAPIC_TPR, 0);

    // Logical Destination Mode
    //lapic_out(LAPIC_DFR, 0xffffffff);   // Flat mode
    //lapic_out(LAPIC_LDR, 0x01000000);   // All cpus use logical id 1

    // Enables the local_apic
    // Configure Spurious Interrupt Vector Register
    lapic_out(LAPIC_SVR, LAPIC_ENABLE | APIC_SPURIOUS_IRQ);

    ASSERT(lapic_get_id() == 0);
    lapic_cpu_init(0);
}

void lapic_timer_enable(bool enable)
{
    // TODO write a more clean way of unmasking this
    if (enable)
        lapic_out(LAPIC_TIMER, LAPIC_LVT(APIC_TIMER_IRQ, 0) | LAPIC_LVT_TIMER_PERIODIC);
    else 
        lapic_out(LAPIC_TIMER, LAPIC_LVT(APIC_TIMER_IRQ, 0) | LAPIC_LVT_MASKED | LAPIC_LVT_TIMER_PERIODIC);
}

void lapic_setup_timer(uint32_t ticks, lapic_timermode mode, bool enable)
{
    uint32_t flags = 0;
    switch (mode) {
        case PERIODIC:
            flags |= LAPIC_LVT_TIMER_PERIODIC;
            break;
        case ONE_SHOT:
            flags |= LAPIC_LVT_TIMER_ONESHOT;
            break;
        case TSCDEADLINE:
            flags |= LAPIC_LVT_TIMER_TSCDEADLINE;
            break;
    }

    if (!enable)
        flags |= LAPIC_LVT_MASKED;
    lapic_out(LAPIC_TIMER, LAPIC_LVT(APIC_TIMER_IRQ, 0) | flags);

    uint32_t config = lapic_in(LAPIC_TDCR);
    config &= ~0xf; // clear divisor

    switch(LAPIC_TIME_DIVISOR) {
        case 1:
            config |= (1 << 3) | 3;
            break;
        case 2:
            break;
        case 4:
            config |= 1;
            break;
        case 8:
            config |= 2;
            break;
        case 16:
            config |= 3;
            break;
        case 32:
            config |= (1 << 3);
            break;
        case 64:
            config |= (1 << 3) | 1;
            break;
        case 128:
            config |= (1 << 3) | 2;
            break;
        default:
            VERIFY_UNREACHED();
    }
    lapic_out(LAPIC_TDCR, config);

    if (mode == PERIODIC) {
        lapic_out(LAPIC_TICR, ticks / LAPIC_TIME_DIVISOR);
    }
}

uint32_t lapic_get_timer_currcount()
{
    return lapic_in(LAPIC_TCCR);
}

void lapic_eoi()
{
    lapic_out(LAPIC_EOI, 0x0);
}


uint8_t lapic_get_id()
{
    return lapic_in(LAPIC_ID) >> ICR_DESTINATION_SHIFT;
}

void lapic_broadcast_ipi()
{
    lapic_waitfor_pending_icr();
    
    uint32_t high = 0x0 << ICR_DESTINATION_SHIFT;
    uint32_t low = APIC_IPI_IRQ | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_ALL_EXCLUDING_SELF;
    
    lapic_write_icr(low, high);
}

void lapic_send_ipi(uint8_t cpu)
{
    lapic_waitfor_pending_icr();
    
    uint32_t high = cpu << ICR_DESTINATION_SHIFT;
    uint32_t low = APIC_IPI_IRQ | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND;
    
    lapic_write_icr(low, high);
}

/* AP init functions */

void lapic_send_init(uint8_t apic_id)
{
    lapic_out(LAPIC_ICRHI, apic_id << ICR_DESTINATION_SHIFT);
    lapic_out(LAPIC_ICRLO, ICR_INIT | ICR_PHYSICAL
        | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);

    lapic_waitfor_pending_icr();   
}

void lapic_send_startup(uint8_t apic_id, uint32_t vector)
{
    lapic_out(LAPIC_ICRHI, apic_id << ICR_DESTINATION_SHIFT);
    lapic_out(LAPIC_ICRLO, vector | ICR_STARTUP
        | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORTHAND);
    
    lapic_waitfor_pending_icr();
}

/* IOAPIC */

apic_int_override * ioapic_overrides[MAX_IOAPIC_OVERRIDES];
static uint8_t redirection_entry_count;

static void ioapic_out(uint8_t reg, uint32_t val)
{
    volatile_write32(get_apic_ioaddr() + IOAPIC_REGSEL, reg);
    volatile_write32(get_apic_ioaddr() + IOAPIC_WIN, val);
}

static uint32_t ioapic_in(uint8_t reg)
{
    volatile_write32(get_apic_ioaddr() + IOAPIC_REGSEL, reg);
    return volatile_read32(get_apic_ioaddr() + IOAPIC_WIN);
}

static uint8_t ioapic_read_redirection_entry(uint8_t index)
{
    return (ioapic_in((index << 1) + IOAPIC_REDIR) & 0xff);
}

static void set_redirection_entry(int index, uint8_t interrupt_vector, uint8_t delivery_mode, 
                                  bool logical_destination, bool active_low, 
                                  bool trigger_level_mode, bool masked, uint8_t destination)
{
    uint32_t redirection_entry1 = interrupt_vector | (delivery_mode & 0b111) << 8 | logical_destination << 11 | active_low << 13 | trigger_level_mode << 15 | masked << 16;
    uint32_t redirection_entry2 = destination << 24;

    ioapic_out((index << 1) + IOAPIC_REDIR, redirection_entry1);
    ioapic_out((index << 1) + IOAPIC_REDIR + 1, redirection_entry2);
}

static void ioapic_identitymap_interrupts(uint8_t vector)
{
    set_redirection_entry(vector, vector + IRQ_BASE, 0, false, false, false, true, 0);
}

static void ioapic_override_interrupts(uint8_t src, uint8_t gsi, uint16_t flags)
{
    bool active_low = false;
    
    switch ((flags & 0b11)) {
    case 0:
        active_low = false;
        break;
    case 1:
        active_low = false;
        break;
    case 3:
        active_low = true;
        break;
    }

    bool trigger_level_mode = false;
    
    switch ((flags >> 2) & 0b11) {
    case 0:
        trigger_level_mode = false;
    case 1:
        trigger_level_mode = false;
        break;
    case 3:
        trigger_level_mode = true;
        break;
    }

    /* Set the redirecion entry to route to the BSP for now and mask it*/
    set_redirection_entry(gsi - get_gsibase(), src + IRQ_BASE, 0, false,
                          active_low, trigger_level_mode, true, 0);
}

static void map_redirection_interrupts(uint8_t vector)
{
    kprint(INFO, "IOAPIC: Mapping %d vector\n", vector);
    for (int i = 0; i < MAX_IOAPIC_OVERRIDES; i++) {
        apic_int_override * override = ioapic_overrides[i];
        
        if (override == NULL)
            break;

        if (override->source != vector)
            continue;
        
        ioapic_override_interrupts(override->source, override->interrupt, override->flags);
        
        kprint(INFO, "IOAPIC: Mapped vector %d to GSI %d\n", override->source, override->interrupt);
        return;
    }

    if (ioapic_read_redirection_entry(vector) == 0) {
        ioapic_identitymap_interrupts(vector);
        kprint(INFO, "IOAPIC: Mapped vector %d to GSI %d\n", vector, vector);
    }
}


void ioapic_mask_redirection_entry(uint8_t index)
{
    uint32_t redirection_entry = ioapic_in((index << 1) + IOAPIC_REDIR);
    if (redirection_entry & (1 << 16))
        return;
    ioapic_out((index << 1) + IOAPIC_REDIR, redirection_entry | (1 << 16));
}

void ioapic_unmask_redirection_entry(uint8_t index)
{
    uint32_t redirection_entry = ioapic_in((index << 1) + IOAPIC_REDIR);
    if (!(redirection_entry & (1 << 16)))
        return;
    ioapic_out((index << 1) + IOAPIC_REDIR, redirection_entry & ~(1 << 16));
}

void ioapic_unmask_all()
{
    for (int i = 0; i < redirection_entry_count; i++) {
        ioapic_unmask_redirection_entry(i);
    }
}

void ioapic_mask_all()
{
    for (int i = 0; i < redirection_entry_count; i++) {
        ioapic_mask_redirection_entry(i);
    }
}


apic_int_override ** ioapic_get_redirection_overrides()
{
    return ioapic_overrides;
}

void ioapic_init()
{
    map_kern_page_flags(get_apic_ioaddr(), get_apic_ioaddr(),
                        PAGE_READWRITE | PAGE_CACHEDISABLE);
    kprint(INFO, "FLUSING init ioapic BSP\n");
    flush_tlb();

    kprint(INFO, "FLUSHED!!!!\n");

    redirection_entry_count = ((ioapic_in(0x01) >> 16) & 0xff) + 1;
    uint32_t gsi_base = get_gsibase();

    for (int i = 0; i < redirection_entry_count; i++) {
        map_redirection_interrupts(i);
    }

    ioapic_mask_all();
}