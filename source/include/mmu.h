#ifndef __KERNEL_MMU_H__
#define __KERNEL_MMU_H__

#include <sys.h>
#include <types.h>

/* Page definitions: */
#define PAGE_SIZE		(1 << 12)
#define PAGE_MASK		(PAGE_SIZE - 1)
#define PAGE_BASE_ADDR		(~PAGE_MASK)

/* Memory mapping types: */
#define MMAP_DEVICE		0
#define MMAP_NORMAL		1


/*         Level 3: page
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
 * bits[49:48] - RES0
 * bit[50]     - GP ?
 * bit[51]     - RES0
 *
 * bits[63:52] - upper block attributes
 * TBD:
 */
typedef struct __packed {
	/* For valid page should be: valid = 1, type = 1 */
	uint64_t valid:1;
	uint64_t type:1;

	/* Low attributes */
	uint64_t attr:3;
	uint64_t ns:1;
	uint64_t ap:2;
	uint64_t sh:2;
	uint64_t af:1;
	uint64_t ng:1;

	/* Output address */
	uint64_t addr:36;

	uint64_t res0:3;

	/* Upper attributes */
	uint64_t dbm:1;
	uint64_t cont:1;
	uint64_t pxn:1;
	uint64_t uxn:1;
	uint64_t ignored0:4;
	uint64_t pbha:4;
	uint64_t ignored1:1;
} lpae_page_t;


/* TBD:
 *                Level 0, 1, 2: block
 * ----------------------------------------------------
 */

/*                Level 0, 1, 2: Table
 * ----------------------------------------------------
 * bit[0]      - indicates if descriptor is valid
 * bit[1]      - descriptor type: table(1) or memory block(0)
 * bits[11:2]  - ignored
 * bits[47:12] - next-level table address [47:12]
 * bits[51:48] - RES0
 * bits[58:52] - ignored
 * bit[59]     - privileged execute-never PXNTable
 * bit[60]     - execute-never control UXNTable
 * bits[62:61] - access permissions limit for subsequent level APTable
 * bit[63]     - non-secure memory access NSTable
 */
typedef struct __packed {
	/* For valid table should be: valid = 1, type = 1 */
	uint64_t valid:1;
	uint64_t type:1;

	uint64_t ignored0:10;

	/* Next-level table address */
	uint64_t addr:36;

	uint64_t res0:4;
	uint64_t ignored1:7;

	/* Upper attributes */
	uint64_t pxn:1;
	uint64_t xn:1;
	uint64_t ap:2;
	uint64_t ns:1;
} lpae_table_t;

/* 64-bit blob for LPAE table entry */
typedef uint64_t tt_desc_t;

/* Memory management API: */
void mmu_init(void);
void mem_map(uint64_t base_addr, size_t size, uint8_t type);

/* Inline wrappers: */
static inline void mmap_io(uint64_t base_addr, size_t size)
{
	return mem_map(base_addr, size, MMAP_DEVICE);
}

static inline void mmap_text(uint64_t base_addr, size_t size)
{
	return mem_map(base_addr, size, MMAP_NORMAL);
}

#endif /* __KERNEL_MMU_H__ */
