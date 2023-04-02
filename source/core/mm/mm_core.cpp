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

using namespace saturn::core;

// Should be global without namespace to be visible in assembly boot code
tt_desc_t ptable_l0[_l0_size]				__align(_page_size);
tt_desc_t ptable_l1[_l1_size]				__align(_page_size);
tt_desc_t ptable_l2[_l1_size][_l2_size]			__align(_page_size);

namespace saturn {
namespace core {

static tt_desc_t ptable_l3[_l3_tables][_l3_size]	__align(_page_size);

static MemoryManagementUnit* 	Saturn_MMU = nullptr;		// MMU object pointer to implement I/O mapping

void MMU_Init(void)
{
	// Create MMU object
	Saturn_MMU = new MemoryManagementUnit(ptable_l0, ptable_l1, ptable_l2, ptable_l3);

}

IMemoryManagementUnit& MMU(void)
{
	return *Saturn_MMU;
}


}; // namespace core
}; // namespace saturn
