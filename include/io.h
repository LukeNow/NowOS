#ifndef _IO_H
#define _IO_H

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
void debug_print (unsigned char data);
void div_zero();
#endif
