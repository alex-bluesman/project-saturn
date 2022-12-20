#ifndef __ASM_MMU_H__
#define __ASM_MMU_H__

/* MAIR contains the following attributes:
 *
 * index  value      description
 * ------------------------------------------------
 * 0_00   0000_0000  Strongly ordered/device nGnRnE
 * 0_01   0100_0100  In/Out non-cachable
 * 0_10   1010_1010  Write-through
 * 0_11   1110_1110  Write-back
 * 1_00   0000_0100  Device nGnRE
 * 1_01   -
 * 1_10   -
 * 1_11   1111_1111  Write-back alloc
 */

#define MAIR0_ATTR		0xeeaa4400U
#define MAIR1_ATTR		0xff000004U
#define MAIR_ATTR64		(MAIR1_ATTR << 32 | MAIR0_ATTR)

#define M_ATTR_DEVICE_nGnRnE	0x0
#define M_ATTR_NORMAL_NC	0x1
#define M_ATTR_NORMAL_WT	0x2
#define M_ATTR_NORMAL_WB	0x3
#define M_ATTR_DEVICE_nGnRE	0x4
#define M_ATTR_NORMAL		0x7

/* Initial value for TCR_EL2:
 * 
 *                  RES1     |    RES1    |   SH0_IS   | ORGN0_WBWA | IRGN0_WBWA | T0SZ = 48 bits
 */
#define TCR_EL2 ( (1U << 31) | (1U << 23) | (3U << 12) | (1U << 10) | (1U << 8) | (64 - 48) )

/* Let's map 4GB of RAM using 4KB page granularity:
 *
 *          L0       L1       L2        L3      Page
 *  --------------------------------------------------
 *  ... | 9 bits | 9 bits | 9 bits | 9 bits | 12 bits |
 *  --------------------------------------------------
 *     48       39       30        21
 *
 * TBD: should be reworked later to be more flexible
 */
#define MAPPED_ADDRESS_SPACE_GB	4

#define L0_SIZE			1				/* Single entry */
#define L1_SIZE			MAPPED_ADDRESS_SPACE_GB		/* 4GB of memory (4 * 1GB)   */
#define L2_SIZE			512				/* 1GB of memory (512 * 2MB) */
#define L3_SIZE			512				/* 2MB of memory (512 * 4KB) */

#define TABLE_INDEX_MASK	0x1ff				/* (512 - 1) */

/* Page shifts */
#define L1_ADDR_SHIFT		30				/* Address shift for L1 page tables entries */
#define L2_ADDR_SHIFT		21				/* Address shift for L2 page tables entries */
#define L3_ADDR_SHIFT		12				/* Address shift for L3 page tables entries */

/* See kernel/mmu.h for explanation of bits encoding */

/* Translation Table Entry pattern: table
 *                                   T           V
 */
#define TTE_TABLE		((1U << 1) | (1U << 0))

/* Translation Table Entry pattern: page
 *                                   nG           AF           SH          AP          NS        ATTR          T           P
 */
#define TTE_PAGE_DEVICE		((1U << 11) | (1U << 10) | (2U << 8) | (1U << 6) | (1U << 5) | (4U << 2) | (1U << 1) | (1U << 0))
#define TTE_PAGE_NORMAL		((1U << 11) | (1U << 10) | (3U << 8) | (1U << 6) | (1U << 5) | (7U << 2) | (1U << 1) | (1U << 0))

/* Translation Table Entry pattern: block
 *                                   nG           AF           SH          AP          NS        ATTR          P
 */
#define TTE_BLOCK_DEVICE	((1U << 11) | (1U << 10) | (2U << 8) | (1U << 6) | (1U << 5) | (4U << 2) | (1U << 0))
#define TTE_BLOCK_NORMAL	((1U << 11) | (1U << 10) | (3U << 8) | (1U << 6) | (1U << 5) | (7U << 2) | (1U << 0))

#endif /* __ASM_MMU_H__ */
