#include <asm/mmu.h>
#include <console.h>
#include <sys.h>
#include <types.h>

/*         Level 0, 1, 2: Block, Level 3: Page
 * ----------------------------------------------------
 *
 * bit[0]      - indicates if descriptor is valid
 * bit[1]      - descriptor type: table(1) or memory block(0)

 * bits[11:2]  - lower block attributes
 *   bits[4:2] - attribute index AttrIndx[2:0] in MAIR
 *   bit[5]    - non-secure bit
 *   bits[7:6] - data access permissions AP[2:1]
 *   bits[9:8] - shareability field
 *   bit[10]   - access flag
 *   bit[11]   - not global bit
 *
 * bits[47:12] - output address
 *   bits[47:30] - for level 1
 *   bits[47:21] - for level 2
 *   bits[47:12] - for level 3
 *
 * bits[49:47] - RES0
 * bit[50]     - GP ?
 * bit[51]     - RES0
 *
 * bits[63:52] - upper block attributes
 */

/*                Level 0, 1, 2: Table
 * ----------------------------------------------------
 * bit[0]      - indicates if descriptor is valid
 * bit[1]      - descriptor type: table(1) or memory block(0)
 * bits[12:2]  - ignored
 * bits[47:12] - next-level table address [47:12]
 * bits[51:48] - RES0
 * bits[58:52] - ignored
 * bit[59]     - privileged execute-never PXNTable
 * bit[60]     - execute-never control UXNTable
 * bits[62:61] - access permissions limit for subsequent level APTable
 * bit[63]     - non-secure memory access NSTable
 */
typedef struct __packed {
	uint64_t data;
} tt_desc_t;

tt_desc_t ptable_l0[L0_SIZE] __align(1 << 12);
tt_desc_t ptable_l1[L1_SIZE] __align(1 << 12);
tt_desc_t ptable_l2[L1_SIZE][L2_SIZE] __align(1 << 12);

#ifdef DEBUG
static void mmu_dump_tables(void)
{
	int i, j;

	console_dbg("Dump of MMU tables:\r\n\n");

	console_dbg_va("ptable_l0 base address: 0x%16x\r\n", ptable_l0);
	console_dbg_va("ptable_l1 base address: 0x%16x\r\n", ptable_l1);
	console_dbg_va("ptable_l2 base address: 0x%16x\r\n\n", ptable_l2);

	console_dbg_va("  ptable_l0[0] = 0x%16x\r\n", ptable_l0[0]);

	for (i = 0; i < L1_SIZE; i++)
	{
		if (ptable_l1[i].data != 0)
		{
			console_dbg_va("    ptable_l1[%d] = 0x%16x\r\n", i, ptable_l1[i]);
			for (j = 0; j < L2_SIZE; j++)
			{
				if (ptable_l2[i][j].data != 0)
				{
					console_dbg_va("      ptable_l2[%d][%d] = 0x%16x\r\n", i, j, ptable_l2[i][j]);
				}
			}
		}
	}
}
#endif /* DEBUG */

void mmu_init(void)
{
#ifdef DEBUG
	mmu_dump_tables();
#endif /* DEBUG */
}
