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
#include <fault>
#include <mops>

namespace saturn {
namespace core {

MemoryManagementUnit::MemoryManagementUnit(tt_desc_t (&Level1)[], MMapStage Stage)
	: PTable1(Level1)
	, TStage(Stage)
{}

void* MemoryManagementUnit::Get_Table(void)
{
	void* ptable = new tt_desc_t[_ptable_size];

	if (ptable)
	{
		MSet<uint64_t>(ptable, _ptable_size, 0);
	}

	return ptable;
}

void MemoryManagementUnit::Free_Table(void* ptable)
{
	if (ptable)
	{
		// NOTE: we assume that table does not contain any valid entry, this should be guaranteed by caller
		MSet<uint64_t>(ptable, _ptable_size, 0);
		delete [] static_cast<tt_desc_t*>(ptable);
	}
}

void MemoryManagementUnit::Fill_Mem_Attrs(lpae_block_t* entry, MMapType type)
{
	entry->ns = 1;
	entry->ap = 1 | TStage; // R/W: EL2 AP[2:1] = b01, EL1 S2AP[1:0] = b11
	entry->af = 1;
	entry->ng = 1;

	switch (type)
	{
	case MMapType::Device:
		entry->attr = 1;
		entry->sh = 2;
		break;
	case MMapType::Normal:
		entry->attr = 7;
		entry->sh = 2;
		break;
	default:
		break;
	}
}

lpae_block_t* MemoryManagementUnit::Map_L1_Block(uint64_t virt_addr, uint64_t phys_addr,  MMapType type)
{
	lpae_block_t* entry = nullptr;
	size_t index = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;

	if (index < _l1_size)
	{
		entry = reinterpret_cast<lpae_block_t *>(&PTable1[index]);

		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			entry->valid = 1;
			entry->type = LPAE_Type::Block;
			entry->addr = phys_addr >> 30;

			Fill_Mem_Attrs(entry, type);

			Log() << "mm: PTable1[] -> 1GB block for address 0x"
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
	lpae_block_t* entry = nullptr;

	lpae_table_t* pt = Map_L1_PTable(virt_addr);
	if (nullptr != pt)
	{
		// L2 page table
		uint64_t pt_addr = static_cast<uint64_t>(pt->addr << 12U);
		pt = reinterpret_cast<lpae_table_t *>(pt_addr);

		// L2 table entry
		size_t index = (virt_addr >> _l2_addr_shift) & _ptable_size_mask;
		entry = reinterpret_cast<lpae_block_t*>(&pt[index]);

		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			entry->valid = 1;
			entry->type = LPAE_Type::Block;
			entry->addr = phys_addr >> 21;

			Fill_Mem_Attrs(entry, type);

			Log() << "mm:   PTable2[] -> 2MB block for address 0x"
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
	lpae_page_t *page = nullptr;

	lpae_table_t* pt = Map_L1_PTable(virt_addr);
	if (nullptr != pt)
	{
		pt = Map_L2_PTable(virt_addr);
		if (nullptr != pt)
		{
			// L3 page table
			uint64_t pt_addr = static_cast<uint64_t>(pt->addr << 12U);
			pt = reinterpret_cast<lpae_table_t *>(pt_addr);

			// L3 table entry
			size_t index = (virt_addr >> _l3_addr_shift) & _ptable_size_mask;
			page = reinterpret_cast<lpae_page_t*>(&pt[index]);

			// Page -> Address
			if (page->valid == 0)
			{
				page->valid = 1;
				page->type = LPAE_Type::Page;
				page->addr = phys_addr >> 12;

				Fill_Mem_Attrs(reinterpret_cast<lpae_block_t*>(page), type);

				Log() << "mm:     PTable3[] -> 4KB page for address 0x"
				      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
			}
		}
	}

	return page;
}

lpae_table_t* MemoryManagementUnit::Map_L1_PTable(uint64_t virt_addr)
{
	lpae_table_t* entry = nullptr;

	size_t index = (virt_addr >> _l1_addr_shift) & _ptable_size_mask;
	if (index < _l1_size)
	{
		entry = reinterpret_cast<lpae_table_t *>(&PTable1[index]);

		// Check if the entry is not already mapped
		if (entry->valid == 0)
		{
			void* ptable = Get_Table();
			if (ptable)
			{
				entry->valid = 1;
				entry->type = LPAE_Type::Table;
				entry->addr = ((uint64_t)ptable) >> 12;

				Log() << "mm: PTable1[] -> PTable2[] for address 0x"
				      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
			}
			else
			{
				entry = nullptr;
			}
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
	lpae_table_t *entry = nullptr;

	lpae_table_t* pt = Map_L1_PTable(virt_addr);
	if (nullptr != pt)
	{
		// L2 page table
		uint64_t pt_addr = static_cast<uint64_t>(pt->addr << 12U);
		pt = reinterpret_cast<lpae_table_t *>(pt_addr);

		// L2 table entry
		size_t index = (virt_addr >> _l2_addr_shift) & _ptable_size_mask;
		entry = reinterpret_cast<lpae_table_t *>(&pt[index]);

		if (entry->valid == 0)
		{
			void* ptable = Get_Table();
			if (ptable)
			{
				entry->valid = 1;
				entry->type = LPAE_Type::Table;
				entry->addr = ((uint64_t)ptable) >> 12;

				Log() << "mm:   PTable2[] -> PTable3[] for address 0x"
				      << fmt::hex << fmt::fill << virt_addr << fmt::endl;
			}
			else
			{
				entry = nullptr;
			}
		}
		else
		{
			// Entry is already mapped as page table
			if (entry->type == LPAE_Type::Block)
			{
				entry = nullptr;
			}
		}
	}

	return entry;
}

void MemoryManagementUnit::TLB_Flush_All(void)
{
	asm volatile("\
			dsb	nshst;		\
			tlbi	vmalle1;	\
			dsb	nsh;		\
			isb;			\
		     " : : : "memory");
}

void* MemoryManagementUnit::MemoryMap(Memory_Region& region)
{
	return MemoryMap(region.VA, region.PA, region.Size, region.Type);
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

	TLB_Flush_All();

	if (nullptr == ret)
	{
		// TBD: does it make any sense to switch to Fault Mode?
		Fault("invalid MMU mapping, please check your configuration");
	}

	return ret;
}

void MemoryManagementUnit::MemoryUnmap(Memory_Region& region)
{
	return MemoryUnmap(region.VA, region.Size);
}

void MemoryManagementUnit::MemoryUnmap(uint64_t virt_addr, size_t size)
{
	uint64_t start = virt_addr & _page_base;
	uint64_t end = (virt_addr + size) & _page_base;

	do
	{
		size_t index = (start >> _l1_addr_shift) & _ptable_size_mask;
		lpae_table_t* entry = reinterpret_cast<lpae_table_t *>(&PTable1[index]);

		// L1 block
		if ((entry->valid == 1) && (entry->type == LPAE_Type::Block))
		{
			void* ptr = reinterpret_cast<void*>(entry);
			MSet<uint64_t>(ptr, 1, 0);

			Log() << "mm: PTable1[] -> free 1GB block for address 0x"
			      << fmt::hex << fmt::fill << start << fmt::endl;

			start += BlockSize::L1_Block;
		}
		else
		// L1 page table
		if ((entry->valid == 1) && (entry->type == LPAE_Type::Table))
		{
			uint64_t pt_addr = static_cast<uint64_t>(entry->addr << 12U);
			lpae_table_t* pt = reinterpret_cast<lpae_table_t *>(pt_addr);

			index = (start >> _l2_addr_shift) & _ptable_size_mask;
			entry = reinterpret_cast<lpae_table_t *>(&pt[index]);

			// L2 block
			if ((entry->valid == 1) && (entry->type == LPAE_Type::Block))
			{
				void* ptr = reinterpret_cast<void*>(entry);
				MSet<uint64_t>(ptr, 1, 0);

				Log() << "mm:   PTable2[] -> free 2MB block for address 0x"
				      << fmt::hex << fmt::fill << start << fmt::endl;

				start += BlockSize::L2_Block;
			}
			else
			// L2 page table
			if ((entry->valid == 1) && (entry->type == LPAE_Type::Table))
			{
				// L3 page table
				pt_addr = static_cast<uint64_t>(entry->addr << 12U);
				pt = reinterpret_cast<lpae_table_t *>(pt_addr);

				index = (start >> _l3_addr_shift) & _ptable_size_mask;
				lpae_page_t* page = reinterpret_cast<lpae_page_t*>(&pt[index]);

				// L3 page
				if (page->valid == 1)
				{
					void* ptr = reinterpret_cast<void*>(page);
					MSet<uint64_t>(ptr, 1, 0);

					Log() << "mm:     PTable3[] -> free 4KB page for address 0x"
					      << fmt::hex << fmt::fill << start << fmt::endl;

					start += BlockSize::L3_Page;
				}
				else
				{
					Fault("attempt to free non-mapped level 3, please check your configuration");
				}
			}
			else
			{
				Fault("attempt to free non-mapped level 2, please check your configuration");
			}
		}
		else
		{
			Fault("attempt to free non-mapped level 1, please check your configuration");
		}
	}
	while (start < end);

	Free_Empty_Tables();

	TLB_Flush_All();
}

void MemoryManagementUnit::Free_Empty_Tables(void)
{
	for (size_t l1 = 0; l1 < _l1_size; ++l1)
	{
		lpae_table_t* entry_l1 = reinterpret_cast<lpae_table_t *>(&PTable1[l1]);

		if ((entry_l1->valid == 1) && (entry_l1->type == LPAE_Type::Table))
		{
			uint64_t pt_addr = static_cast<uint64_t>(entry_l1->addr << 12U);
			lpae_table_t* pt2 = reinterpret_cast<lpae_table_t *>(pt_addr);
			bool empty_l2 = true;

			for (size_t l2 = 0; l2 < _ptable_size; ++l2)
			{
				lpae_table_t* entry_l2 = reinterpret_cast<lpae_table_t *>(&pt2[l2]);

				if ((entry_l2->valid == 1) && (entry_l2->type == LPAE_Type::Block))
				{
					empty_l2 = false;
					break;
				}
				else
				if ((entry_l2->valid == 1) && (entry_l2->type == LPAE_Type::Table))
				{
					pt_addr = static_cast<uint64_t>(entry_l2->addr << 12U);
					lpae_table_t* pt3 = reinterpret_cast<lpae_table_t *>(pt_addr);
					bool empty_l3 = true;

					for (size_t l3 = 0; l3 < _ptable_size; ++l3)
					{
						lpae_page_t* entry_l3 = reinterpret_cast<lpae_page_t *>(&pt3[l3]);

						if (entry_l3->valid == 1)
						{
							empty_l3 = false;
							break;
						}
					}

					if (empty_l3)
					{
						Log() << "mm:   PTable2[] -> free PTable3[]" << fmt::endl;
						void* ptr = reinterpret_cast<void*>(entry_l2);
						MSet<uint64_t>(ptr, 1, 0);
						Free_Table(pt3);
					}
					else
					{
						empty_l2 = false;
					}
				}
			}

			if (empty_l2)
			{
				Log() << "mm: PTable1[] -> free PTable2[]" << fmt::endl;
				void* ptr = reinterpret_cast<void*>(entry_l1);
				MSet<uint64_t>(ptr, 1, 0);
				Free_Table(pt2);
			}
		}
	}
}

}; // namespace core
}; // namespace saturn
