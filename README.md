# NowOS
A simple operating system written in C targeted towards x86-32 compatible machines.

This project mainly aims to learn about building an OS from scratch (everything except the bootloader, thank you GRUB) and in general learning about operating system design and implementation. I am interested in developing an operating system using a micro-kernel architecture, as modular design is much easier to implement and debugging becomes much more feasible (which we will need it to be). 

The operating system at the moment features
* multi-queue round robin single-core multi-tasking scheduler
* memory managment capabilities (kfree, kalloc, page_kalloc)
* Basic terminal (it only prints out debugging info)
* Paging support (no drivers for disk so not very interesting in terms of swapping) 
* Pit support (tasks can sleep, etc.)
* Various useful libraries (strings (memcpy, memset, etc.), linked lists, array lists)

What it doesent feature (YET!) 
* Keyboard and mouse drivers
* Filesystem
* IPC (bread and butter of the micro-kernel architecture)
* Networking stack

There is a copy of the kernel binary and an iso included in the top level of the directory that you can use to run the OS. If you want to compile your own or make some changes, you will have to go through some extra steps as mentioned below. 
# Compiling


To compile the OS you will need to use a cross compiler, not the compiler that you use on your system. If you wish to compile from scratch, follow theses steps in the article to build a compiler (and to learn why you even need one): https://wiki.osdev.org/GCC_Cross-Compiler. 

Once the compiler is built, you can run 
 
    $ make
    
and a kernel binary named "nowos.kernel" should appear in boot/iso/boot directory. 

# Running

With either the freshly built or the prebuilt kernel binary, we can run it virtually with your favorite virtual machine. I will only talk specifically about qemu as it is the most popular one and is the one I used the most. 

to run it using qemu-system-i386 you can type

    $ qemu-system-i386 -kernel boot/iso/boot/nowos.kernel -serial file:serial.log
    
This will boot up the kernel binary and print out and terminal info to a file called "serial.log" that you can inspect after running the os. 

# Info
If anyone happens to stumble upon this code and has any recommendations/tips or comments please feel free to email me at lnowakow@ucsd.edu, I would be glad to talk about it! I love talking about operating systems and would love to hear what you have to say about mine :)
