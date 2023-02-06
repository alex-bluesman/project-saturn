#include <system>

#include "console.hpp"
#include "heap.hpp"
#include "mmu.hpp"
#include "uart_pl011.hpp"

using namespace saturn;

// Saturn stack definition
unsigned int boot_stack[_stack_size] __align(_page_size);

namespace saturn {
namespace core {

// Heap object pointer to implement operators new/delete
static Heap* Saturn_Heap;

// MMU object pointer to implement I/O mapping
MemoryManagementUnit* Saturn_MMU;

// Saturn console pointer
static Console* Saturn_Console = nullptr;

// External API:
void Exceptions_Init();

static void Main(void)
{
	// The Main function will never quit, so all the allocated on stack objects
	// will be available whole Saturn life cycle

	Heap Main_Heap;
	Saturn_Heap = &Main_Heap;

	Saturn_MMU = new MemoryManagementUnit();

	device::UartPl011& Uart = *new device::UartPl011();
	Saturn_Console = new Console(Uart);

	Exceptions_Init();

	Log() << "<core initialization complete>" << fmt::endl;

	for (;;);
}

IConsole& Log()
{
	return *Saturn_Console;
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
