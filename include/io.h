#ifndef _IO_H
#define _IO_H

#include <kdef.h>
// TODO Phase these out
void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);
void debug_print (unsigned char data);

void io_write8(uint32_t port, uint8_t data);
void io_write16(uint32_t port, uint16_t data);
void io_write32(uint32_t port, uint32_t data);

uint8_t io_read8(uint32_t port);
uint16_t io_read16(uint32_t port);
uint32_t io_read32(uint32_t port);

void volatile_write8(void * dest, uint8_t data);
void volatile_write16(void * dest, uint16_t data);
void volatile_write32(void * dest, uint32_t data);

uint8_t volatile_read8(void * ptr);
uint16_t volatile_read16(void * ptr);
uint32_t volatile_read32(void * ptr);

void volatile_read_n(void *dst, const volatile void *src, size_t bytes);
#endif
