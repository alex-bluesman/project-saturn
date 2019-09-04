#include <asm/mmu.h>
#include <console.h>
#include <mmu.h>
#include <sys.h>
#include <types.h>

/* Number of tables for hypervisor level-3 page mapping. Hypervisor uses them
 * to map I/O peripheral regions. And due to only certain devices are used
 * (like UART and GIC), no need to create many tables.
 */
#define L3_TABLES	16

tt_desc_t ptable_l0[L0_SIZE]		__align(PAGE_SIZE);
tt_desc_t ptable_l1[L1_SIZE]		__align(PAGE_SIZE);
tt_desc_t ptable_l2[L1_SIZE][L2_SIZE]	__align(PAGE_SIZE);
tt_desc_t ptable_l3[L3_TABLES][L3_SIZE]	__align(PAGE_SIZE);

/* This mask will be used to manage validity of level-3 tables */
static uint64_t l3_valid_mask;

static uint16_t find_free_l3_table(void)
{
	uint64_t i;

	for (i = 0; i < L3_TABLES; i++)
	{
		if (((l3_valid_mask >> i) & 1) == 0)
		{
			l3_valid_mask |= 1U << i;
			break;
		}
	}

	if (i == L3_TABLES)
	{
		/* TODO: die here */
	}

	return i;
}

static void map_page(uint64_t base_addr, uint8_t type)
{
	uint16_t index;
	lpae_table_t *pt, *entry;
	lpae_page_t *page;

	index = (base_addr >> L1_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)&ptable_l1;
	entry = (void *)&pt[index];

	/* Map L1 -> L2 */
	if (entry->valid == 0)
	{
		entry->valid = 1;
		entry->type = 1;
		entry->addr = ((uint64_t)ptable_l2[index]) >> 12;
	}
	else
	{
		if (entry->type == 0)
		{
			/* L1 entry maps 1GB memory block */
			return;
		}
	}

	index = (base_addr >> L2_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)(uint64_t)(entry->addr << 12U);
	entry = &pt[index];

	/* L2 -> L3 */
	if (entry->valid == 0)
	{
		index = find_free_l3_table();

		entry->valid = 1;
		entry->type = 1;
		entry->addr = ((uint64_t)&ptable_l3[index]) >> 12;
	}
	else
	{
		if (entry->type == 0)
		{
			/* L2 entry maps 2MB memory block */
			return;
		}
	}

	index = (base_addr >> L3_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)(uint64_t)(entry->addr << 12U);
	page = (lpae_page_t *)&pt[index];

	/* Page -> address */
	if (page->valid == 0)
	{
		page->valid = 1;
		page->type = 1;
		page->ns = 1;
		page->ap = 1;
		page->af = 1;
		page->ng = 1;

		switch (type)
		{
		case MMAP_DEVICE:
			page->attr = 4;
			page->sh = 2;
			break;
		case MMAP_NORMAL:
			page->attr = 7;
			page->sh = 3;
			break;
		default:
			/* TODO: error here */
			break;
		}

		page->addr = base_addr >> 12;
	}
}

void mem_map(uint64_t base_addr, size_t size, uint8_t type)
{
	uint64_t start = base_addr & PAGE_BASE_ADDR;
	uint64_t end = (base_addr + size) & PAGE_BASE_ADDR;

	do
	{
		map_page(start, type);
		start += PAGE_SIZE;
	}
	while (start < end);

	/* TODO: update TLB and data caches here */
}

void mmu_init(void)
{
}
