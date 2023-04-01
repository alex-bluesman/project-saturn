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

lpae_block_t* MemoryManagementUnit::Map_L1_Block(uint64_t virt_addr, uint64_t phys_addr,  MMapType type)
{
	size_t index;
	lpae_table_t* pt;
	lpae_block_t* entry = nullptr;

	index = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;

	if (index < _l1_size)
	{
		entry = reinterpret_cast<lpae_block_t *>(&PTable1[index]);

		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			entry->valid = 1;
			entry->type = LPAE_Type::Block;
			entry->addr = phys_addr >> 30;
			entry->ns = 1;
			entry->ap = 1;
			entry->af = 1;
			entry->ng = 1;

			switch (type)
			{
			case MMapType::Device:
				entry->attr = 4;
				entry->sh = 2;
				break;
			case MMapType::Normal:
				entry->attr = 7;
				entry->sh = 3;
				break;
			default:
				break;
			}

			Log() << "mm: PTable1[" << index << "] -> 1GB block for address 0x"
			      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
		}
		else
		{
			// Entry is already mapped as page table
			if (entry->type == LPAE_Type::Table)
			{
				entry = nullptr;
			}
		}
	}

	return entry;
}

lpae_block_t* MemoryManagementUnit::Map_L2_Block(uint64_t virt_addr, uint64_t phys_addr, MMapType type)
{
	size_t index;
	lpae_table_t* pt;
	lpae_block_t* entry = nullptr;

	pt = Map_L1_PTable(virt_addr);
	if (nullptr != pt)
	{
		// L2 page table
		uint64_t pt_addr = static_cast<uint64_t>(pt->addr << 12U);
		pt = reinterpret_cast<lpae_table_t *>(pt_addr);

		// L2 table entry
		index = (virt_addr >> _l2_addr_shift) & _ptable_size_mask;
		entry = (lpae_block_t *)&pt[index];


		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			entry->valid = 1;
			entry->type = LPAE_Type::Block;
			entry->addr = phys_addr >> 21;
			entry->ns = 1;
			entry->ap = 1;
			entry->af = 1;
			entry->ng = 1;

			switch (type)
			{
			case MMapType::Device:
				entry->attr = 4;
				entry->sh = 2;
				break;
			case MMapType::Normal:
				entry->attr = 7;
				entry->sh = 3;
				break;
			default:
				break;
			}

			Log() << "mm:   PTable2[][" << index << "] -> 2MB block for address 0x"
			      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
		}
		else
		{
			// Entry is already mapped as page table
			if (entry->type == LPAE_Type::Table)
			{
				entry = nullptr;
			}
		}
	}

	return entry;
}

lpae_page_t* MemoryManagementUnit::Map_L3_Page(uint64_t virt_addr, uint64_t phys_addr, MMapType type)
{
	size_t index;
	lpae_table_t *pt, *entry;
	lpae_page_t *page = nullptr;

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
				page->type = LPAE_Type::Page;
				page->addr = phys_addr >> 12;
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
			}

			Log() << "mm:     PTable3[][" << index << "] -> 4KB page for address 0x"
			      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
		}
		else
		{
			Error() << "level-2 mapping failed for address: 0x" << fmt::fill << fmt::hex << virt_addr << fmt::endl;
		}
	}
	else
	{
		Error() << "level-1 mapping failed for address: 0x" << fmt::fill << fmt::hex << virt_addr << fmt::endl;
	}

	return page;
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

			Log() << "mm: PTable1[" << index << "] -> PTable2[" << index << "][] for address 0x"
			      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
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
	lpae_table_t *pt, *entry;

	// L2 page table
	// NOTE: no need to check index because it's already checked by L1 mapping
	size_t l1  = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;
	pt = reinterpret_cast<lpae_table_t *>(&PTable2[l1]);

	// L2 table entry
	size_t l2 = (virt_addr >> _l2_addr_shift) & _ptable_size_mask;
	entry = reinterpret_cast<lpae_table_t *>(&pt[l2]);

	if (entry->valid == 0)
	{
		size_t index = FindFreeL3();

		entry->valid = 1;
		entry->type = LPAE_Type::Table;
		entry->addr = ((uint64_t)&PTable3[index]) >> 12;

		Log() << "mm:   PTable2[" << l1 << "][" << l2 << "] -> PTable3[" << index << "] for address 0x"
		      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
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

void* MemoryManagementUnit::MemoryMap(uint64_t virt_addr, uint64_t phys_addr, size_t size, MMapType type)
{
	uint64_t start = virt_addr & _page_base;
	uint64_t end = (virt_addr + size) & _page_base;
	uint64_t pa = phys_addr & _page_base;

	void* ret = reinterpret_cast<void*>(start);

	do
	{
		if (((start & _l1_block_mask) == 0) &&	// virtual address is 1GB aligned
		    ((pa & _l1_block_mask) == 0) &&
		    ((end - start) >= BlockSize::L1_Block))
		{
			if (nullptr == Map_L1_Block(start, pa, type))
			{
				ret = nullptr;
				break;
			}

			start += BlockSize::L1_Block;
			pa += BlockSize::L1_Block;
		}
		else
		if (((start & _l2_block_mask) == 0) &&	// virtual address is 2MB aligned
		    ((pa & _l2_block_mask) == 0) &&
		    ((end - start) >= BlockSize::L2_Block))
		{
			if (nullptr == Map_L2_Block(start, pa, type))
			{
				ret = nullptr;
				break;
			}

			start += BlockSize::L2_Block;
			pa += BlockSize::L2_Block;
		}
		else					// then just map a page
		{
			if (nullptr == Map_L3_Page(start, pa, type))
			{
				ret = nullptr;
				break;
			}

			start += BlockSize::L3_Page;
			pa += BlockSize::L3_Page;
		}
	}
	while (start < end);

	// TBD: update TLB and data caches here
//	asm volatile("\
//			dsb	ishst;	\
//			tlbi	alle2;	\
//			dsb	ish;	\
//			isb;		\
//	             " : : : "memory");

	if (nullptr == ret)
	{
		// TBD: does it make any sense to switch to Fault Mode?
		Fault("invalid MMU mapping, please check your configuration");
	}

	return ret;
}

void MemoryManagementUnit::MemoryUnmap(uint64_t base_addr, size_t size)
{
	// TBD: implement free
}

}; // namespace core
}; // namespace saturn