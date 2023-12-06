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

#include "ic/ic_core.hpp"
#include "mm/trap.hpp"

#include <arm64/registers>
#include <core/iconsole>
#include <core/iic>
#include <core/ivmm>

extern saturn::uint64_t saturn_vector;

namespace saturn {
namespace core {

// Static pointer to the saved context. It will simplify access to context from different parts of Saturn
static AArch64_Regs* Current_Context = nullptr;

void Exceptions_Init()
{
	Info() << "register AArch64 EL2 excpetions vector" << fmt::endl;

	// Set the exception vector base
	WriteArm64Reg(VBAR_EL2, (uint64_t)&saturn_vector);

	// Enable receiving of Aborts (AMO bit), IRQs (IMO bit) and Fast IRQs (FMO bit)
	WriteArm64Reg(HCR_EL2, (1 << 5) | (1 << 4) | (1 << 3));
}

static void Print_Hyp_Frame(struct AArch64_Regs* Regs)
{
	Info() << fmt::endl << "AArch64 general purpose registers EL2 state:" << fmt::endl;

	Info() << fmt::fill << fmt::hex
	      <<  "  x0  = 0x" << Regs->x0 << "  x1  = 0x" << Regs->x1
	      <<  "  x2  = 0x" << Regs->x2 << "  x3  = 0x" << Regs->x3
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x4  = 0x" << Regs->x4 << "  x5  = 0x" << Regs->x5
	      <<  "  x6  = 0x" << Regs->x6 << "  x7  = 0x" << Regs->x7
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x8  = 0x" << Regs->x8 <<  "  x9  = 0x" << Regs->x9
	      <<  "  x10 = 0x" << Regs->x10 << "  x11 = 0x" << Regs->x11
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x12 = 0x" << Regs->x12 << "  x13 = 0x" << Regs->x13
	      <<  "  x14 = 0x" << Regs->x14 << "  x15 = 0x" << Regs->x15
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x16 = 0x" << Regs->x16 << "  x17 = 0x" << Regs->x17
	      <<  "  x18 = 0x" << Regs->x18 << "  x19 = 0x" << Regs->x19
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x20 = 0x" << Regs->x20 << "  x21 = 0x" << Regs->x21
	      <<  "  x22 = 0x" << Regs->x22 << "  x23 = 0x" << Regs->x23
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x24 = 0x" << Regs->x24 << "  x25 = 0x" << Regs->x25
	      <<  "  x26 = 0x" << Regs->x26 << "  x27 = 0x" << Regs->x27
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  x28 = 0x" << Regs->x28 << "  x29 = 0x" << Regs->x29
	      <<  "  lr =  0x" << Regs->lr_el2 <<  "  sp =  0x" << Regs->sp_el2
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  pc  = 0x" << Regs->pc_el2
	      << fmt::endl;

	Info() << fmt::endl << "AArch64 control registers EL2 state:" << fmt::endl;

	Info() << fmt::fill << fmt::hex
	      <<  "  cpsr = 0x" << Regs->cpsr_el2
	      << fmt::endl;

	uint64_t esr = ReadArm64Reg(ESR_EL2);
	uint64_t far = ReadArm64Reg(FAR_EL2);
	uint64_t pa = va_to_pa_el1(far);
	Info() << fmt::fill << fmt::hex
	      <<  "  esr  = 0x" << esr << "  far = 0x" << far << "  pa = 0x" << pa
	      << fmt::endl;
}

static void Print_Sys_Frame(struct AArch64_Regs* Regs)
{
	Info() << fmt::endl << "AArch64 general purpose registers EL1 state:" << fmt::endl;

	Info() << fmt::fill << fmt::hex
	      <<  "  lr =  0x" << Regs->lr_el1 <<  "  sp =  0x" << Regs->sp_el1
	      << fmt::endl;
	Info() << fmt::fill << fmt::hex
	      <<  "  pc  = 0x" << Regs->pc_el1
	      << fmt::endl;

	Info() << fmt::endl << "AArch64 control registers EL1 state:" << fmt::endl;

	Info() << fmt::fill << fmt::hex
	      <<  "  cpsr  = 0x" << Regs->cpsr_el1
	      << fmt::endl;
}

static void Fault_Mode(struct AArch64_Regs* Regs, bool SysMode)
{
	Print_Hyp_Frame(Regs);

	if (SysMode)
	{
		Print_Sys_Frame(Regs);
	}

	Error() << fmt::endl << "Fault Mode: execution stopped" << fmt::endl;

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" {

using namespace saturn;

void Sync_Abort(struct AArch64_Regs* Regs)
{
	core::Error() << "Exception: Synchronous Abort" << core::fmt::endl;

	core::Fault_Mode(Regs, false);
}

void System_Error(struct AArch64_Regs* Regs)
{
	core::Error() << "Exception: System Error" << core::fmt::endl;

	core::Fault_Mode(Regs, false);
}

void IRq_Handler(struct AArch64_Regs* Regs)
{
	core::Current_Context = Regs;

	core::iIC().Handle_IRq();

	// Check if there is signal to stop VM
	if (core::iVMM().Get_VM_State() == core::vm_state::request_shutdown)
	{
		core::iVMM().Stop_VM();
	}
}

void Guest_Abort(struct AArch64_Regs* Regs)
{
	core::Current_Context = Regs;

	if (saturn::core::Do_Memory_Trap(Regs) == false)
	{
		core::Error() << "Exception: Guest Abort" << core::fmt::endl;
		core::Fault_Mode(Regs, true);
	}
	else
	{
		// Trap handled, skip current EL1 instruction
		Regs->pc_el2 += 4;
	}
}

void Guest_Error(struct AArch64_Regs* Regs)
{
	core::Error() << "Exception: Guest Error" << core::fmt::endl;

	core::Fault_Mode(Regs, true);
}

} // extern "C"
