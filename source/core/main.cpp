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

#include "main.hpp"
#include "uart_pl011.hpp"

#include <system>

// Saturn stack definition
saturn::uint64_t boot_stack[_stack_size] __align(_page_size);

// When switch to EL1 we don't need EL2 stack anymore except heap, which
// could be allocated only on stack. So the following marker could be used
// as SP to which we can reset the stack without damaging heap.
saturn::uint64_t el2_stack_reset = 0;

namespace saturn {

namespace apps {
	void Applications_Start();
};

namespace core {

// External API:
void Exceptions_Init();
void MMU_Init();
void Start_VM_Manager();

static void Main(void)
{
	// The Main function will never quit, so all the allocated on stack objects
	// will be available whole Saturn life cycle

	Heap Main_Heap;
	Saturn_Heap = &Main_Heap;

	// Set stack marker. Below this marker the stack could be reset during switch to EL1
	asm volatile("mov %0, sp" : "=r" (el2_stack_reset));

	// TBD: check that all allocations are successful

	// Let's create console as soon as possible to be able to collect output from MMU.
	// It has no connection to UART, but it could buffer the output and flush it later.
	// Also let's keep heap initialization before, we could use it for buffering.
	Saturn_Console = new Console();

	// Initialize hypervisor and guest MMUs
	MMU_Init();

	device::UartPl011& Uart = *new device::UartPl011();
	Saturn_Console->RegisterUart(Uart);

	// Set default log level
	Saturn_Console->SetLevel(llevel::info);

	Exceptions_Init();

	// Starting from now there is Fault Mode available, so it makes sense to do sanity checks
	// for memory allocations and return values from calls

	// Create CPU object
	Local_CPU = new CpuInfo();

	// Create interrupt controller object
	Saturn_IC = new IC_Core();

	Info() << fmt::endl << "<core initialization complete>" << fmt::endl;

	// Finally we are ready to receive interrupts
	IC().Local_IRq_Enable();

	// Now we are ready to receive data from console
	Uart.EnableRx();

	Start_VM_Manager();

	saturn::apps::Applications_Start();

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" void saturn_init()
{
	// Let's switch to C++ world!
	saturn::core::Main();
}
