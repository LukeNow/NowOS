# NowOS - The OS you want now, but probably will never get
My take on an operating system targeted towards x86 compatible machines. 

My goal is to create a functioning kernel and eventually be able to support user-space. I am much more interested in the kernel-level side of things so I might never get around to implementing a full desktop environment but who knows! 

This project is for educational purposes and will most likely take years to complete and will probably not be in any useful state. But every aspiring kernel developer has to take a crack at writing their own operating system at least once in their lives, right?

# Steps to take

1) Set up the boot environment after GRUB gives us control to pass over to my kernel. 
  
    A) Set up paging and virtual memory, basically we need to set up the memory basics
  
    B) Go from real to protected mode
  
    C) Go into the kernel

2) Terminal
  
    A) We need to get debugging utils early, going to do a lot of debugging...

3) Interrupt handling

4) Memory managment
  
    A) Implementing our virtual memory

5) Scheduling
  
    A) Multi-processing 
  
6) Device Drivers

7) Userland and beyond

# Info
If anyone happens to stumble upon this code and has any recommendations/tips or comments please feel free to email me at lnowakow@eng.ucsd.edu, I would be glad to talk about it! 
