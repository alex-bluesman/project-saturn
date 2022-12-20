#define EARLY_UART_BASE		0x09000000	/* Base UART address for QEMU Aarch64 machine */
#define EARLY_UART_BAUD		115200		/* Default baud rate */

#define PL011_REG_DR		(0x00)		/* Data register */
#define PL011_REG_FR		(0x18)		/* Flag register */
#define PL011_REG_IBRD		(0x24)		/* Integer baud rate register */
#define PL011_REG_FBRD		(0x28)		/* Fractional baud rate register */
#define PL011_REG_LCR_H		(0x2c)		/* Line control register */
#define PL011_REG_CR		(0x30)		/* Control register */

#define PL011_REG_CR_UARTEN	(1 << 0)	/* UART enable bit */
#define PL011_REG_CR_TXE	(8 << 0)	/* TX enable bit */
#define PL011_REG_CR_RXE	(9 << 0)	/* RX enable bit */
#define PL011_REG_LCR_H_WLEN_8B	(3 << 5)	/* Word length set to 8 bits */
#define PL011_REG_FR_BUSY	(1 << 3)	/* UART busy bit */

	.global	early_uart_init
	.align 2
early_uart_init:

	ldr	x23, =EARLY_UART_BASE

	/* The baud divisor can be calculated as follows:
	 *
	 *   BAUDDIV = (F_uclk / (16 * BAUD))
	 *
	 * where F_uclk is default UART clock 7.3728MHz
	 */
	mov	x1, #(7372800 / EARLY_UART_BAUD % 16)			/* Fractional divisor */
	strh	w1, [x23, #PL011_REG_FBRD]
	mov	x1, #(7372800 / EARLY_UART_BAUD / 16)			/* Integer divisor */
	strh	w1, [x23, #PL011_REG_IBRD]

	mov	x1, #PL011_REG_LCR_H_WLEN_8B				/* 8 bits, no parity */
	str	w1, [x23, #PL011_REG_LCR_H]

	ldr	x1, =(PL011_REG_CR_UARTEN | PL011_REG_CR_TXE | PL011_REG_CR_RXE)
	str	w1, [x23, #PL011_REG_CR]				/* Enable UART RX and TX */

	ret


	.global early_uart_print
	.align 2
early_uart_print:
1:
	/* Wait for the UART to be ready */
	ldrh	w1, [x23, #PL011_REG_FR]
	tst	w1, #PL011_REG_FR_BUSY					/* Check BUSY bit */
	b.ne	1b

	ldrb	w1, [x0], #1						/* Load next char */
	cbz	w1, 2f

	strb	w1, [x23, PL011_REG_DR]					/* Send char to TX FIFO */
	b	early_uart_print
2:
	ret


	.global early_uart_print_hex
early_uart_print_hex:
	adr	x1, hex
	mov	x3, #8
1:
	/* Wait for the UART to be ready */
	ldrh	w2, [x23, #PL011_REG_FR]
	tst	w2, #PL011_REG_FR_BUSY					/* Check BUSY bit */
	b.ne	1b

	and	x2, x0, #0xf0000000					/* Mask off the top nybble */
	lsr	x2, x2, #28
	ldrb	w2, [x1, x2]						/* Convert to a char */

	strb	w2, [x23, PL011_REG_DR]					/* Send char to TX FIFO */

	lsl	x0, x0, #4						/* Roll it through one nybble at a time */
	subs	x3, x3, #1
	b.ne	1b

	ret
hex:
	.ascii	"0123456789abcdef"
	.align	2
