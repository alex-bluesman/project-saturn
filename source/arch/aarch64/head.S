/*
 * Registers usage:
 *
 * x20 - Physical boot address offset
 * x23 - UART base address, used by early output routine
 * x24 - Current CPU id
 * x30 - Link register (LR)
 */

#include <asm/early_print.h>
#include <asm/mmu.h>
#include <asm/sys_ctrl.h>

	.global start
start:
	b	skip_header

	/* TBD: add UEFI or zImage header here */

skip_header:

	.global smp_entry
smp_entry:
	msr	DAIFSet, 0xf		/* Disable all interrupts */

	ldr	x0, =start		/* Get relative boot address */
	adr	x19, start		/* Get physical boot address */
	sub	x20, x19, x0		/* x20 - physical offset */

	early_uart_init()
	early_print("x\r\n")

	mov	x24, #0
	mrs	x0, mpidr_el1
	tst	x0, MPIDR_UP
	bne	1f
	ands	x24, x0, #MPIDR_CPUID

	cbz	x24, 2f

	/* TBD: SMP core initialization */
1:
	b	.
2:
	/* Print ASCII art designed by -hrr- */
	early_print("                                                  ,o88888\r\n")
	early_print("                                               ,o8888888'\r\n")
	early_print("                         ,:o:o:oooo.        ,8O88Pd8888\"\r\n")
	early_print("                     ,.::.::o:ooooOoOoO. ,oO8O8Pd888'\"\r\n")
	early_print("                   ,.:.::o:ooOoOoOO8O8OOo.8OOPd8O8O\"\r\n")
	early_print("                  , ..:.::o:ooOoOOOO8OOOOo.FdO8O8\"\r\n")
	early_print("                 , ..:.::o:ooOoOO8O888O8O,COCOO\"\r\n")
	early_print("                , . ..:.::o:ooOoOOOO8OOOOCOCO\"\r\n")
	early_print("                 . ..:.::o:ooOoOoOO8O8OCCCC\"o\r\n")
	early_print("                    . ..:.::o:ooooOoCoCCC\"o:o\r\n")
	early_print("                    . ..:.::o:o:,cooooCo\"oo:o:\r\n")
	early_print("                 `   . . ..:.:cocoooo\"'o:o:::'\r\n")
	early_print("                 .`   . ..::ccccoc\"'o:o:o:::'\r\n")
	early_print("                :.:.    ,c:cccc\"':.:.:.:.:.'\r\n")
	early_print("              ..:.:\"'`::::c:\"'..:.:.:.:.:.'\r\n")
	early_print("            ...:.'.:.::::\"'    . . . . .'\r\n")
	early_print("           .. . ....:.\"' `   .  . . ''\r\n")
	early_print("         . . . ....\"'\r\n")
	early_print("         .. . .\"'\r\n")
	early_print("        .\r\n")

	early_print(" - Project Saturn 2019 -\r\n\n")

	early_print(" > physical start: 0x")
	adr	x0, start
	early_print_hex()
        early_print("\r\n > link offset:    0x")
	mov	x0, x20
	early_print_hex()
        early_print("\r\n")

check_el2:
	mrs	x0, CurrentEL
	lsr	x0, x0, #2
	cmp	x0, #2
	beq	common_start

	early_print("error: hypervisor should start in EL2 mode\r\n")
	b	panic

common_start:
	/* TBD: add CPU offset */

	/* Flush BSS with zeros */
	ldr	x0, =_bss_start		/* BSS start from .lds script */
	add	x0, x0, x20
	ldr	x1, =_bss_end		/* BSS end from .lds script */
	add	x1, x1, x20
