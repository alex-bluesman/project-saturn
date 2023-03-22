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

#include <core/iconsole>
#include <core/iirq>
#include <mmap>
#include <platform/qemuarm64>
#include <system>

namespace saturn {
namespace device {

using namespace core;

static const uint32_t _pl011_int = 33;	// PL011 UART interrupt in QEMU virt machine

// TBD: workaround to let INT handler access driver instance from outside
UartPl011* UartPl011::Self = nullptr;

enum Pl011_Regs {
	TDR	= 0x00,		// Data register
	FR	= 0x18,		// Flag register
	IBRD	= 0x24,		// Integer baud rate register
	FBRD	= 0x28,		// Fractional baud rate register
	LCR_H	= 0x2c,		// Line control register
	CR	= 0x30,		// Control register
	RIS	= 0x3c,		// Raw interrupt status register
	IMSC	= 0x38,		// Interrupt mask register
	ICR	= 0x44		// Interrupt clear register
};

enum Reg_CR {
	Enable	= 1 << 0	// UART enable bit
};

enum Reg_FR {
	Busy	= 1 << 3	// UART busy bit
};

enum Pl011_INT {
	RX	= 1 << 4	// RX interrupt
};

UartPl011::UartPl011()
{
	UartPl011::Self = this;

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
		while (!(Regs->Read<uint32_t>(Pl011_Regs::FR) && Reg_FR::Busy));

		Regs->Write<uint8_t>(Pl011_Regs::TDR, *buff++);
	}
}

void UartPl011::HandleIRq(void)
{
	uint32_t status = Regs->Read<uint32_t>(Pl011_Regs::RIS);

	// Clear pending interrupts and process the status
	Regs->Write<uint32_t>(Pl011_Regs::ICR, status);

	if (status & Pl011_INT::RX)
	{
		status &= ~Pl011_INT::RX;

		uint8_t data = Regs->Read<uint16_t>(Pl011_Regs::TDR) & 0xff;

		ConIO().UartRX(static_cast<char>(data));
	}

	if (status != 0)
	{
		// TBD: do we need to care about this?
		// Log() << "uart: unexpected interrupt, mask = 0x" << fmt::hex << fmt::fill << status << fmt::endl;
	}
}

// Static method to register within IC. It forwards the handling to Pl011 object via static pointer.
void UartPl011::UartIRqHandler(uint32_t id)
{
	Self->HandleIRq();
}

void UartPl011::EnableRx(void)
{
	// Register INT handler and enable respective interrupt
	IC().Register_IRq_Handler(_pl011_int, &UartIRqHandler);

	// Disable UART
	uint32_t cr = Regs->Read<uint32_t>(Pl011_Regs::CR);
	cr &= ~Reg_CR::Enable;
	Regs->Write<uint32_t>(Pl011_Regs::CR, cr);

	// Disable FIFO to flush the buffer
	uint32_t lcr_h = Regs->Read<uint32_t>(Pl011_Regs::LCR_H);
	lcr_h &= ~(1 << 4);
	Regs->Write<uint32_t>(Pl011_Regs::LCR_H, lcr_h);

	// Mask all the interrupts to properly initialize
	Regs->Write<uint32_t>(Pl011_Regs::IMSC, 0);

	// Enable FIFO
	lcr_h |= (1 << 4);
	Regs->Write<uint32_t>(Pl011_Regs::LCR_H, lcr_h);

	// Enable RX interrupts
	Regs->Write<uint32_t>(Pl011_Regs::IMSC, Pl011_INT::RX);

	// Enable UART back
	Regs->Write<uint32_t>(Pl011_Regs::CR, cr | Reg_CR::Enable);
}

}; // namespace device
}; // namespace saturn
