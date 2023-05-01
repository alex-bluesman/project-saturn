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

#include <arm64/registers>
#include <core/iconsole>
#include <core/immu>
#include <mset>

extern saturn::uint64_t* boot_stack;

extern "C" {
	extern void Switch_EL12(struct saturn::AArch64_Regs*);
	extern void Parking_Lot_EL1();
}

namespace saturn {
namespace core {

IMemoryManagementUnit& IPA_MMU(void);

// TBD: introduce configuration interface to avoid hardcoding
namespace config {
	static const uint64_t _guest_pa = 0x41000000;
	static const uint64_t _guest_ipa = 0x40000000;
}; // namespace config

void VM_Start(void)
{
	struct AArch64_Regs guestContext;

	MSet<uint8_t>(&guestContext, sizeof(guestContext), 0);

	guestContext.cpsr_el2 = 0x3c5;
	guestContext.pc_el2 = config::_guest_ipa;

	guestContext.sp_el1 = config::_guest_ipa + BlockSize::L3_Page;	// Some temporary location in VM address space

	// The following registers to be checked:
	// MPIDR_EL1, SCTLR_EL1
	
	IPA_MMU().MemoryMap(config::_guest_ipa, config::_guest_pa, BlockSize::L2_Block, MMapType::Normal);

	Switch_EL12(&guestContext);
}

static void Stop_VM(void)
{
	struct AArch64_Regs saturnContext;

	// TBD: fill properly context to return to Saturn
	
	Switch_EL12(&saturnContext);
}

void Start_VM_Manager(void)
{
	// Enable Stage-2 translation for EL1&0 and use AArch64 mode
	uint64_t hcr = ReadArm64Reg(HCR_EL2);
	hcr |= (1 << 31) | (1 << 0);	// RW,bit[31] | VM,bit[0]
	WriteArm64Reg(HCR_EL2, hcr);

	Info() << "VM manager started" << fmt::endl;
}

}; // namespace core
}; // namespace saturn
