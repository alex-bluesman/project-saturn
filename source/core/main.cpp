#include <system>
#include <console>

#include "uart_pl011.hpp"

// Saturn stack definition
unsigned int boot_stack[_stack_size] __align(_page_size);

namespace saturn {
namespace core {

// External functions
extern "C" 
{
	void console_init(void);
	void console_msg(const char *fmt, ...);
}

static void Main(void)
{
	device::UartPl011 Uart;
	Uart.Init();

	Console c(Uart);
	c << "Console test: " << 1234 << " = 0x" << fmt::hex << fmt::fill << 1234 << fmt::endl;

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	saturn::core::Main();
}
