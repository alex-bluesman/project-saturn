#include <system>

#include "console.hpp"
#include "heap.hpp"
#include "uart_pl011.hpp"

using namespace saturn;

// Saturn stack definition
unsigned int boot_stack[_stack_size] __align(_page_size);

namespace saturn {
namespace core {

static Heap* Saturn_Heap;

static void Main(void)
{
	Heap Main_Heap;
	Saturn_Heap = &Main_Heap;

	device::UartPl011 Uart = *new device::UartPl011;
	Uart.Init();

	Console& Log = *new Console(Uart);

	Log << "<core initialization complete>" << fmt::endl;

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	saturn::core::Main();
}

void* operator new(size_t size) noexcept
{
	return saturn::core::Saturn_Heap->Alloc(size);
}

void operator delete(void* base, size_t size) noexcept
{
	return saturn::core::Saturn_Heap->Free(base);
}
