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
#include <core/iconsole>

using namespace saturn::core;

// Should be global without namespace to be visible in assembly boot code
tt_desc_t core_ptable_l0[_l0_size]	__align(_page_size);
tt_desc_t core_ptable_l1[_ptable_size]	__align(_page_size);
tt_desc_t core_ptable_l2[_ptable_size]	__align(_page_size);
tt_desc_t core_ptable_l3[_ptable_size]	__align(_page_size);

namespace saturn {
namespace core {

// Guest PA-IPA data
static tt_desc_t ipa_ptable_l1[_ptable_size]	__align(_page_size);

static MemoryManagementUnit* 	Saturn_MMU = nullptr;		// MMU object pointer for hypervisor mapping
static MemoryManagementUnit* 	Guest_MMU = nullptr;		// MMU object pointer for guest mapping

// External API:
void Memory_Trap_Init(void);

void MMU_Init(void)
{
	// Create Saturn MMU
	Saturn_MMU = new MemoryManagementUnit(core_ptable_l1, MMapStage::Stage1);

	// Initial value for VTCR_EL2:
	//		  SH0_IS   | ORGN0_WBWA | IRGN0_WBWA |  SL0_L1   | T0SZ = 32 bits
	uint64_t vtcr = (3U << 12) | (1U << 10) | (1U << 8)  | (1U << 6) | (64 - 32);
	WriteArm64Reg(VTCR_EL2, vtcr);
	
	// Set IPA page tables to 0 value
	for (size_t i = 0; i < _ptable_size; i++)
	{
		ipa_ptable_l1[i] = 0;
	}

	// Initial value for VTTBR_EL2
	// TBD: set real VMID instead of '1'
	uint64_t vttbr = reinterpret_cast<uint64_t>(&ipa_ptable_l1[0]) | (1UL << 48);
	WriteArm64Reg(VTTBR_EL2, vttbr);

	// Create guest IPA MMU
	Guest_MMU = new MemoryManagementUnit(ipa_ptable_l1, MMapStage::Stage2);

	// Initialize guest memory traps
	Memory_Trap_Init();

	Info() << "memory management is initialized" << fmt::endl;
}

IMemoryManagementUnit& iMMU(void)
{
	return *Saturn_MMU;
}

IMemoryManagementUnit& iMMU_VM(void)
{
	return *Guest_MMU;
}

}; // namespace core
}; // namespace saturn
