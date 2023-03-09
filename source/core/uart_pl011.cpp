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

#include <mmap>
#include <platform/qemuarm64>
#include <system>
#include "uart_pl011.hpp"

#include <io>

namespace saturn {
namespace device {

enum Pl011_Regs {
	DR	= 0x00,		// Data register
	FR	= 0x18,		// Flag register
	IBRD	= 0x24,		// Integer baud rate register
	FBRD	= 0x28,		// Fractional baud rate register
	LCR_H	= 0x2c,		// Line control register
	CR	= 0x30		// Control register
};

enum Pl011_Reg_FR {
	Busy	= 1 << 3	// UART busy bit
};

UartPl011::UartPl011()
{
	// TBD: destroy the allocated data
	Regs = new MMap(MMap::IO_Region(_uart_addr));
}

//UartPl011::~UartPl011()
//{
//}

void UartPl011::Rx(uint8_t *buff, size_t len)
{}

void UartPl011::Tx(uint8_t *buff, size_t len)
{
	while (len--)
	{
		// Wait for UART to be ready
		while (!(Regs->Read<uint32_t>(Pl011_Regs::FR) && Pl011_Reg_FR::Busy));

		Regs->Write<uint8_t>(Pl011_Regs::DR, *buff++);
	}
}

}; // namespace device
}; // namespace saturn
