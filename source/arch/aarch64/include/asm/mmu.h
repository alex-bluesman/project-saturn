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

#define MAIR0_ATTR		0xeeaa4400
#define MAIR1_ATTR		0xff000004
#define MAIR_ATTR64		(MAIR1_ATTR << 32 | MAIR0_ATTR)

#define M_ATTR_DEVICE_nGnRnE	0x0
#define M_ATTR_NORMAL_NC	0x1
#define M_ATTR_NORMAL_WT	0x2
#define M_ATTR_NORMAL_WB	0x3
#define M_ATTR_DEVICE_nGnRE	0x4
#define M_ATTR_NORMAL		0x7

#endif /* __ASM_MMU_H__ */
