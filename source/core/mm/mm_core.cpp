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

#include <arm64/registers>

using namespace saturn::core;

// Should be global without namespace to be visible in assembly boot code
tt_desc_t ptable_l0[_l0_size]				__align(_page_size);
tt_desc_t ptable_l1[_l1_size]				__align(_page_size);
tt_desc_t ptable_l2[_l1_size][_l2_size]			__align(_page_size);

namespace saturn {
namespace core {

// Hypervisor PA-VA data
static tt_desc_t ptable_l3[_l3_tables][_l3_size]	__align(_page_size);

// Guest PA-IPA data
static tt_desc_t ipa_ptable_l1[_l1_size]		__align(_page_size);
static tt_desc_t ipa_ptable_l2[_l1_size][_l2_size]	__align(_page_size);
static tt_desc_t ipa_ptable_l3[_l3_tables][_l3_size]	__align(_page_size);

static MemoryManagementUnit* 	Saturn_MMU = nullptr;		// MMU object pointer for hypervisor mapping
static MemoryManagementUnit* 	Guest_MMU = nullptr;		// MMU object pointer for guest mapping

void MMU_Init(void)
{
	// Create Saturn MMU
	Saturn_MMU = new MemoryManagementUnit(ptable_l1, ptable_l2, ptable_l3, MMapStage::Stage1);

	// Initial value for VTCR_EL2:
	//		  SH0_IS   | ORGN0_WBWA | IRGN0_WBWA |  SL0_L1   | T0SZ = 32 bits
	uint64_t vtcr = (3U << 12) | (1U << 10) | (1U << 8)  | (1U << 6) | (64 - 32);
	WriteArm64Reg(VTCR_EL2, vtcr);
	
	// Set IPA page tables to 0 value
	for (size_t i = 0; i < _l1_size; i++)
	{
		ipa_ptable_l1[i] = 0;

		for (size_t j = 0; j < _l2_size; j++)
		{
			ipa_ptable_l2[i][j] = 0;
		}
	}

	for (size_t i = 0; i < _l3_tables; i++)
	{
		for (size_t j = 0; j < _l3_size; j++)
		{
			ipa_ptable_l3[i][j] = 0;
		}
	}

	// Initial value for VTTBR_EL2
	// TBD: set real VMID instead of '1'
	uint64_t vttbr = reinterpret_cast<uint64_t>(&ipa_ptable_l1[0]) | (1UL << 48);
	WriteArm64Reg(VTTBR_EL2, vttbr);

	// Create guest IPA MMU
	Guest_MMU = new MemoryManagementUnit(ipa_ptable_l1, ipa_ptable_l2, ipa_ptable_l3, MMapStage::Stage2);
}

IMemoryManagementUnit& MMU(void)
{
	return *Saturn_MMU;
}

IMemoryManagementUnit& IPA_MMU(void)
{
	return *Guest_MMU;
}

}; // namespace core
}; // namespace saturn
