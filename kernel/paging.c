#include <kdef.h>
#include <paging.h>
#include <kheap.h>
#include <kprint.h>
#include <string.h>
#include <mm.h>
#include <lock.h>

#define DIR_SIZE 2 << 22
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define GET_TBL_PHYS_ADDR(x) ((x) & ~0x3ff)
#define PAGE_GET_PHYS_ADDRR(x) (*x & ~0xfff)

SPINLOCK_INIT(lock);

page_dir_t * kernel_dir; //physical_addr

static void set_page_dir_entry(page_dir_entry_t *entry, uint32_t phys_addr,
			 int read_write, int privledge, int write_through,
			 int cache_disabled, int page_size)
{
	uint32_t temp_entry = 0;
	uint32_t temp_addr = ALIGN_UP(phys_addr, PAGE_SIZE);

	temp_entry |= (temp_addr & ~0x03FF);
	temp_entry |= 1;
	temp_entry |= read_write << 1;
	temp_entry |= privledge << 2;
	temp_entry |= write_through << 3;
	temp_entry |= cache_disabled << 4;
	temp_entry |= page_size << 7;
	*entry = (page_dir_entry_t) temp_entry;
}

static void set_page_tbl_entry(page_tbl_entry_t *entry, uint32_t phys_addr, 
			int read_write, int privledge, int write_through, 
			int cache_disabled)
{
	uint32_t temp_entry = 0;
	uint32_t temp_addr = ALIGN_UP(phys_addr, PAGE_SIZE);
	
	temp_entry |= (phys_addr & ~0x03FF);
	temp_entry |= 1;
	temp_entry |= read_write << 1;
	temp_entry |= privledge << 2;
	temp_entry |= write_through << 3;
	temp_entry |= cache_disabled << 4;
	*entry = (page_tbl_entry_t) temp_entry;
}

void map_kern_page(uint32_t virtual_addr, uint32_t phys_addr)
{
	unsigned long pdindex = virtual_addr >> 22;
	unsigned long ptindex = virtual_addr >> 12 & 0x03FF;

	spin_lock(&lock);
	
	page_dir_t * dir = kernel_dir;

	if (dir->page_dir[pdindex] == 0) {
		/* Map new page table */
		unsigned long temp_phys_addr = 0;
		temp_phys_addr = early_kmalloc_pages(1);
		memset((uint32_t *)temp_phys_addr, 0, PAGE_SIZE);
		set_page_dir_entry((page_dir_entry_t*)&dir->page_dir[pdindex],
				   temp_phys_addr, 1, 0, 0, 0, 0);
	}
	
	page_tbl_t *tbl = (page_tbl_t *) GET_TBL_PHYS_ADDR(dir->page_dir[pdindex]);
	page_tbl_entry_t* tbl_entry= (page_tbl_entry_t *) &tbl->page_tbl[ptindex];
	
	set_page_tbl_entry(tbl_entry, phys_addr, 1, 0, 0, 0);

	spin_unlock(&lock);
}

void map_kern_page_flags(uint32_t virtual_addr, uint32_t phys_addr, 
					     flags_t flags)
{
	unsigned long pdindex = virtual_addr >> 22;
	unsigned long ptindex = virtual_addr >> 12 & 0x03FF;
	kprint(INFO, "MAPPING KERN PAGE %x -> %x\n", phys_addr, virtual_addr);
	spin_lock(&lock);

	page_dir_t * dir = kernel_dir;

	if (dir->page_dir[pdindex] == 0) {
		kprint(INFO, "new page table is being set\n");
		/* Map new page table */
		unsigned long temp_phys_addr = 0;
		temp_phys_addr = early_kmalloc_pages(1);
		memset((uint32_t *)temp_phys_addr, 0, PAGE_SIZE);
		set_page_dir_entry((page_dir_entry_t*)&dir->page_dir[pdindex],
				   temp_phys_addr, 1, 0, 0, 0, 0);
	}
	
	page_tbl_t *tbl = (page_tbl_t *) GET_TBL_PHYS_ADDR(dir->page_dir[pdindex]);
	page_tbl_entry_t* tbl_entry= (page_tbl_entry_t *) &tbl->page_tbl[ptindex];
	
	int read_write = (flags & PAGE_READWRITE) >> PAGE_READWRITE_BIT;
	int privledge = (flags & PAGE_ALLACCESS) >> PAGE_ALLACCESS_BIT;
	int write_through = (flags & PAGE_WRITETHROUGH) >> PAGE_WRITETHROUGH_BIT;
	int cache_disabled = (flags & PAGE_CACHEDISABLE) >> PAGE_CACHEDISABLE_BIT;
	set_page_tbl_entry(tbl_entry, phys_addr, read_write, 
					   privledge, write_through, cache_disabled);

	spin_unlock(&lock);
}

