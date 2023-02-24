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
	Reg = new MMap(MMap::IO_Region(_uart_addr));
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
		while (!(Read<uint32_t>(Reg->GetBase() + Pl011_Regs::FR) && Pl011_Reg_FR::Busy));

		Write<uint8_t>(Reg->GetBase() + Pl011_Regs::DR, *buff++);
	}

}

}; // namespace device
}; // namespace saturn
