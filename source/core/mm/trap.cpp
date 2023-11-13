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

#include "trap.hpp"

#include <arm64/registers>
#include <core/iconsole>
#include <mtrap>

namespace saturn {
namespace core {

static const uint32_t _ec_abort_el1 = 0x24;			// Data Abort exception from lower Exception Level
static list_head_t Memory_Trap_List;				// List to handle trap nodes

void Memory_Trap_Init(void)
{
	List_Init(&Memory_Trap_List);
}

void Register_Trap_Region(MTrap& mt)
{
	// TBD: check if trap region to be added overlaps with
	//      already existing one
	List_Add(&mt.List, &Memory_Trap_List);
}

void Remove_Trap_Region(MTrap& mt)
{
	list_head_t *head = &Memory_Trap_List;
	list_head_t *entry = head->next;

	while (entry != head)
	{
		MTrap* node = List_Entry(entry, MTrap, List);
		if (node == &mt)
		{
			List_Del(entry);
			break;
		}

		entry = entry->next;
	}
}

static MTrap* Find_Trap_Node(uint64_t addr, uint64_t size)
{
	MTrap* node = nullptr;
	list_head_t *head = &Memory_Trap_List;
	list_head_t *entry = head->next;
	
	while (entry != head)
	{
		MTrap* mt = List_Entry(entry, MTrap, List);
		if (mt->InRange(addr, size))
		{
			node = mt;
			break;
		}

		entry = entry->next;
	}

	return node;
}

bool Do_Memory_Trap(struct AArch64_Regs* Regs)
{
	// ESR_EL2:
	//
	// 63                               37 36      32
	//  __________________________________ _________
	// |                RES0              |   ISS2  |
	// '----------------------------------'---------'
	//
	// 31     26  25  24                            0
	//  ____________________________________________
	// |   EC   | IL |              ISS             |
	// '--------'----'------------------------------'
	uint64_t esr = ReadArm64Reg(ESR_EL2);
	uint32_t ec  = (esr >> 26) & 0x3f;
	uint32_t iss = esr & 0x1ffffff;
	bool il = (esr >> 25) & 1;
	bool ret = false;

	// We care only about data abort from lower ELs
	if ((_ec_abort_el1 == ec) && il)		// TBD: only ARM64 instructions set, no Thumb mode
	{
		// ISS encoding Data Abort
		//
		//     24    23    21 20      16   15   14     13
		//  _____ _____ _____ __________ ____ ____ ______ 
		// | ISV | SAS | SSE |   SRT    | SF | AR | VNCR |
		// '-----'-----'-----'----------'----'----'------'
		//
		//  12 11    10    9    8       7     6 5       0
		//  _____ _____ ____ ____ _______ _____ _________
		// | LST | FnV | EA | CM | S1PTW | WnR |   DFSC  |
		// '-----'-----'----'----'-------'-----'---------'

		if (
		    (((iss >> 24) & 1) == 1) &&		// ISS holds valid instruction syndrom
		    (((iss >> 10) & 1) == 0)		// FAR register contains valid address
		   )
		{
			uint8_t sas = (iss >> 22) & 0x3;
			uint8_t wnr = (iss >> 6) & 0x1;
			uint8_t srt = (iss >> 16) & 0x1f;
			uint64_t far = ReadArm64Reg(FAR_EL2);
			uint64_t* reg = &Regs->x0;
			uint64_t pa = va_to_pa_el1(far);

			reg += srt;

			MTrap* mt = Find_Trap_Node(pa, sas + 1);

			if (mt != nullptr)
			{
				uint64_t offset = pa - mt->GetBase();

				if (wnr == 0)
				{
					mt->VDrv.Read(offset, reg, static_cast<AccessSize>(sas));
				}
				else
				{
					mt->VDrv.Write(offset, reg, static_cast<AccessSize>(sas));
				}

				ret = true;
			}
		}
	}

	return ret;
}

}; // namespace core
}; // namespace saturn
