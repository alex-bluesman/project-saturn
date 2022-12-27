#include "console.hpp"

namespace saturn {
namespace core {

using namespace device;

static const size_t max_message_size = 128;

Console::Console(UartDevice& u)
	: isActive(false)
	, isHex(false)
	, isFill(false)
	, uart(u)
{
	isActive = true;
	*this << fmt::endl << "<console enabled>" << fmt::endl << fmt::endl;
}

//void * Console::operator new(long unsigned int size)
//{}

Console& Console::operator<<(char const *msg)
{
	uint8_t buf[max_message_size];
	size_t len = 0;
	char c;

	while ((c = *msg++) != 0)
	{
		if (c == '/')
		{
			c = *msg++;
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
				buf[len++] = '/';
				break;
			}
		}

		buf[len++] = c;
	}

	if (isActive)
	{
		uart.Tx(buf, len);
	}

	return *this;
}

Console& Console::operator<<(int num)
{
	if (num < 0)
	{
		if (isHex)
		{
			*this << "<invalid>";
		}
		else
		{
			*this << "-" << static_cast<unsigned long>(-num);
		}
	}
	else
	{
		*this << static_cast<unsigned long>(num);
	}

	return *this;
}

Console& Console::operator<<(unsigned long num)
{

	// Max unsigned 64 bit
	//  - in dec: 1 777 777 777 777 777 777 777
	//  - in hex: ffff ffff ffff ffff
	// So the maximal literal length is 22
	char str[22];

	// Symbol table
	char hex[] = "0123456789abcdef";

	// Formating flags
	uint8_t base = isHex ? 16 : 10;
	uint8_t fill = isFill ? 8 : 0;

	char *s = str + sizeof(str);
	*--s = 0;

	size_t i = 0;
	do
	{
		*--s = hex[num % base];
		num /= base;
		i++;
	}
	while (num > 0);


	while (i++ < fill)
	{
		*--s = '0';
	}

	return *this << s;
}

Console& Console::operator<<(fmt format)
{
	switch (format)
	{
	case fmt::endl:
		*this << "\r\n";
		isHex = false;
		isFill = false;
		break;
	case fmt::dec:
		isHex = false;
		break;
	case fmt::hex:
		isHex = true;
		break;
	case fmt::fill:
		isFill = true;
		break;
	case fmt::nofill:
		isFill = false;
		break;
	default:
		break;
	}

	return *this;
}

}; // namespace core
}; // namespace saturn
