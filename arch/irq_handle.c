#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/io.h"
#include "../include/pic.h"
#include "../include/kprint.h"
#include "../include/machine.h"
#include "../include/processor.h"

void divide_by_zero_handler(void)
{
	interrupt_register_dump(0);
	halt_system();
}


/*PIC HANDLERS*/

void irq0_handler(void)
{
	//kprint(INFO, "IRQ 0 FIRED\n");
	pic_sendEOI(0);
}

void irq1_handler(void)
{
	
	kprint(INFO, "IRQ 1 FIRED\n");
	pic_sendEOI(1);
}

 
void irq2_handler(void) 
{
	kprint(INFO, "IRQ 2 FIRED\n");
	pic_sendEOI(2);
}
 
void irq3_handler(void) 
{
	kprint(INFO, "IRQ 3 FIRED\n");
	pic_sendEOI(3);
}
 
void irq4_handler(void) 
{
	kprint(INFO, "IRQ 4 FIRED\n");
	pic_sendEOI(4);
}
 
void irq5_handler(void) 
{
	kprint(INFO, "IRQ 5 FIRED\n");
	pic_sendEOI(5);
}
 
void irq6_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq7_handler(void) {
          outb(0x20, 0x20); //EOI
}
 
void irq8_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI          
}
 
void irq9_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq10_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq11_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq12_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq13_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq14_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
 
void irq15_handler(void) {
          outb(0xA0, 0x20);
          outb(0x20, 0x20); //EOI
}
