#include <console.h>

#define PRINT_BUFF_SIZE		80

static size_t format_console_message(char *buff, const char *fmt, va_list argp)
{
	size_t len = 0;
	char hex[] = "0123456789abcdef";

	char c;

	while ((c = *fmt++) != 0)
	{

		if (c == '%')
		{
			char *s = 0;
			size_t fill = 0;
			uint32_t base = 10;
			uint8_t negative = 0;

			int d;
			unsigned long u;

			c = *fmt++;

			while ((c >= '0') && (c <= '9'))
			{
				fill = fill * 10 + (size_t)(c - '0');
				c = *fmt++;
			}

			switch(c)
			{
			case 'd':
				d = va_arg(argp, signed int);

				negative = (d < 0);
				u = (d < 0) ? (unsigned long)(-d) : (unsigned long)d;
				base = 10;

				break;
			case 'u':
			case 'x':
				u = va_arg(argp, unsigned long);
				base = (c == 'u') ? 10 : 0x10;

				break;
			case 's':
				s = va_arg(argp, char *);

				if (s == 0)
				{
					s = "null";
				}

				break;
			case '%':
				s = "%";

				break;
			case 'c':
				s = "?";
				s[0] = va_arg(argp, int);

				break;
			default:
				s = "%?";
				s[1] = c;

				break;
			}

			if (s == 0)
			{
				/* Max unsigned 64 bit
				 *  - in dec: 1 777 777 777 777 777 777 777
				 *  - in hex: ffff ffff ffff ffff
				 * So the maximal literal length is 22
				 */
				char str[22];
				size_t i = 0;

				s = str + sizeof(str);
				*--s = 0;

				do
				{
					if (fill && (i++ >= fill))
					{
						break;
					}

					*--s = hex[u % base];
					u /= base;
				}
				while (u > 0);

				while (i++ < fill)
				{
					*--s = '0';
				}

				if (negative)
				{
					*--s = '-';
				}
			}

			while (*s != 0)
			{
				buff[len++] = *s++;
			}
		}
		else
		{
			if (c == '/')
			{
				c = *fmt++;
				switch(c)
				{
				case 'n':
					c = 0x0a;
					break;
				case 'r':
					c = 0x0d;
					break;
				case 'b':
					c = 0x08;
					break;
				default:
					buff[len++] = '/';
					break;
				}
			}

			buff[len++] = c;
		}
	}

	return len;
}

void console_msg(const char *fmt, ...)
{
	va_list args;
	unsigned int len;
	char print_buffer[PRINT_BUFF_SIZE];

	va_start(args, fmt);
	len = format_console_message(print_buffer, fmt, args);
	va_end(args);

	print_buffer[len] = 0;
	uart_tx((uint8_t *)print_buffer, len);
}

void console_init(void)
{
	uart_init();
}
