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

	early_print(" - Project Saturn 2019 -\r\n")

check_el2:
	mrs	x0, CurrentEL
	lsr	x0, x0, #2
	cmp	x0, #2
	beq	common_start

	early_print("error: hypervisor should start in EL2 mode\r\n")
	b	panic

common_start:
	ldr	x0, =MAIR_ATTR64	/* Load memory attributes used in boot page tables */
	msr	mair_el2, x0

	/* TBD: enable MMU */
	dsb	sy			/* Flush the ARM pipeline after enabling MMU */
	isb				/* Flush I-cache */

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
