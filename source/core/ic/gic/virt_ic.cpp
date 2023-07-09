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

#include "virt_distributor.hpp"
#include "virt_ic.hpp"

#include <arm64/registers>
#include <core/iic>
#include <fault>

namespace saturn {
namespace core {

// Arm strongly recomments that maintenance interrupts are configured to use INTID 25.
static const uint32_t _maintenance_int = 25;

GicVirtIC::GicVirtIC(GicDistributor& dist)
	: GicDist(dist)
	, vGicDist(nullptr)
	, vState(VICState::Stopped)
{
}

void GicVirtIC::Start(void)
{
	if (VICState::Stopped == vState)
	{
		vGicDist = new VirtGicDistributor(GicDist);
		if (nullptr == vGicDist)
		{
			vState = VICState::Failed;
			Fault("GIC virtual distributor allocation failed");
		}
		else
		{
			iIC().Register_IRq_Handler(_maintenance_int, &MaintenanceIRqHandler);

			uint64_t hcr = (1 << 0);			// En bit
			WriteICCReg(ICH_HCR_EL2, hcr);
			WriteICCReg(ICH_VMCR_EL2, (1 << 9) | (1 << 1));	// VEOIM (EOI drop only), VENG1 (Group 1 INTs)

			vState = VICState::Started;
		}
	}
	else
	{
	
		Fault("GIC virtualization start from incorrect state");
	}
}

void GicVirtIC::Stop()
{
	if (VICState::Started == vState)
	{
		delete vGicDist;
		vGicDist = nullptr;

		vState = VICState::Stopped;
	}
}

void GicVirtIC::Inject_IRq(uint32_t nr)
{
	if (nr < 32)
	{
		// TBD: handle SGI and PPI via virtual redistributor
	}
	else
	{
		if (vGicDist->IRq_Enabled(nr))
		{
			// Inject the IRq
			uint64_t lr = (1UL << 62) | (0UL << 61) | (1UL << 60) | (0x80UL << 48) | (1UL << 41) | nr;	// State (Pending), Group (1), Priority (0x80)
			WriteICCReg(ICH_LR0_EL2, lr);
		}
	}
}

void GicVirtIC::MaintenanceIRqHandler(uint32_t id)
{
	// TBD: see comment to Inject_VM_IRq function
	WriteICCReg(ICH_LR0_EL2, 0);
}

}; // namespace core
}; // namespace saturn
