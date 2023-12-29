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
#include <bsp/ibsp>
#include <core/iconsole>
#include <core/iic>
#include <core/ivirtic>
#include <core/immu>
#include <mops>

extern "C" {
	extern void Switch_EL12(struct saturn::AArch64_Regs*);
}

extern saturn::uint64_t el2_stack_reset;

namespace saturn {

namespace apps {
	void Applications_Start();
};

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

	// Fill the configuration for Saturn virtual machines
	Load_Config();
}

VM_Manager::~VM_Manager()
{}

void VM_Manager::Load_Config(void)
{
	// Create new VM configuration
	vmConfig = new VM_Configuration;
	
	Info() << "vmm: load VM configuration" << fmt::endl;

	// Load the configuration from BSP
	bsp::iBSP().Load_VM_Configuration(*vmConfig);
}

void VM_Manager::Start_VM()
{
	if (vmConfig && (vm_state::stopped == vmState))
	{
		struct AArch64_Regs guestContext;

		MSet<uint8_t>(&guestContext, sizeof(guestContext), 0);

		guestContext.cpsr_el2 = 0x3c5;
		guestContext.pc_el2 = vmConfig->VM_Get_Entry_Address();
		guestContext.sp_el1 = vmConfig->VM_Get_Entry_Address();	// Some temporary location in VM address space

		bsp::iBSP().Prepare_OS(guestContext);

		iVirtIC().Start_Virt_IC();
		vmConfig->VM_Allocate_Resources();

		// Start virtual devices
		bsp::iBSP().Start_Virtual_Devices();

		// TBD: reset all EL1 registers:
		WriteArm64Reg(sctlr_el1, 0xc50838);	// Default value collected after cold reset

		Info() << fmt::endl << "vmm: start VM" << fmt::endl;

		vmState = vm_state::running;
		Switch_EL12(&guestContext);
	}
}

void VM_Manager::Stop_VM()
{
	if (vmConfig && (vm_state::running == vmState))
	{
		vmState = vm_state::request_shutdown;

		Raw() << fmt::endl;
		Info() << "vmm: request shutdown" << fmt::endl;
	}
	else
	if (vmConfig && (vm_state::request_shutdown == vmState))
	{
		// Stop virtual devices
		bsp::iBSP().Start_Virtual_Devices();

		vmConfig->VM_Free_Resources();
		iVirtIC().Stop_Virt_IC();

		vmState = vm_state::stopped;
		Info() << "vmm: VM stopped" << fmt::endl;

		// Reset stack to safe boot state
		asm volatile("mov sp, %0" :: "r" (el2_stack_reset));
		iIC().Local_IRq_Enable();

		apps::Applications_Start();
	}
	else
	{
		Info() << "vmm: request to shutdown non-running VM" << fmt::endl;
	}
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
