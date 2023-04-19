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

/* NOTE: this must be aligned with arm64/registers structure
 *       should be reworked later
 */
#define Regs_EL2_LR_Offset		(30 * 8)
#define Regs_EL2_SP_Offset		(31 * 8)
#define Regs_EL2_PC_Offset		(32 * 8)
#define Regs_EL2_CPSR_Offset		(33 * 8)

#define Regs_EL1_LR_Offset		(34 * 8)
#define Regs_EL1_SP_Offset		(35 * 8)
#define Regs_EL1_PC_Offset		(36 * 8)
#define Regs_EL1_CPSR_Offset		(37 * 8)

#define Regs_End_Offset			(38 * 8)

	// Store PE state before start exception processing
	.macro Store_Hyp_Frame
		// Skip Sys and Hyp mode frames:
		//  - Sys mode we just ignore here, because it's hypervisor context
		//  - Hyp mode will be filled in the end of macro
		sub	sp, sp, #(Regs_End_Offset - Regs_EL2_LR_Offset)

		// First save GP registers to be able to use them
		stp     x28, x29, [sp, #-16]!
		stp     x26, x27, [sp, #-16]!
		stp     x24, x25, [sp, #-16]!
		stp     x22, x23, [sp, #-16]!
		stp     x20, x21, [sp, #-16]!
		stp     x18, x19, [sp, #-16]!
		stp     x16, x17, [sp, #-16]!
		stp     x14, x15, [sp, #-16]!
		stp     x12, x13, [sp, #-16]!
		stp     x10, x11, [sp, #-16]!
		stp     x8, x9, [sp, #-16]!
		stp     x6, x7, [sp, #-16]!
		stp     x4, x5, [sp, #-16]!
		stp     x2, x3, [sp, #-16]!
		stp     x0, x1, [sp, #-16]!

		add	x0, sp, #Regs_End_Offset
		stp	lr, x0, [sp, #Regs_EL2_LR_Offset]	// Store LR_EL2 and SP_EL2

		mrs	x0, elr_el2
		str	x0, [sp, #Regs_EL2_PC_Offset]		// Store PC_EL2

		mrs	x0, spsr_el2
		str	x0, [sp, #Regs_EL2_CPSR_Offset]		// Store CPSR_EL2
	.endm

	/* Restore PE state after exception processing */
	.macro Restore_Hyp_Frame
		ldr	x0, [sp, #Regs_EL2_CPSR_Offset]
		msr	spsr_el2, x0

		ldr	x0, [sp, #Regs_EL2_PC_Offset]
		msr	elr_el2, x0

		ldr	lr, [sp, #Regs_EL2_LR_Offset]

		ldp	x0, x1, [sp], #16
		ldp	x2, x3, [sp], #16
		ldp	x4, x5, [sp], #16
		ldp	x6, x7, [sp], #16
		ldp	x8, x9, [sp], #16
		ldp	x10, x11, [sp], #16
		ldp	x12, x13, [sp], #16
		ldp	x14, x15, [sp], #16
		ldp	x16, x17, [sp], #16
		ldp	x18, x19, [sp], #16
		ldp	x20, x21, [sp], #16
		ldp	x22, x23, [sp], #16
		ldp	x24, x25, [sp], #16
		ldp	x26, x27, [sp], #16
		ldp	x28, x29, [sp], #16

		// Skip Sys and Hyp mode frames to wipe AArch64_Regs structure out from stack
		add	sp, sp, #(Regs_End_Offset - Regs_EL2_LR_Offset)
	.endm

	.macro Store_Sys_Frame
		str	lr, [sp, #Regs_EL1_LR_Offset]

		mrs	x0, sp_el1
		str	x0, [sp, #Regs_EL1_SP_Offset]

		mrs	x0, elr_el1
		str	x0, [sp, #Regs_EL1_PC_Offset]

		mrs	x0, spsr_el1
		str	x0, [sp, #Regs_EL1_CPSR_Offset]
	.endm

	.macro Restore_Sys_Frame
		ldr	x0, [sp, #Regs_EL1_CPSR_Offset]
		msr	spsr_el1, x0

		ldr	x0, [sp, #Regs_EL1_PC_Offset]
		msr	elr_el1, x0

		ldr	x0, [sp, #Regs_EL1_SP_Offset]
		msr	sp_el1, x0

		ldr	lr, [sp, #Regs_EL1_LR_Offset]
	.endm

	/* TBD: dummy handler for unsupported features */
unused:
	b	.

saturn_sync:
	msr	DAIFSet, #15	/* Mask {D, A, I, F} due to we never return */
	Store_Hyp_Frame
	mov	x0, sp
	b	Sync_Abort

saturn_irq:
	msr	DAIFSet, #2	/* Mask {I} to avoid nested interrupts */
	Store_Hyp_Frame

	mov	x0, sp
	bl	IRq_Handler

	Restore_Hyp_Frame
	msr	DAIFClr, #2	/* Unmask {I} to enable interrupts back */
	eret

saturn_fiq:
	b	.

saturn_error:
	Store_Hyp_Frame
	mov	x0, sp
	b	System_Error

guest_sync:
	Store_Hyp_Frame
	Store_Sys_Frame

	mov	x0, sp
	bl	Guest_Abort

	Restore_Sys_Frame
	Restore_Hyp_Frame
	eret

guest_error:
	Store_Hyp_Frame
	Store_Sys_Frame
	mov	x0, sp
	b	Guest_Error

	.macro	entry	handler
	.align 7
	b	\handler
	.endm

	.align 11
	.global saturn_vector
saturn_vector:

	/* Current Exception level with SP_EL0 (EL2t) */
	entry	unused
	entry	unused
	entry	unused
	entry	unused

	/* Current Exception level with SP_ELx, x > 0 */
	entry	saturn_sync
	entry	saturn_irq
	entry	saturn_fiq
	entry	saturn_error

	/* Lower Exception level using AArch64 */
	entry	guest_sync
	entry	unused
	entry	unused
	entry	guest_error

	/* Lower Exception level using AArch32 */
	entry	unused
	entry	unused
	entry	unused
	entry	unused

	.align 3
	.global Switch_EL12
Switch_EL12:
	// We will not return from this function to current hypervisor running
	// context, so this means we don't need hypervisor stack anymore. After
	// all the registers are set, we could reset the stack to the initial
	// boot state to avoid stack leak

	// x0 contains pointer to AArch64_Regs structure
	mov	sp, x0

	// Restore the VM context
	Restore_Sys_Frame
	Restore_Hyp_Frame

	// Reset hypervisor stack to initial value
	ldr	x20, =el2_stack_reset
	ldr	x21, [x20]
	mov	sp, x21
	mov	x20, xzr
	mov	x21, xzr

	eret
