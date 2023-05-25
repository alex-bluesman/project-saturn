// Copyright (C) 2023 Alexander Smirnov <alex.bluesman.smirnov@gmail.com>
//
// Licensed under the MIT License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// http://opensource.org/licenses/MIT
//
// Unless required by applicable law or agreed to in writing, software distributed 
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
// CONDITIONS OF ANY KIND, either express or implied. See the License for the 
// specific language governing permissions and limitations under the License.

#include "console.hpp"

namespace saturn {
namespace core {

using namespace device;

static const size_t _max_message_size = 128;

// Local CPU console buffer to cache the data if UART driver is not active.
// This is very important for early console messages.
static uint8_t _consoleBuffer[_buffer_size];

Console::Console()
	: isActive(false)
	, isHex(false)
	, isFill(false)
	, isLevel(false)
	, uart(nullptr)
	, currentMsgLevel(llevel::info)
	, consoleLevel(llevel::info)
{
	consoleBuffer = new RingBuffer<uint8_t, _buffer_size>(rb::full_overwrite, _consoleBuffer);
	rxBuffer = new RingBuffer<char, _rx_size>(rb::full_ignore);

	*this << fmt::endl << "<console enabled>" << fmt::endl << fmt::endl;
}

void Console::RegisterUart(IUartDevice& u)
{
	uint8_t c;

	uart = &u;

	while (consoleBuffer->Out(c))
	{
		uart->Tx(reinterpret_cast<uint8_t *>(&c), 1);
	}

	isActive = true;
}

bool Console::UartRX(char sym)
{
	return rxBuffer->In(sym);
}

char Console::GetChar(iomode mode)
{
	char c;

	if (mode == iomode::sync)
	{
		while (rxBuffer->Out(c) == false)
		{
			asm volatile("wfi" : : : "memory");
		}
	}
	else
	{
		if (rxBuffer->Out(c) == false)
		{
			c = 0;
		}
	}

	return c;
}

Console& Console::operator<<(char c)
{
	if (isActive)
	{
		uart->Tx(reinterpret_cast<uint8_t *>(&c), 1);
	}
	else
	{
		consoleBuffer->In(static_cast<uint8_t>(c));
	}

	return *this;
}

Console& Console::operator<<(char const *msg)
{
	uint8_t buf[_max_message_size];
	size_t len = 0;
	char c;

	if (currentMsgLevel >= consoleLevel)
	{
		// We need this to handle line breaks in the middle of message, for example:
		//   Log() << "Line break" << fmt::endl << "in the middle of message" << fmt::endl;
		ShowLogLevel();

		while ((c = *msg++) != 0)
		{
			if (c == '\\')
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
				case '\\':
					break;
				default:
					buf[len++] = '\\';
					break;
				}
			}

			buf[len++] = c;
		}

		if (isActive)
		{
			uart->Tx(buf, len);
		}
		else
		{
			size_t i = 0;
			while (i < len)
			{
				consoleBuffer->In(buf[i]);
				i++;
			}
		}
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

void Console::ShowLogLevel()
{
	if (false == isLevel)
	{
		isLevel = true;

		switch (currentMsgLevel)
		{
		case llevel::log:
			*this << "[log] ";
			break;
		case llevel::info:
			*this << "[inf] ";
			break;
		case llevel::error:
			*this << "[err] ";
			break;
		default:
			break;
		}
	}
}

void Console::SetLevel(llevel level)
{
	consoleLevel = level;
}

Console& Console::operator<<(fmt format)
{
	switch (format)
	{
	case fmt::endl:
		*this << "\r\n";
		isHex = false;
		isFill = false;
		isLevel = false;
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

Console& Console::operator<<(llevel level)
{
	currentMsgLevel = level;
	ShowLogLevel();

	return *this;
}

}; // namespace core
}; // namespace saturn
