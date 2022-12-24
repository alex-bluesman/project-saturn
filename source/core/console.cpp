#include <console>

namespace saturn {
namespace core {

using namespace device;

static const size_t max_message_size = 128;

Console::Console(UartDevice& u)
	: isActive(0)
	, uart(u)
{
	isActive = true;
	*this << fmt::endl << "<console enabled>" << fmt::endl << fmt::endl;
}

//void * Console::operator new(long unsigned int size)
//{
//}

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

Console& Console::operator<<(fmt format)
{
	switch (format)
	{
	case fmt::endl:
		*this << "\r\n";
		break;
	default:
		break;	// TBD: add unsupported formatting options
	}


	return *this;
}

}; // namespace core
}; // namespace saturn
