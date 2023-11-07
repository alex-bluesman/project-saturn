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

#include "vm_manager.hpp"

#include <arm64/registers>
#include <core/iconsole>
#include <core/ivirtic>
#include <core/immu>
#include <mops>

extern saturn::uint64_t* boot_stack;

extern "C" {
	extern void Switch_EL12(struct saturn::AArch64_Regs*);
}

namespace saturn {
namespace core {

VM_Manager::VM_Manager()
	: vmState(vm_state::stopped)
	, vmConfig(nullptr)
{
	// Enable Stage-2 translation for EL1&0 and use AArch64 mode
	uint64_t hcr = ReadArm64Reg(HCR_EL2);
	hcr |= (1 << 31) | (1 << 0);	// RW,bit[31] | VM,bit[0]
	WriteArm64Reg(HCR_EL2, hcr);

	Info() << "VM manager started" << fmt::endl;

	// TBD: should be some external configuration
	Load_Config(OS_Type::Default);
}

VM_Manager::~VM_Manager()
{}

void VM_Manager::Load_Config(OS_Type type)
{
	// Create new VM configuration
	vmConfig = new VM_Configuration;
	
	if (type == OS_Type::Linux)
	{
		// TBD
	}
	else
	{
		Info() << "vmm: load default configuration" << fmt::endl;
		vmConfig->VM_Add_Memory_Region({0x08010000, 0x08040000, 0x00010000, MMapType::Device});
		vmConfig->VM_Add_Memory_Region({0x41000000, 0x41000000, BlockSize::L2_Block, MMapType::Normal});
		vmConfig->VM_Set_Guest_OS(OS_Type::Default);
		vmConfig->VM_Set_Entry(0x41000000);
	}
}

void VM_Manager::Start_VM()
{
	if (vmConfig)
	{
		struct AArch64_Regs guestContext;

		MSet<uint8_t>(&guestContext, sizeof(guestContext), 0);

		guestContext.cpsr_el2 = 0x3c5;
		guestContext.pc_el2 = vmConfig->osEntry;
		guestContext.sp_el1 = vmConfig->osEntry;	// TBD: some temporary location in VM address space

		// The following registers to be checked:
		// MPIDR_EL1, SCTLR_EL1

		iVirtIC().Start_Virt_IC();
		vmConfig->VM_Map_All();

		vmState = vm_state::running;
		Switch_EL12(&guestContext);
	}
}

void VM_Manager::Stop_VM()
{
	iVirtIC().Stop_Virt_IC();

	struct AArch64_Regs saturnContext;

	// TBD: fill properly context to return to Saturn
	
	vmState = vm_state::stopped;
	Switch_EL12(&saturnContext);
}

vm_state VM_Manager::Get_VM_State()
{
	return vmState;
}

bool VM_Manager::Guest_IRq(uint32_t nr)
{
	return vmConfig->VM_Own_Interrupt(nr);
}

}; // namespace core
}; // namespace saturn
