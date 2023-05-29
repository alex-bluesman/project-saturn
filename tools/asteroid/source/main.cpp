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

#include <arm64/registers>
#include <io>

using namespace saturn;
using namespace saturn::core;

namespace asteroid {

// External API:
extern void Exceptions_Init();

static void Demo_Application(void)
{
	Info() << "* start demo application *" << fmt::endl;

	for (;;);
}

void Main()
{
	Heap Main_Heap;
	Asteroid_Heap = &Main_Heap;

	Asteroid_Console = new Console();

	Raw() << fmt::endl;
	Raw() << " = Asteroid Application 2023 =" << fmt::endl;
	Raw() << fmt::endl;

	uint64_t reg;
	reg = ReadArm64Reg(CurrentEL);
	Info() << "running in EL" << (reg >> 2) << fmt::endl;

	Exceptions_Init();
	Asteroid_IC = new IC_Core();

	device::UartPl011& Uart = *new device::UartPl011();
	Asteroid_Console->RegisterUart(Uart);

	iIC().Local_IRq_Enable();

	// Kernel initialization complete

	// Run demo application
	Demo_Application();

	for (;;);
}

}; // namespace asteroid

extern "C" {
	uint64_t boot_stack[_stack_size];

	void Asteroid_Init()
	{
		asteroid::Main();
	}
};
