	/* TBD: dummy handler for unsupported features */
unused:
	b	.

saturn_sync:
	b	Sync_Abort

saturn_irq:
	b	.

saturn_fiq:
	b	.

saturn_error:
	b	.


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
