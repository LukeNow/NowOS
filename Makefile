ARCH:=i686-elf
CFLAGS:=-02 -g -ffreestanding -Wall -Wextra
LIBS:=-nostdlib -lk -lgcc

INCLUDEDIR:=include/
ARCHDIR:=arch/


KERNEL_OBJS:=kernel.o \

LINK_LIST=\
     $(ARCHDIR)/crti.o \
     $(ARCHDIR)/crtbegin.o \
     $(KERNEL_OBJS) \
     $(LIBS) \
     $(ARCHDIR)/crtend.o \
     $(ARCHDIR)/crtn.o \

.PHONY: all clean 
.SUFFIXES: .o .c .S

all: nowos.kernel
nowos.kernel: $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot nowos.kernel

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS)

