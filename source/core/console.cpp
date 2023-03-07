#include "console.hpp"

namespace saturn {
namespace core {

using namespace device;

static const size_t max_message_size = 128;

Console::Console()
	: isActive(false)
	, isHex(false)
	, isFill(false)
	, uart(nullptr)
{
	*this << fmt::endl << "<console enabled>" << fmt::endl << fmt::endl;
}

Console::Console(UartDevice& u)
	: isActive(false)
	, isHex(false)
	, isFill(false)
	, uart(&u)
{
	isActive = true;
	*this << fmt::endl << "<console enabled>" << fmt::endl << fmt::endl;
}


void Console::RegisterUart(UartDevice& u)
{
	uart = &u;
	isActive = true;

	// TBD: flush buffered output
}

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
			case '/':
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
		uart->Tx(buf, len);
	}

	return *this;
}

Console& Console::operator<<(int32_t num)
{
	uint8_t fillSize = isFill ? 8 : 0;
	return SignedToStr(num, fillSize);
}

Console& Console::operator<<(uint32_t num)
{
	uint8_t fillSize = isFill ? 8 : 0;
	return UnsignedToStr(num, fillSize);
}

Console& Console::operator<<(int64_t num)
{
	uint8_t fillSize = isFill ? 16 : 0;
	return SignedToStr(num, fillSize);
}

Console& Console::operator<<(uint64_t num)
{
	uint8_t fillSize = isFill ? 16 : 0;
	return UnsignedToStr(num, fillSize);
}

Console& Console::operator<<(size_t num)
{
	uint8_t fillSize = isFill ? 16 : 0;
	return UnsignedToStr(num, fillSize);
}

Console& Console::SignedToStr(int64_t num, uint8_t fillSize)
{
	if (num < 0)
	{
		if (isHex)
		{
			*this << "<invalid>";	// No need to support negative hex
		}
		else
		{
			*this << "-";
			UnsignedToStr(static_cast<uint64_t>(-num), fillSize);
		}
	}
	else
	{
		UnsignedToStr(static_cast<uint64_t>(num), fillSize);
	}

	return *this;
}

Console& Console::UnsignedToStr(uint64_t num, uint8_t fillSize)
{

	// Max unsigned 64 bit
	//  - in dec: 1 777 777 777 777 777 777 777
	//  - in hex: ffff ffff ffff ffff
	// So the maximal literal length is 22
	char numStr[22];

	// Symbol table
	char hexTable[] = "0123456789abcdef";

	// Formating flags
	uint8_t divBase = isHex ? 16 : 10;

	char *s = numStr + sizeof(numStr);
	*--s = 0;

	size_t i = 0;
	do
	{
		*--s = hexTable[num % divBase];
		num /= divBase;
		i++;
	}
	while (num > 0);


	if (isHex)
	{
		while (i++ < fillSize)
		{
			*--s = '0';
		}
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
