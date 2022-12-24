#ifndef __ASM_SYS_CTRL_H__
#define __ASM_SYS_CTRL_H__

#define STACK_SIZE	1024			/* Stack size for one CPU */

#define MPIDR_UP	(1 << 30)		/* Multiprocessing extension:
						 *   0 - Processor is part of cluster
						 *   1 - Processor is uniprocessor
						 */
#define MPIDR_CPUID	(0x3)			/* Indicates the processor number */

#endif /* __ASM_SYS_CTRL_H__ */
