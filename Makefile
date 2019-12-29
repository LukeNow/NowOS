ARCH:=i686-elf
CFLAGS:=-g -ffreestanding -Wall -Wextra -lgcc -nostdlib -m32 -Wl,--build-id=none
ASFLAGS:=
DEBUGFLAGS:= -D DEBUG
TESTFLAGS:= -D TEST

CC:="$(ARCH)-gcc"
ASM:=nasm
LIBS:=-nostdlib -lk -lgcc

KERNDIR:=kernel
BOOTDIR:=boot
INCLUDEDIR:=include
KERNELDIR:=boot/iso/boot
KERNELNAME:=nowos
INCLUDE_DIR:= include
TEST_DIR := kernel/tests
SYS_DIR:= sysroot/usr

ifdef DEBUG
	CFLAGS := $(CFLAGS) $(DEBUGFLAGS)
endif

ifdef TEST
	CLFAGS := $(CFLAGS) $(TESTFLAGS)
endif

SYS_ROOT:="$(PWD)/sysroot"

KERN_OBJS:=  kernel/tty.o \
	       kernel/kmain.o \
	       kernel/kprint.o \
	       kernel/kheap.o \
	       kernel/paging.o \
	       kernel/mm.o \
	       kernel/scheduler.o \
	       kernel/task.o \
	       kernel/timer.o \
	       kernel/process.o \
	       kernel/ipc.o \
	       kernel/ktest.o \
	       kernel/system.o

BOOT_OBJS:= boot/boot.o \
		
ARCH_OBJS:= 	arch/gdt_init.o \
		arch/idt_init.o \
		arch/gdt.o \
		arch/irq_handle.o \
		arch/irq.o \
		arch/idt.o \
		arch/pic.o \
		arch/processor.o \
		arch/machine.o \
		arch/paging_init.o \
		arch/pit.o \
		arch/task.o \
		arch/io.o

KLIB_OBJS:=	klib/string.o \
		klib/sorted_array_list.o \
		klib/linked_list.o \
		klib/byte_index_list.o \
		klib/circ_buf.o

TEST_OBJS:= $(TEST_DIR)/run_tests.o \
		$(TEST_DIR)/test_circ_buf.o \
		$(TEST_DIR)/test_linked_list.o \
		$(TEST_DIR)/test_kheap.o

OBJS:= $(BOOT_OBJS) $(KLIB_OBJS) $(KERN_OBJS) $(ARCH_OBJS) $(TEST_OBJS)

.PHONY: all clean run iso bochs-run qemu-run
.SUFFIXES: .o .c .s

all: clean nowos.kernel

install-headers:
	mkdir -p $(SYS_DIR)/$(INCLUDE_DIR)
	cp -R --preserve=timestamps include/. $(SYS_DIR)/$(INCLUDE_DIR)

nowos.kernel: install-headers $(OBJS) boot/linker.ld
	echo $(SYS_ROOT)
	$(CC) -T boot/linker.ld -o $(KERNELDIR)/$(KERNELNAME).kernel $(CFLAGS) $(OBJS) 
	sh scripts/check-multiboot.sh $(KERNELDIR) $(KERNELNAME)

qemu-run: all
	qemu-system-i386 -kernel $(KERNELDIR)/$(KERNELNAME).kernel -serial file:serial.log

bochs-run: iso1
	bochs -q 

iso1: all
	genisoimage -R                          \
                -b boot/grub/stage2_eltorito    \
                -no-emul-boot                   \
                -boot-load-size 4               \
                -A nowos                        \
                -input-charset utf8             \
                -quiet                          \
                -boot-info-table                \
                -o nowos.iso                    \
                boot/iso	

iso2: all
	grub-mkrescue -o nowos.iso boot/iso

clean:	
	rm -f bx_enh_dbg.ini
	rm -f nowos.iso
	rm -f $(KERNELDIR)/$(KERNELNAME).kernel
	rm -f $(OBJS) 
	rm -rf $(SYS_DIR)
	rm -rf sysroot
.c.o:
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS)

.s.o:
	$(ASM) -felf32 $< -o $@ $(ASMFLAGS)

