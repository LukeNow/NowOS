# NowOS - The OS you want now, but probably will never get
My take on an operating system written in C targeted towards x86 compatible machines. 

My goal is to create a functioning kernel and eventually be able to support user-space. I am much more interested in the kernel-level side of things so I might never get around to implementing a full desktop environment but who knows! 

This project is for educational purposes and will probably not be in any useful state. But every aspiring kernel developer has to take a crack at writing their own operating system at least once in their lives, right?

# Log
1) First thing that we are doing is coming out of the boot environment provided by grub and settin things up for
our actual kernel
      
      A) We need to set up an actual environment that GRUB has temporarily provided us. We need to load descriptor tabels
      and set up paging. 
      
      B) Move our kernel to higher-address spaces. 
      
      C) Get better terminal support and logging. We will need to be debugging a lot. 

# Info
If anyone happens to stumble upon this code and has any recommendations/tips or comments please feel free to email me at lnowakow@eng.ucsd.edu, I would be glad to talk about it! 
