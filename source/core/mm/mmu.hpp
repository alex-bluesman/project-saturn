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

#pragma once

#include "config.hpp"
#include "ttable.hpp"

#include <core/immu>

namespace saturn {
namespace core {

// Table could have only 512 entries
static const size_t _ptable_size_mask = 0x1ff;

// L1 page definitions:
static const size_t _l1_block_mask = (BlockSize::L1_Block - 1);

// L2 page definitions:
static const size_t _l2_block_mask = (BlockSize::L2_Block - 1);

// L3 page definitions:
static const size_t _page_size = BlockSize::L3_Page;
static const size_t _page_mask = (_page_size - 1);
static const size_t _page_base = (~_page_mask);

// 64-bit blob for LPAE table entry
using tt_desc_t = uint64_t;

// Memory mapping stages:
enum MMapStage
{
	Stage1 = 0,	// Hyp mode
	Stage2 = 2	// Sys mode (IPA)
};

class MemoryManagementUnit : public IMemoryManagementUnit {
public:
	MemoryManagementUnit(tt_desc_t (&Level1)[],
			     MMapStage Stage = MMapStage::Stage1
			    );

public:
	inline void* MemoryMap(Memory_Region& region);
	void* MemoryMap(uint64_t virt_addr, uint64_t phys_addr, size_t size, MMapType type);
	inline void MemoryUnmap(Memory_Region& region);
	void MemoryUnmap(uint64_t base_addr, size_t size);

private:
	inline void* Get_Table(void);
	inline void Free_Table(void* ptable);
	void Fill_Mem_Attrs(lpae_block_t* entry, MMapType type);
	inline void TLB_Flush_All(void);
	void Free_Empty_Tables(void);

private:
	lpae_table_t* Map_L1_PTable(uint64_t virt_addr);
	lpae_table_t* Map_L2_PTable(uint64_t virt_addr);

	lpae_block_t* Map_L1_Block(uint64_t virt_addr, uint64_t phys_addr, MMapType type);
	lpae_block_t* Map_L2_Block(uint64_t virt_addr, uint64_t phys_addr, MMapType type);
	lpae_page_t*  Map_L3_Page(uint64_t virt_addr, uint64_t phys_addr, MMapType type);

private:
	MMapStage TStage;
	tt_desc_t (&PTable1)[];
};

}; // namespace core
}; // namespace saturn
