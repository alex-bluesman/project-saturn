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

extern void Exceptions_Init();

static void Context_Info(void)
{
	uint64_t reg;

	reg = ReadArm64Reg(CurrentEL);
	Info() << "  running in EL" << (reg >> 2) << fmt::endl;
}

static void Gic_Info(void)
{
	uint32_t reg = Read<uint32_t>(0x08000000);
	Info() << "gicd.ctrl = 0x" << fmt::hex << fmt::fill << reg << fmt::endl;

	// Single priority group
	WriteICCReg(ICC_BPR1_EL1, 0);

	// Set priority mask to handle all interrupts
	WriteICCReg(ICC_PMR_EL1, 0xff);

	// EOIR provides priority drop functionality only.
	// DIR provides interrupt deactivation functionality.
	WriteICCReg(ICC_CTRL_EL1, 1 << 1);
	
	// Enable Group1 interrupts
	WriteICCReg(ICC_IGRPEN1_EL1, 1);

	asm volatile (
			"msr daifclr, #2\n"
			"isb\n"
			::: "memory"
		     );

	Info() << "RUN!" << fmt::endl;

	Write<uint32_t>(0x08000000 + 0x0104, 1);
}

void Main()
{
	Heap Main_Heap;
	Asteroid_Heap = &Main_Heap;

	Asteroid_Console = new Console();

	device::UartPl011& Uart = *new device::UartPl011();
	Asteroid_Console->RegisterUart(Uart);

	Exceptions_Init();

	Raw() << fmt::endl;
	Raw() << " = Asteroid Application 2023 =" << fmt::endl;
	Raw() << fmt::endl;

	Context_Info();
	Gic_Info();

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
