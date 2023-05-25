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

#include "uart_pl011.hpp"
#include "virt_uart_pl011.hpp"

#include <core/iconsole>

namespace saturn {
namespace device {

static device::UartPl011* Uart = nullptr;
static device::VirtUartPl011* VirtUart = nullptr;

void BSP_Init(void)
{
	Uart = new device::UartPl011();
	core::iConsole().RegisterUart(*Uart);

	VirtUart = new device::VirtUartPl011();
}

}; // namespace device
}; // namespace saturn
