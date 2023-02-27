#pragma once

#include <dev/uart>
#include <iconsole>

namespace saturn {
namespace core {

using namespace device;

class Console : public IConsole
{
//public:
//	void * operator new(long unsigned int size);
//	void operator delete(void * p, unsigned long){};

public:
	Console();
	Console(UartDevice&);

public:
	Console& operator<<(fmt format);
	Console& operator<<(char const *msg);

	Console& operator<<(int32_t num);
	Console& operator<<(uint32_t num);
	Console& operator<<(int64_t num);
	Console& operator<<(uint64_t num);

	Console& operator<<(size_t num);

public:
	void RegisterUart(UartDevice&);

private:
	Console& SignedToStr(int64_t num, uint8_t fillSize = 0);
	Console& UnsignedToStr(uint64_t num, uint8_t fillSize = 0);

private:
	bool isActive;
	bool isHex;
	bool isFill;
	UartDevice* uart;
};

}; // namespace core
}; // namespace saturn
