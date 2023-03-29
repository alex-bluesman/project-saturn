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
#include "ttable.hpp"

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

bool MemoryManagementUnit::Map_L3_Page(uint64_t virt_addr, MMapType type)
{
	uint16_t index;
	lpae_table_t *pt, *entry;
	lpae_page_t *page;
	bool ret = true;

	entry = Map_L1_PTable(virt_addr);
	if (nullptr != entry)
	{
		entry = Map_L2_PTable(virt_addr);
		if (nullptr != entry)
		{
			// L3 page table
			uint64_t pt_addr = static_cast<uint64_t>(entry->addr << 12U);
			pt = reinterpret_cast<lpae_table_t *>(pt_addr);

			// L3 table entry
			index = (virt_addr >> _l3_addr_shift) & _ptable_size_mask;
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
					break;
				}

				page->addr = virt_addr >> 12;
			}
		}
		else
		{
			Error() << "level-2 mapping failed for address: 0x" << fmt::fill << fmt::hex << virt_addr << fmt::endl;
			ret = false;
		}
	}
	else
	{
		Error() << "level-1 mapping failed for address: 0x" << fmt::fill << fmt::hex << virt_addr << fmt::endl;
		ret = false;
	}

	return ret;
}

lpae_table_t* MemoryManagementUnit::Map_L1_PTable(uint64_t virt_addr)
{
	size_t index;
	lpae_table_t* pt;
	lpae_table_t* entry = nullptr;

	index = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;

	if (index < _l1_size)
	{
		entry = reinterpret_cast<lpae_table_t *>(&PTable1[index]);

		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			entry->valid = 1;
			entry->type = LPAE_Type::Table;
			entry->addr = reinterpret_cast<uint64_t>(PTable2[index])>> 12;
		}
		else
		{
			// Entry is already mapped as memory block
			if (entry->type == LPAE_Type::Block)
			{
				entry = nullptr;
			}
		}
	}

	return entry;
}

lpae_table_t* MemoryManagementUnit::Map_L2_PTable(uint64_t virt_addr)
{
	size_t index;
	lpae_table_t* pt;
	lpae_table_t* entry = nullptr;

	// L2 page table
	// NOTE: no need to check index because it's already checked by L1 mapping
	index = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;
	pt = reinterpret_cast<lpae_table_t *>(&PTable2[index]);

	// L2 table entry
	index = (virt_addr >> _l2_addr_shift) & _ptable_size_mask;
	entry = reinterpret_cast<lpae_table_t *>(&pt[index]);

	if (entry->valid == 0)
	{
		index = FindFreeL3();

		entry->valid = 1;
		entry->type = LPAE_Type::Table;
		entry->addr = ((uint64_t)&PTable3[index]) >> 12;
	}
	else
	{
		if (entry->type == LPAE_Type::Block)
		{
			entry = nullptr;
		}
	}

	return entry;
}

bool MemoryManagementUnit::Map_Memory_Block(uint64_t base_addr, BlockSize size)
{
	bool ret = true;

	// Check if address is properly aligned
	if (base_addr & (size - 1) != 0)
	{
		ret = false;
	}
	else
	{
		switch (size)
		{
		case BlockSize::L1_Block:
			break;
		
		case BlockSize::L2_Block:
			break;

		case BlockSize::L3_Page:
			break;

		default:
			break;
		}
	}

	return ret;
}

void MemoryManagementUnit::MemoryMap(uint64_t base_addr, size_t size, MMapType type)
{
	uint64_t start = base_addr & _page_base;
	uint64_t end = (base_addr + size) & _page_base;

	do
	{
		Map_L3_Page(start, type);
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