1:
	str	xzr, [x0], #8		/* Store zero and increase index by 8 bytes */
	cmp	x0, x1			/* Check if we reach BSS end */
	b.lo	1b

	ldr	x0, =MAIR_ATTR64	/* Load memory attributes used in boot page tables */
	msr	mair_el2, x0

	/* Initialize TCR */
	ldr	x0, =TCR_EL2
	mrs	x1, ID_AA64MMFR0_EL1
	bfi	x0, x1, #16, #3		/* Get PARange (physical address range implemented) */
	msr	tcr_el2, x0

	/* TBD: initialize HSCTRL */
	ldr	x0, =0x30c51878
	msr	SCTLR_EL2, x0

	/* TBD */
	msr	spsel, #1

	/* According to the linker script:
	 *
	 *  _start:   Code
	 *  +2MB  :   Data
	 *
	 *       L0               L1             L2
	 *  -----------      -----------     -----------
	 * | ptable_l0 |    | ptable_l1 |   | ptable_l2 |
	 *  -----------      -----------     -----------
	 * [0]: 0..512 GB   [0]: 0..1 GB    [0][0]: 0..2 MB
	 *                  [1]: 1..2 GB    ...
	 *                  [2]: 2..3 GB
	 *                  [3]: 3..4 GB
	 */

	/* Initialize L0 table */
	ldr	x1, =ptable_l0			/* x1 - level 0 page table address */
	add	x1, x1, x20			/* Add physical memory offset */
	msr	TTBR0_EL2, x1			/* Store table address */

	/* Map L0 -> L1 */
	ldr	x2, =ptable_l1			/* x2 - level 1 page table address */
	add	x2, x2, x20			/* Add physical memory offset */
	mov	x3, #TTE_TABLE			/* Attributes for translation table */
	orr	x4, x2, x3			/* Add attributes to table descriptor */
	str	x4, [x1]			/* Store ptable_l0[0] = ptable_l1 */

	/* Map L1 -> L2 */
	ldr	x1, =ptable_l2
	add	x1, x1, x20			/* Add physical memory offset */
	adr	x0, start			/* x0 - physical boot start */
	lsr	x0, x0, #L1_ADDR_SHIFT		/* Get index for _start address in L1 table (bits 31..30) */
	lsl	x3, x0, #12			/* Get offset in L2 page table: ptable_l2[index][] */
	add	x1, x1, x3			/* x1 - physical address for ptable_l2[index] */
	mov	x3, #TTE_TABLE			/* Attributes for translation table */
	orr	x3, x1, x3			/* Add table attributes to descriptor */
	lsl	x4, x0, #3			/* Get offset in L1 page table: ptable_l1[index] */
	str	x3, [x2, x4]			/* Store ptable_l1[index] = ptable_l2[index][] */

	/* Map L2 -> 2MB: text */
	adr	x0, start
	lsr	x0, x0, #L2_ADDR_SHIFT		/* Text start address in 2MB blocks (bits 31..21) */
	and	x2, x0, #TABLE_INDEX_MASK	/* x2 - text start index in ptable_l2 table */
	lsl	x2, x2, 3			/* Get offset in the table: offset = index * 8 (descriptor size) */
	lsl	x0, x0, #L2_ADDR_SHIFT
	mov	x3, #TTE_BLOCK_NORMAL		/* Attributes for page/block */
	orr	x0, x0, x3			/* Add block attributes to descriptor */
	str	x0, [x1, x2]			/* Store descriptor to ptable_l2[][] entry */

	/* Map 1-to-1 1GB of RAM, usually it contains all the IOMEM for peripherals.
	 * TBD: do it on demand in device drivers
	 */
	ldr	x0, =ptable_l1			/* x1 - ptable_l1[0] */
	add	x0, x0, x20			/* Add physical memory offset */
	mov	x1, #TTE_BLOCK_DEVICE		/* Due to it's the 1st GB, then the base address is 0 */
	str	x1, [x0]			/* Store descriptor to ptable_l1[0] */

	tlbi	alle2				/* Flush hypervisor TLB */
	dsb	nsh

	mrs	x0, SCTLR_EL2
	orr	x0, x0, #(1 << 0)
	orr	x0, x0, #(1 << 2)
	dsb	sy				/* Flush the ARM pipeline after enabling MMU */
	msr	SCTLR_EL2, x0
	isb					/* Flush I-cache */

	ldr	x0, =boot_stack
	add	x0, x0, x20
	ldr	x1, =STACK_SIZE
	lsl	x1, x1, #3
	add	x0, x0, x1			/* Stack grows down, so add STACK_SIZE * 8 (word size) */
	mov	sp, x0

	b	saturn_init

	early_print("error: you should never see this")

	/* Something went wrong, stop execution here */
panic:
	wfe
	b	panic