page_tbl_entry_t * get_tbl_entry(uint32_t virt_addr)
{
	spin_lock(&lock);

	unsigned long pdindex = virt_addr >> 22;
	unsigned long ptindex = virt_addr >> 12 & 0x03FF;
	page_dir_t * dir = kernel_dir;
	
	page_tbl_t *tbl = (page_tbl_t *) GET_TBL_PHYS_ADDR(dir->page_dir[pdindex]);
	if (tbl == NULL) {
		spin_unlock(&lock);
		return NULL;
	}

	page_tbl_entry_t* tbl_entry= (page_tbl_entry_t *) &tbl->page_tbl[ptindex];

	spin_unlock(&lock);
	return tbl_entry;
}

void print_page_tbl_entry(uint32_t virt_addr)
{
	unsigned long pdindex = virt_addr >> 22;
	unsigned long ptindex = virt_addr >> 12 & 0x03FF;
	
	spin_lock(&lock);

	page_dir_t * dir = kernel_dir;
	page_tbl_t *tbl = (page_tbl_t *) GET_TBL_PHYS_ADDR(dir->page_dir[pdindex]);
	
	if (tbl == NULL) {
		spin_unlock(&lock);
		kprint(INFO, "***Virt addr: %x does not have an entry\n", virt_addr);
		return;
	}
	
	page_tbl_entry_t* tbl_entry= (page_tbl_entry_t *) &tbl->page_tbl[ptindex];
	uint32_t phys_addr = *tbl_entry & ~0x03FF;

	spin_unlock(&lock);
	kprint(INFO, "Virt addr %x -> %x\n", virt_addr, phys_addr);
}

void set_kern_dir()
{
	/* Asm function to load the new page table */
	paging_init(kernel_dir);
}

void init_kern_paging()
{
	unsigned long virt_addr_count = 0;
	uint32_t phys_addr_count = 0;
	uint32_t page_dir_addr_phys = 0;

	/* Allocate main kern directory */
	page_dir_addr_phys = early_kmalloc_pages(1);
	kernel_dir = (page_dir_t *)page_dir_addr_phys;
	memset(kernel_dir, 0, PAGE_SIZE);
	
	//map_kern_page(page_dir_addr_phys, page_dir_addr_phys);
	/* Identity map the first 1MB 
	 * PHYSICAL_ADDR = VIRTUAL_ADDR */
	while (phys_addr_count < MB(2)) {
		map_kern_page(phys_addr_count, phys_addr_count);		
		phys_addr_count += PAGE_SIZE;
	}
	
	virt_addr_count = KERN_VIRTUAL_ADDR;
	phys_addr_count = 0;
	/* Map the from 1MB - 4MB -> 3GB + 1MB - 3GB + 4MB */
	int count = 0;
	while (phys_addr_count < MB(6)) {
		map_kern_page(virt_addr_count, phys_addr_count);
		virt_addr_count += PAGE_SIZE;
		phys_addr_count += PAGE_SIZE;
		count ++;
	}
	

	kprint(INFO, "---Paging initialized\n");
	kprint(INFO, "--Top of mapped mem %x\n", virt_addr_count);
	kprint(INFO, "--Page count %d\n", count);
	kprint(INFO, "Kern dir at %x\n", kernel_dir);
	set_kern_dir();
}
