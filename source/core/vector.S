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
#define Regs_LR_Offset		(30 * 8)
#define Regs_PC_Offset		(32 * 8)
#define Regs_CPSR_Offset	(33 * 8)
#define Regs_End_Offset		(34 * 8)

	.macro Store_Context
	sub	sp, sp, #(Regs_End_Offset - Regs_LR_Offset)	/* LR, SP. PC. CPSR */

	/* First save GP registers to be able to use them */
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
	stp	lr, x0, [sp, #Regs_LR_Offset]

	mrs	x0, elr_el2
	str	x0, [sp, #Regs_PC_Offset]

	mrs	x0, spsr_el2
	str	x0, [sp, #Regs_CPSR_Offset]

	.endm

	/* TBD: dummy handler for unsupported features */
unused:
	b	.

saturn_sync:
	Store_Context
	mov	x0, sp
	b	Sync_Abort

saturn_irq:
	Store_Context
	mov	x0, sp
	b	IRq_Handler

saturn_fiq:
	b	.

saturn_error:
	Store_Context
	mov	x0, sp
	b	System_Error

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
	entry	unused
	entry	unused
	entry	unused
	entry	unused

	/* Lower Exception level using AArch32 */
	entry	unused
	entry	unused
	entry	unused
	entry	unused
