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

#include "mmu.hpp"

// TBD: rework it
#include <asm/mmu.h>
#include <fault>

using namespace saturn::core;

// Should be global without namespace to be visible in assembly boot code
tt_desc_t ptable_l0[_l0_size]			__align(_page_size);
tt_desc_t ptable_l1[_l1_size]			__align(_page_size);
tt_desc_t ptable_l2[_l1_size][_l2_size]		__align(_page_size);

namespace saturn {
namespace core {

// Number of tables for hypervisor level-3 page mapping. Hypervisor uses them
// to map I/O peripheral regions. And due to only certain devices are used
// (like UART and GIC), no need to create many tables
static const size_t _l3_tables = 16;
static tt_desc_t ptable_l3[_l3_tables][_l3_size]	__align(_page_size);

MemoryManagementUnit::MemoryManagementUnit()
	: PTable0(ptable_l0)
	, PTable1(ptable_l1)
	, PTable2(ptable_l2)
	, PTable3(ptable_l3)
	, FreeMaskL3(0)
{
	Info() << "memory management unit initialized" << fmt::endl;
}

uint16_t MemoryManagementUnit::FindFreeL3(void)
{
	uint64_t i;

	for (i = 0; i < _l3_tables; i++)
	{
		if (((FreeMaskL3 >> i) & 1) == 0)
		{
			FreeMaskL3 |= 1U << i;
			break;
		}
	}

	if (i == _l3_tables)
	{
		Fault("mmu: no free L3 tables left");
	}

	return i;
}

void MemoryManagementUnit::MapPage(uint64_t base_addr, MMapType type)
{
	uint16_t index;
	lpae_table_t *pt, *entry;
	lpae_page_t *page;

	index = (base_addr >> L1_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)&PTable1;
	entry = (lpae_table_t *)(void *)&pt[index];

	// L1 -> L2
	if (entry->valid == 0)
	{
		entry->valid = 1;
		entry->type = 1;
		entry->addr = ((uint64_t)PTable2[index]) >> 12;
	}
	else
	{
		if (entry->type == 0)
		{
			// L1 entry maps 1GB memory block
			return;
		}
	}

	index = (base_addr >> L2_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)(uint64_t)(entry->addr << 12U);
	entry = &pt[index];

	// L2 -> L3
	if (entry->valid == 0)
	{
		index = FindFreeL3();

		entry->valid = 1;
		entry->type = 1;
		entry->addr = ((uint64_t)&PTable3[index]) >> 12;
	}
	else
	{
		if (entry->type == 0)
		{
			// L2 entry maps 2MB memory block
			return;
		}
	}

	index = (base_addr >> L3_ADDR_SHIFT) & TABLE_INDEX_MASK;
	pt = (lpae_table_t *)(uint64_t)(entry->addr << 12U);
	page = (lpae_page_t *)&pt[index];

	// Page -> Address
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
		case MMapType::Device:
			page->attr = 4;
			page->sh = 2;
			break;
		case MMapType::Normal:
			page->attr = 7;
			page->sh = 3;
			break;
		default:
			// TBD: error here
			break;
		}

		page->addr = base_addr >> 12;
	}
}

void MemoryManagementUnit::MemoryMap(uint64_t base_addr, size_t size, MMapType type)
{
	uint64_t start = base_addr & _page_base;
	uint64_t end = (base_addr + size) & _page_base;

	do
	{
		MapPage(start, type);
		start += _page_size;
	}
	while (start < end);

	// TBD: update TLB and data caches here
//	asm volatile("\
//			dsb	ishst;	\
//			tlbi	alle2;	\
//			dsb	ish;	\
//			isb;		\
//	             " : : : "memory");
}

void MemoryManagementUnit::MemoryUnmap(uint64_t base_addr, size_t size)
{
	// TBD: implement free
}

}; // namespace core
}; // namespace saturn
