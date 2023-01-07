#include <mmap>
#include <system>
#include "uart_pl011.hpp"

#include <io>

namespace saturn {
namespace device {

static const uint64_t uart_base_address = 0x09000000;

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
	// Let's keep constructor empty, because we could create the
	// object before MMU is initialized. When system is ready a
	// user could call Init method instead.
}

//UartPl011::~UartPl011()
//{
	// TBD: destroy the allocated data
//}

void UartPl011::Init(void)
{
	Reg = new MMap(uart_base_address, _page_size, MMapType::Device);
}

void UartPl011::Rx(uint8_t *buff, size_t len)
{}

void UartPl011::Tx(uint8_t *buff, size_t len)
{
	// TBD: rework by using mem_read32 to avoid this black magic
	volatile uint32_t *reg = (uint32_t *)(void *)(uart_base_address + Pl011_Regs::FR);

	while (len--)
	{
		// Wait for UART to be ready
		while (!(*reg && Pl011_Reg_FR::Busy));
		mem_write8(uart_base_address + Pl011_Regs::DR, *buff++);
	}

}

}; // namespace device
}; // namespace saturn
