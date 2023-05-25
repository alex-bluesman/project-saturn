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

#include <bsp/platform>
#include <core/iconsole>

namespace saturn {
namespace device {

using namespace core;

VirtUartPl011::VirtUartPl011()
{
	mTrap = new MTrap(_uart_addr, *this);
}

VirtUartPl011::~VirtUartPl011()
{}

void VirtUartPl011::Read(uint64_t reg, void* data, AccessSize size)
{
	switch (reg)
	{
	case Pl011_Regs::TDR:
		{
			// Get char from console buffer
			char c = iConsole().GetChar(iomode::async);
			uint8_t* tdr = static_cast<uint8_t*>(data);
			*tdr = c;
			break;
		}
	case Pl011_Regs::FR:
		{
			// Notify that UART is always ready
			uint8_t* fr = static_cast<uint8_t*>(data);
			*fr = 0;
			break;
		}
	default:
		// TBD: ignore others for now
		break;
	}
}

void VirtUartPl011::Write(uint64_t reg, void* data, AccessSize size)
{
	switch (reg)
	{
	case Pl011_Regs::TDR:
		{
			// Send char to TX
			char c = *static_cast<char *>(data);
			Raw() << c;
			break;
		}
	default:
		// TBD: ignore others for now
		break;
	}
}

}; // namespace device
}; // namespace saturn
