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
	//console_init();
	//console_msg("\r\n > console enabled\r\n");

	device::UartPl011 Uart;

	Uart.Init();
//	console_init();
//	console_msg("\r\n > console enabled\r\n");

	Console c(Uart);
	c << "Hey!";

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	saturn::core::Main();
}
