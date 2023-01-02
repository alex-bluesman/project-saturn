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
	Console(UartDevice&);

public:
	Console& operator<<(char const *msg);
	Console& operator<<(int num);
	Console& operator<<(unsigned long num);
	Console& operator<<(fmt format);

private:
	bool isActive;
	bool isHex;
	bool isFill;
	UartDevice& uart;
};

}; // namespace core
}; // namespace saturn
