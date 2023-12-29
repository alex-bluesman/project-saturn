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

static struct Pl011Regs _pl011_state;

VirtUartPl011::VirtUartPl011(UartPl011& uart)
	: hwUart(uart)
	, regState(_pl011_state)
{
	mTrap = new MTrap(_uart_addr, *this);

	hwUart.Load_State(regState);
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
			uint32_t* tdr = static_cast<uint32_t*>(data);

			*tdr = c;

			break;
		}
	case Pl011_Regs::FR:
		{
			// Notify that UART is always ready
			uint16_t* fr = static_cast<uint16_t*>(data);
			*fr = (1 << 7);

			if (iConsole().RxFifoEmpty())
			{
				*fr |= 0x10;
			}
			break;
		}
	case Pl011_Regs::IMSC:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = regState.imsc;
			break;
		}
	case Pl011_Regs::CR:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = regState.cr;
			break;
		}
	case Pl011_Regs::RIS:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			if (iConsole().RxFifoEmpty())
			{
				*val = 0;
			}
			else
			{
				*val = Pl011_INT::RX;
			}
			break;
		}
	case PERIPHID0:
	case PERIPHID0 + 0x4:
	case PERIPHID0 + 0x8:
	case PERIPHID0 + 0xc:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			uint8_t id = (reg - PERIPHID0) / 4;

			*val = regState.periphid[id];
			break;
		}
	case PCELLID0:
	case PCELLID0 + 0x4:
	case PCELLID0 + 0x8:
	case PCELLID0 + 0xc:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			uint8_t id = (reg - PCELLID0) / 4;

			*val = regState.pcellid[id];
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
			uint8_t c = *static_cast<uint8_t*>(data);
			hwUart.Tx(&c, 1);
			break;
		}
	case Pl011_Regs::IMSC:
		{
			regState.imsc = *static_cast<uint32_t*>(data);
			break;
		}
	case Pl011_Regs::CR:
		{
			regState.cr = *static_cast<uint32_t*>(data);
			break;
		}
	default:
		// TBD: ignore others for now
		break;
	}
}

}; // namespace device
}; // namespace saturn
