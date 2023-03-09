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

#include "gic/cpu_interface.hpp"
#include "gic/distributor.hpp"
#include "gic/redistributor.hpp"

#include <fault>

namespace saturn {
namespace core {

static CpuInterface* CpuIface = nullptr;
static GicDistributor* GicDist = nullptr;
static GicRedistributor* GicRedist = nullptr;

static inline void Local_IRQ_Enable()
{
	asm volatile (
			"msr daifclr, #2\n"
			"isb\n"
			::: "memory"
		     );
}

void Interrupts_Init()
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

	// Now we are ready to ebable IRQs
	Local_IRQ_Enable();

	// Debug purpose:
	GicDist->SendSGI();
}

}; // namespace core
}; // namespace saturn
