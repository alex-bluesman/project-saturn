#include <system>

#include "console.hpp"
#include "heap.hpp"
#include "uart_pl011.hpp"

using namespace saturn;

// Saturn stack definition
unsigned int boot_stack[_stack_size] __align(_page_size);

namespace saturn {
namespace core {


extern Heap SaturnHeap;

static void Main(void)
{
	device::UartPl011 Uart;
	Uart.Init();

	Console Log(Uart);
	Log << "Console test: " << 1234 << " = 0x" << fmt::hex << fmt::fill << 1234 << fmt::endl;

	Heap Saturn_Heap;


	void* base = Saturn_Heap.Alloc(10);
	Saturn_Heap.Alloc(20);
	Saturn_Heap.Alloc(30);
	Saturn_Heap.Alloc(60);
	Saturn_Heap.State(Log);


	Log << "xxx" << fmt::endl;

	Saturn_Heap.Free(base);
	Saturn_Heap.State(Log);

	Log << fmt::endl << "<core initialization complete>" << fmt::endl;
	

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	saturn::core::Main();
}
