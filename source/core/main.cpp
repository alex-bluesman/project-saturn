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
void Interrupts_Init();
void Register_CPU();

static void Main(void)
{
	// The Main function will never quit, so all the allocated on stack objects
	// will be available whole Saturn life cycle

	Heap Main_Heap;
	Saturn_Heap = &Main_Heap;

	// Let's create console as soon as possible to be able to collect output from MMU.
	// It has no connection to UART, but it could buffer the output and flush it later.
	// Also let's keep heap initialization before, we could use it for buffering.
	Saturn_Console = new Console();

	Saturn_MMU = new MemoryManagementUnit();

	device::UartPl011& Uart = *new device::UartPl011();
	Saturn_Console->RegisterUart(Uart);

	Exceptions_Init();

	// Starting from now there is Fault Mode available, so it makes sense to do sanity checks
	// for memory allocations and return values from calls

	Register_CPU();
	Interrupts_Init();

	Log() << fmt::endl << "<core initialization complete>" << fmt::endl;

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
