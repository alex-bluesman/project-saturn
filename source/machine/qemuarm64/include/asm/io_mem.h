#ifndef __IO_MEM_H__
#define __IO_MEM_H__

#define UART_BASE_ADDRESS		0x09000000	/* QEMU virt machine UART base address */

#define PL011_REG_DR			(0x00)		/* Data register */
#define PL011_REG_FR			(0x18)		/* Flag register */
#define PL011_REG_IBRD			(0x24)		/* Integer baud rate register */
#define PL011_REG_FBRD			(0x28)		/* Fractional baud rate register */
#define PL011_REG_LCR_H			(0x2c)		/* Line control register */
#define PL011_REG_CR			(0x30)		/* Control register */

#define PL011_REG_CR_UARTEN		(1 << 0)	/* UART enable bit */
#define PL011_REG_CR_TXE		(8 << 0)	/* TX enable bit */
#define PL011_REG_CR_RXE		(9 << 0)	/* RX enable bit */

#define PL011_REG_LCR_H_WLEN_8B		(3 << 5)	/* Word length set to 8 bits */

#define PL011_REG_FR_BUSY		(1 << 3)	/* UART busy bit */

#endif
