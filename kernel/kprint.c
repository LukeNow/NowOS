#include <kdef.h>
#include <stdarg.h>
#include <tty.h>
#include <ctype.h>
#include <kprint.h>
#include <irq_handle.h>
#include <lock.h>

int log_count = 0;

SPINLOCK_INIT(lock);

static char *convert(unsigned int num, int base) 
{
	static char representation[] = "0123456789ABCDEF";
	static char buff[50];
	char *ptr;

	ptr = &buff[49];
	*ptr = '\0';

	do {
		*--ptr = representation[num % base];
		num /= base;
	} while(num != 0);
	
		
	return ptr;
}


void klog(LOG_LEVEL level, char* format, ...)
{
	char *charp;
	int i;
	char *s;
	
	va_list arg;
	va_start(arg, format);
	
	int arg_num = 0;

	for(charp = format; *charp != '\0'; charp++) {
		switch(*charp) {
			case 'c': i = va_arg(arg, int);
					
					break;

			case 'd': i = va_arg(arg,int);
					break;

			case 's': s = va_arg(arg, char *);
					break;

			case 'x': i = va_arg(arg, unsigned int);
					break;
		}
	}
	va_end(arg);
}

void kprint(LOG_LEVEL level, char* format, ...)
{
	char *charp;
	int i;
	char *s;
	
	//spin_lock(&lock);

	va_list arg;
	va_start(arg, format);
	
	/* Set color for log level */
	switch(level) {
		case INFO: 
			term_setcolor(VGA_COLOR_WHITE);
			break;

		case WARN:
			term_setcolor(VGA_COLOR_LIGHT_RED);
			break;

		case ERROR:
			term_setcolor(VGA_COLOR_RED);
			break;
	}
	
	term_writestring("[");
	term_writestring(convert(log_count, 10));
	term_writestring("] ");
	log_count++;

	for(charp = format; *charp != '\0'; charp++) {
		if (*charp != '%')
			term_putchar(*charp);
		else {
			charp++; //get the next identifer
			switch(*charp) {
				case 'c': i = va_arg(arg, int);
						term_putchar(toascii(i));
						break;

				case 'd': i = va_arg(arg,int);
						if (i < 0) {
							i = -1;
							term_putchar('-');
						}
						term_writestring(convert(i, 10));
						break;

				case 's': s = va_arg(arg, char *);
						term_writestring(s);
						break;

				case 'x': i = va_arg(arg, unsigned int);
						term_writestring("0x");
						term_writestring(convert(i, 16));
						break;
				/*case 'b': i = va_arg(arg, unsigned int);
						term_writestring("B");
						term_writestring(convert(i, 2));
						break;*/
			}

		}

	}
	va_end(arg);

	//spin_unlock(&lock);
}
