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

#include "ic_core.hpp"

#include "gic/config.hpp"
#include "gic/cpu_interface.hpp"
#include "gic/distributor.hpp"
#include "gic/redistributor.hpp"

#include "virt/virt_ic.hpp"

#include <arm64/registers>
#include <core/ivmm>
#include <fault>

namespace saturn {
namespace core {

// TBD: think about better allocation for this data block
static IRqHandler _IRq_Table[_maxIRq];

IC_Core::IC_Core()
	: IRq_Table(_IRq_Table)
{
	// GICv3 consists from the following logical components:
	//
	//  --- Interrupt Routing Infrastructure-----   --- CPU Cluster ----
	// |   -------------       ---------------   | |   ---------------  |
	// |  | Distributor | <-> | Redistributor | <---> | CPU Interface | |
	// |   -------------       ---------------   | |   ---------------  |
	//  -----------------------------------------   --------------------
	//
	// So let's construct objects for the complete routing chain

	// TBD: we should distinguish primary CPU and secondary.
	//      GIC dist should be initialized only for primary CPU.
	Info() << "create interrupts infrastructure" << fmt::endl;


	GicDist = new GicDistributor();
	if (nullptr == GicDist)
	{
		Fault("GIC distributor allocation failed");
	}

	GicRedist = new GicRedistributor();
	if (nullptr == GicRedist)
	{
		Fault("GIC redistributor allocation failed");
	}

	CpuIface = new CpuInterface();
	if (nullptr == CpuIface)
	{
		Fault("GIC CPU interface allocation failed");
	}

	for (int i = 0; i < GicDist->Get_Max_Lines(); i++)
	{
		IRq_Table[i] = Default_Handler;
	}

	GicVIC = new GicVirtIC(*GicDist, *GicRedist);
	if (nullptr == GicVIC)
	{
		Fault("GIC virtual interface allocation failed");
	}
}

void IC_Core::Local_IRq_Disable()
{
	asm volatile (
			"msr daifset, #2\n"
			"isb\n"
			::: "memory"
		     );
}

void IC_Core::Local_IRq_Enable()
{
	asm volatile (
			"msr daifclr, #2\n"
			"isb\n"
			::: "memory"
		     );
}

void IC_Core::Send_SGI(uint32_t targetList, uint8_t id)
{
	if (id < 16)
	{
		GicDist->Send_SGI(targetList, id);
	}
	else
	{
		Error() << "error: invalid SGI id = " << id << fmt::endl;
	}
}

void IC_Core::Handle_IRq()
{
	uint32_t nr = CpuIface->Read_Ack_IRq();

	if ((iVMM().Get_VM_State() == vm_state::running) && (iVMM().Guest_IRq(nr)))
	{
		// VM is running and IRq assigned to the guest, so just route it
		Inject_VM_IRq(nr);
	}
	else
	{
		// For all other cases handle IRq by hypervisor
		if (nr < GicDist->Get_Max_Lines())
		{
			IRq_Table[nr](nr);
		}
		else
		{
			Error() << "error: received INT with ID (" << nr << ") out of supported range" << fmt::endl;
		}

		CpuIface->EOI(nr);
	}
}

void IC_Core::Register_IRq_Handler(uint32_t id, IRqHandler handler)
{
	if (id < GicDist->Get_Max_Lines())
	{
		IRq_Table[id] = handler;

		if (id < 32)
		{
			GicRedist->IRq_Enable(id);
		}
		else
		{
			GicDist->IRq_Enable(id);
		}
	}
	else
	{
		Error() << "error: attempt to register INT handler with ID (" << id << ") out of supported range" << fmt::endl;
	}
}

void IC_Core::Default_Handler(uint32_t id)
{
	Error() << "warning: received INT with ID (" << id << ") without registered handler" << fmt::endl;
}

void IC_Core::Start_Virt_IC()
{
	GicVIC->Start();
}

void IC_Core::Stop_Virt_IC()
{
	GicVIC->Stop();
}

void IC_Core::Inject_VM_IRq(uint32_t nr)
{
	if (iVMM().Get_VM_State() == vm_state::running)
	{
		GicVIC->Inject_IRq(nr);
	}
	else
	{	
		Error() << "warning: attempt to inject INT to non-running VM" << fmt::endl;
	}
}

}; // namespace core
}; // namespace saturn
