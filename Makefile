ARCH:=i686-elf
CFLAGS:=-g -ffreestanding -Wall -Wextra -lgcc -nostdlib
ASFLAGS:=

CC:=$(ARCH)-gcc
ASM:=nasm

LIBS:=-nostdlib -lk -lgcc

KERNDIR:=kernel
BOOTDIR:=boot
INCLUDEDIR:=include


KERN_OBJS:= kernel/io.o  \
	       kernel/tty.o \
	       kernel/kernel.o \
	       kernel/kprint.o 

BOOT_OBJS:= boot/boot.o \
		
ARCH_OBJS:= arch/gdt.o \
		arch/idt.o \
		arch/irq_handle.o \
		arch/irq.o \
		arch/pic.o \
		arch/processor.o \
		arch/machine.o


OBJS:= $(BOOT_OBJS) $(KERN_OBJS) $(ARCH_OBJS)
	  



.PHONY: all clean run
.SUFFIXES: .o .c .s

all: clean nowos.kernel

nowos.kernel: $(OBJS) boot/linker.ld
	$(CC) -T boot/linker.ld -o $@ $(CFLAGS) $(OBJS)
	grub-file --is-x86-multiboot nowos.kernel

run: all
	qemu-system-i386 -kernel nowos.kernel -serial file:serial.log

clean:	
	rm -f nowos.kernel
	rm -f $(OBJS) 
.c.o:
	$(CC) -c $< -o $@ -std=gnu11 $(CFLAGS)

.s.o:
	$(ASM) -felf32 $< -o $@ $(ASMFLAGS)

