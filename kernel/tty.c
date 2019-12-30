#include <kdef.h>
#include <tty.h>
#include <io.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM 0xC00B8000

#define TERM_COMMAND_PORT 0x3D4
#define TERM_DATA_PORT 0x3D5
#define TERM_HIGH_BYTE_COMMAND 14
#define TERM_LOW_BYTE_COMMAND 15

static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;


static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

void term_init()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) VGA_MEM;

	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void term_move_cursor(unsigned short pos)
{
	outb(TERM_COMMAND_PORT, TERM_HIGH_BYTE_COMMAND);
	outb(TERM_DATA_PORT, ((pos >> 8) & 0x00FF));
	outb(TERM_COMMAND_PORT, TERM_LOW_BYTE_COMMAND);
	outb(TERM_DATA_PORT, pos & 0x00FF);
}

void term_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void term_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
	term_move_cursor(index);
}

static void move_entries_up() {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {

			const size_t from_index = (y + 1) * VGA_WIDTH + x;
			const size_t to_index = (y) * VGA_WIDTH + x;
			
			if (y == VGA_HEIGHT - 1) {
				const size_t cursor_pos = (y) * VGA_WIDTH;
				terminal_buffer[to_index] = vga_entry(' ', terminal_color);
				term_move_cursor(cursor_pos);
			}
			else 
				terminal_buffer[to_index] = terminal_buffer[from_index];
				
		
		}
	}



}

void term_putchar(char c) {
	
	switch(c) {
		case '\n': {
				terminal_column = 0;
				if (++terminal_row == VGA_HEIGHT) {
					terminal_row = VGA_HEIGHT - 1;
					move_entries_up();
				}

				break;
			   }
		
		default: {
				
				if (++terminal_column == VGA_WIDTH) {
					terminal_column = 0;
					if (++terminal_row == VGA_HEIGHT) {
						terminal_row = VGA_HEIGHT - 1;
						move_entries_up();
					}
						
				}
				
				term_putentryat(c, terminal_color, terminal_column, terminal_row);

			 }

	}


	debug_print(c); //Write character to serial log

}
 
void term_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		term_putchar(data[i]);
}
 
void term_writestring(const char* data) {
	for(size_t i = 0; data[i]; i++) {
		term_putchar(data[i]);
	}
}



