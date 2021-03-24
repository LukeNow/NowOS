#include <io.h>
inline void io_write8(uint32_t port, uint8_t data)
{
    __asm__ volatile("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

inline void io_write16(uint32_t port, uint16_t data)
{
    __asm__ volatile("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

inline void io_write32(uint32_t port, uint32_t data)
{
    __asm__ volatile("outl %0, %w1" : : "a" (data), "Nd" (port));
}

inline uint8_t io_read8(uint32_t port)
{
    uint8_t data;
    __asm__ volatile("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

inline uint16_t io_read16(uint32_t port)
{
    uint8_t data;
    __asm__ volatile("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}

inline uint32_t io_read32(uint32_t port)
{
    uint32_t data;
    __asm__ volatile("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

inline void volatile_write8(void * dest, uint8_t data)
{
    *(volatile uint8_t *)(dest) = data;
}

inline void volatile_write16(void * dest, uint16_t data)
{
    *(volatile uint16_t *)(dest) = data;
}

inline void volatile_write32(void * dest, uint32_t data)
{
    *(volatile uint32_t *)(dest) = data;
}

inline uint8_t volatile_read8(void * ptr)
{
    return *(volatile uint8_t *)(ptr);
}

inline uint16_t volatile_read16(void * ptr)
{
    return *(volatile uint16_t *)(ptr);
}

inline uint32_t volatile_read32(void * ptr)
{
    return *(volatile uint32_t *)(ptr);
}

inline void volatile_read_n(void *dst, const volatile void *src, size_t bytes)
{
    volatile uint8_t *s = (volatile uint8_t *)src;
    uint8_t *d = (uint8_t *)dst;
    while (bytes > 0)
    {
        *d =  *s;
        ++s;
        ++d;
        --bytes;
    }
}