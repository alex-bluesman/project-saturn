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

// Registers usage:
//
// x23 - UART base address, used by early output routine
// x24 - Current CPU id
// x30 - Link register (LR)

#include "mmu.inc"

#define early_print(_s)		\
adr	x0, 98f;		\
bl	early_uart_print;	\
b	99f;			\
98:				\
	.asciz _s;		\
	.align 2;		\
99:

#define early_print_hex()	\
bl	early_uart_print_hex;

#define early_uart_init()	\
bl	early_uart_init

	.global start
start:

	.global smp_entry
smp_entry:
	msr	DAIFSet, 0xf		// Disable all interrupts

	early_uart_init()
	early_print("x\r\n")

	mov	x24, #0
	mrs	x0, mpidr_el1
	tst	x0, #(1 << 30)		// MPIDR_UP: 0 - processor is a part of cluster, 1 - uniprocessor system
	bne	1f
	ands	x24, x0, #3		// MPIDR_CPUID mask

	cbz	x24, 2f

	// TBD: SMP core initialization
1:
	b	.
2:
	// Print ASCII art designed by -hrr-
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
	early_print("                                                 Art by -hrr-\r\n\n")

	early_print(" - Project Saturn 2023 -\r\n\n")

	early_print(" > physical start: 0x")
	adr	x0, start
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
	// TBD: add CPU offset to properly map

	// Flush BSS with zeros
	ldr	x0, =_bss_start		// BSS start from .lds script
	ldr	x1, =_bss_end		// BSS end from .lds script
	add	x1, x1, x20
1:
	str	xzr, [x0], #8		// Store zero and increase index by 8 bytes
	cmp	x0, x1			// Check if we reach BSS end
	b.lo	1b

	ldr	x0, =MAIR_ATTR64	// Load memory attributes used in boot page tables
	msr	mair_el2, x0

	// Initialize TCR
	ldr	x0, =TCR_EL2
	mrs	x1, ID_AA64MMFR0_EL1
	bfi	x0, x1, #16, #3		// Get PARange (physical address range implemented)
	msr	tcr_el2, x0

	// TBD: initialize HSCTRL
	ldr	x0, =0x30c51878
	msr	SCTLR_EL2, x0

	// TBD
	msr	spsel, #1

	// According to the linker script the Saturn core composed from:
	//
	//              Virtual               Physical
	//  _start:    --------               --------
	//            |  Code  | ----------> |  Code  |  Read-only exectuable memory shared between CPUs
	//  _data:     --------               --------
	//            |        |          -> |  Data  |  CPU0 private writeable memory for data
	//  _bss:     .  Data  . ---- ...     --------
	//            |        |          -> |  Data  |  CPU1 private writeable memory for data
	//  _end:      --------               --------
	//                                   |  ...   |  CPUx ...
	//
	// The Saturn MMU implementation composed from two parts:
	//  - Static: it's fixed set of page tables needed to properly map Saturn core during
	//    boot time
	//  - Dynamic: it's mapping mechanism introduced in Saturn core, which is used for drivers
	//    and guest VMs mappings. It doesn't require any static tables, the allocation is done
	//    on heap
	//
	// Static boot tables:
	//
	//         L0                    L1                   L2                     L3
	//  ----------------      ----------------      ----------------      ----------------
	// | core_ptable_l0 |    | core_ptable_l1 |    | core_ptable_l2 |    | core_ptable_l3 |
	//  ----------------      ----------------      ----------------      ----------------
	//  [0]: 0..512 GB        [0]: 0..1 GB          [0]: 0..2 MB          [0]: 0..4 KB

	// Initialize L0 table
	ldr	x1, =core_ptable_l0		// x1 - level 0 page table address
	msr	TTBR0_EL2, x1			// Store table address

	// Map: Table L0 -> L1
	ldr	x2, =core_ptable_l1		// x2 - level 1 page table address
	mov	x3, #TTE_TABLE			// Attributes for translation table
	orr	x4, x2, x3			// Add attributes to table descriptor
	str	x4, [x1]			// Store core_ptable_l0[0] = core_ptable_l1[]

	// Map: Table L1 -> L2
	adr	x0, _start
	lsr	x0, x0, #L1_ADDR_SHIFT
	and	x0, x0, #TABLE_INDEX_MASK	// x0 = index for _start address in L1 table (bits 38..30)
	lsl	x0, x0, #3			// Get offset in L1 page table: core_ptable_l1[index]

	ldr	x1, =core_ptable_l2		// x1 - level 2 page table
	mov	x3, #TTE_TABLE			// Attributes for translation table
	orr	x4, x1, x3			// Add table attributes to descriptor
	str	x4, [x2, x0]			// Store core_ptable_l1[index] = core_ptable_l2[]

	// Map: Table L2 -> L3
	adr	x0, _start
	lsr	x0, x0, #L2_ADDR_SHIFT
	and	x0, x0, #TABLE_INDEX_MASK	// x0 = index for _start address in L2 table (bits 29..21)
	lsl	x0, x0, #3			// Get offset in L2 page table: core_ptable_l2[index]

	ldr	x2, =core_ptable_l3		// x1 - level 3 page table
	mov	x3, #TTE_TABLE			// Attributes for translation table
	orr	x4, x2, x3			// Add table attributes to descriptor
	str	x4, [x1, x0]			// Store core_ptable_l2[index] = core_ptable_l3[]

	// Map: Code
	adr	x0, _start
	adr	x5, _data

	lsr	x1, x0, #L3_ADDR_SHIFT
	and	x1, x1, #TABLE_INDEX_MASK	// x0 = index for _start address in L3 table (bits 20..12)
	lsl	x1, x1, #3			// Get offset in L3 page table: core_ptable_l3[index]

	mov	x3, #TTE_PAGE_NORMAL		// Attributes for page descriptor
