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

#include "gic/cpu_interface.hpp"
#include "gic/distributor.hpp"
#include "gic/redistributor.hpp"

#include <fault>

namespace saturn {
namespace core {

// According to the GICv3 Architecture Specification, the maximal SPI number could be 1020.
// TBD: due to we do not support ESPIs and LPIs, let's limit table size to 1020.
static const size_t _maxIRq = 1020;

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
	Log() << "create interrupts infrastructure" << fmt::endl;


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
		Log() << "!error: invalid SGI id = " << id << fmt::endl;
	}
}

void IC_Core::Handle_IRq()
{
	uint32_t id = CpuIface->Read_Ack_IRq();

	if (id < GicDist->Get_Max_Lines())
	{
		IRq_Table[id](id);
	}
	else
	{
		Log() << "!error: received INT with ID (" << id << ") out of supported range" << fmt::endl;
	}

	CpuIface->EOI(id);
}

void IC_Core::Register_IRq_Handler(uint32_t id, IRqHandler handler)
{
	if (id < GicDist->Get_Max_Lines())
	{
		IRq_Table[id] = handler;
	}
	else
	{
		Log() << "!error: attempt to register INT handler with ID (" << id << ") out of supported range" << fmt::endl;
	}
}

void IC_Core::Default_Handler(uint32_t id)
{
	Log() << "!warning: received INT with ID (" << id << ") without registered handler" << fmt::endl;
}

}; // namespace core
}; // namespace saturn
