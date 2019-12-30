ARCH:=i686-elf
CFLAGS:=-g -ffreestanding -Wall -Wextra -lgcc -nostdlib -m32 -Wl,--build-id=none -Iinclude
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

KERN_OBJS:=  $(patsubst %.c,%.o,$(wildcard kernel/*.c))

BOOT_OBJS:= boot/boot.o 
		
ARCH_OBJS:= 	$(patsubst %.s,%.o,$(wildcard arch/*.s))
ARCH_OBJS+= 	$(patsubst %.c,%.o,$(wildcard arch/*.c))

KLIB_OBJS:=	$(patsubst %.c,%.o,$(wildcard klib/*.c))

TEST_OBJS:= 	$(patsubst %.c,%.o,$(wildcard kernel/tests/*.c))

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

