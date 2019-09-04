#include <asm/io_mem.h>
#include <io.h>
#include <mmu.h>
#include <types.h>

static uint64_t uart_base_addr;

void uart_init(void)
{
	uart_base_addr = UART_BASE_ADDRESS;
	mmap_io(uart_base_addr, PAGE_SIZE);
}

void uart_tx(uint8_t *buff, size_t len)
{
	volatile uint32_t *reg = (void *)(uart_base_addr + PL011_REG_FR);

	while (len--)
	{
		/* Wait for UART to be ready */
		while (!(*reg && PL011_REG_FR_BUSY));
		mem_write8(uart_base_addr + PL011_REG_DR, *buff++);
	}
}