1:
	orr	x4, x0, x3			// Add table attributes to descriptor
	str	x4, [x2, x1]			// Store core_ptable_l2[index] = core_ptable_l3[]
	add	x1, x1, #8			// Increase index in core_ptable_l3
	add	x0, x0, #4096			// Increase base address by one page
	cmp	x0, x5
	bne	1b

	// Map: Data
	adr	x0, _data
	adr	x5, _end

	sub	x6, x5, x0			// Get size of the whole Saturn data
	mul	x6, x6, x24			// Calculate data offset by CPU id

	lsr	x1, x0, #L3_ADDR_SHIFT
	and	x1, x1, #TABLE_INDEX_MASK	// x0 = index for _start address in L3 table (bits 20..12)
	lsl	x1, x1, #3			// Get offset in L3 page table: core_ptable_l3[index]

	mov	x3, #TTE_PAGE_NORMAL		// Attributes for page descriptor
1:
	add	x4, x0, x6			// Apply CPU offset to physical address
	orr	x4, x4, x3			// Add table attributes to descriptor
	str	x4, [x2, x1]			// Store core_ptable_l2[index] = core_ptable_l3[]
	add	x1, x1, #8			// Increase index in core_ptable_l3
	add	x0, x0, #4096			// Increase base address by one page
	cmp	x0, x5
	bne	1b

	tlbi	alle2				// Flush hypervisor TLB
	dsb	nsh

	mrs	x0, SCTLR_EL2
	orr	x0, x0, #(1 << 0)
	orr	x0, x0, #(1 << 2)
	dsb	sy				// Flush the ARM pipeline after enabling MMU
	msr	SCTLR_EL2, x0
	isb					// Flush I-cache

	ldr	x0, =boot_stack
	ldr	x1, =STACK_SIZE
	lsl	x1, x1, #3
	add	x0, x0, x1			// Stack grows down, so add STACK_SIZE * 8 (word size)
	mov	sp, x0

	b	saturn_init

	early_print("error: you should never see this")

	// Something went wrong, stop execution here
panic:
	wfe
	b	panic
