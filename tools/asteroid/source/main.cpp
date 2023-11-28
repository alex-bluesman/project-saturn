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

#define TICK_HZ			1			// Desired tick resolution
#define TIMER_HZ		64500000		// Physical counter resolution
#define TIMER_PERIOD_MS		(TIMER_HZ / TICK_HZ)	// Scheduling period

static uint64_t _timer_counter;

static void Timer_IRq_Handler(uint32_t id)
{
	_timer_counter++;
	Info() << "timer: tick " << _timer_counter << fmt::endl;

	uint64_t cval = ReadArm64Reg(CNTV_CVAL_EL0);
	cval += TIMER_PERIOD_MS;
	WriteArm64Reg(CNTV_CVAL_EL0, cval);
}

static void Timer_App(void)
{
	_timer_counter = 0;

	// Disable timer
	WriteArm64Reg(CNTV_CTL_EL0, 0);

	iIC().Register_IRq_Handler(27, &Timer_IRq_Handler);

	uint64_t cval = ReadArm64Reg(CNTVCT_EL0);
	WriteArm64Reg(CNTV_CVAL_EL0, cval);

	WriteArm64Reg(CNTV_CTL_EL0, 1);
}

static void Demo_Application(void)
{
	Info() << "* start demo application *" << fmt::endl;

	Timer_App();

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
