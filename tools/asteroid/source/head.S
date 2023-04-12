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
// x20 - Physical boot address offset
// x23 - UART base address, used by early output routine
// x24 - Current CPU id
// x30 - Link register (LR)

	.global start
start:

	.global smp_entry
smp_entry:
	msr	DAIFSet, 0xf		// Disable all interrupts

	ldr	x0, =start		// Get relative boot address
	adr	x19, start		// Get physical boot address
	sub	x20, x19, x0		// x20 - physical offset

	mov	x24, #0
	mrs	x0, mpidr_el1
	tst	x0, #(1 << 30)		// MPIDR_UP: 0 - processor is a part of cluster, 1 - uniprocessor system
	bne	1f
	ands	x24, x0, #3		// MPIDR_CPUID mask

	cbz	x24, common_start

	// TBD: SMP core initialization
1:
	b	.

common_start:
	// Flush BSS with zeros
	ldr	x0, =_bss_start		// BSS start from .lds script
	add	x0, x0, x20
	ldr	x1, =_bss_end		// BSS end from .lds script
	add	x1, x1, x20
1:
	str	xzr, [x0], #8		// Store zero and increase index by 8 bytes
	cmp	x0, x1			// Check if we reach BSS end
	b.lo	1b

	// Setup stack
	ldr	x0, =boot_stack
	add	x0, x0, x20
	ldr	x1, =STACK_SIZE
	lsl	x1, x1, #3
	add	x0, x0, x1		// Stack grows down, so add STACK_SIZE * 8 (word size)
	mov	sp, x0

	b	Asteroid_Init

	// Something went wrong, stop execution here
panic:
	wfe
	b	panic
