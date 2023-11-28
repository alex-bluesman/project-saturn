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

#include "mmu.hpp"

#include <arm64/registers>
#include <basetypes>

namespace saturn {
namespace core {

// External API:
bool Do_Memory_Trap(struct AArch64_Regs* Regs);

static inline uint64_t va_to_pa_el1(uint64_t va)
{
	uint64_t par;
	uint64_t guest_par = ReadArm64Reg(PAR_EL1);

	asm volatile ("at s1e1r, %0;" : : "r" (va));
	asm volatile("isb" : : : "memory");
	par = ReadArm64Reg(PAR_EL1);

	WriteArm64Reg(PAR_EL1, guest_par);

	par &= _page_base;
	par &= (1UL << 52) - 1;
	par += (va & _page_mask);
	return par;
}

}; // namespace core
}; // namespace saturn
