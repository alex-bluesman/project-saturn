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

#include "../gic/cpu_interface.hpp"
#include "virt_distributor.hpp"
#include "virt_redistributor.hpp"
#include "virt_ic.hpp"

#include <arm64/registers>
#include <bitops>
#include <core/iic>
#include <core/ivmm>
#include <fault>

namespace saturn {
namespace core {

// Arm strongly recommends that maintenance interrupts are configured to use INTID 25.
static const uint32_t _maintenance_int = 25;
// TBD: ugly way to have access from static function to class instance
static GicVirtIC* thisVIC = nullptr;

GicVirtIC::GicVirtIC(CpuInterface& cpu, GicDistributor& dist, GicRedistributor& redist)
	: CpuIface(cpu)
	, GicDist(dist)
	, GicRedist(redist)
	, vGicDist(nullptr)
	, vGicRedist(nullptr)
	, vState(VICState::Stopped)
{
	thisVIC = this;
	nrLRs = (ReadICCReg(ICH_VTR_EL2) & 0xf) + 1;

	Log() << "vic: found " << nrLRs << " LR registers" << fmt::endl;
}

void GicVirtIC::Start(void)
{
	if (VICState::Stopped == vState)
	{
		vGicDist = new VirtGicDistributor(GicDist);
		vGicRedist = new VirtGicRedistributor(GicRedist);
		
		if ((nullptr == vGicDist) || (nullptr == vGicRedist))
		{
			vState = VICState::Failed;
			Fault("GIC virtual [re]distributor allocation failed");
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

		delete vGicRedist;
		vGicRedist = nullptr;

		vState = VICState::Stopped;
	}
}

void GicVirtIC::Set_LR(uint8_t id, uint64_t val)
{
	switch (id)
	{
	case 0:
		WriteICCReg(ICH_LR0_EL2, val);
		break;
	case 1:
		WriteICCReg(ICH_LR1_EL2, val);
		break;
	case 2:
		WriteICCReg(ICH_LR2_EL2, val);
		break;
	case 3:
		WriteICCReg(ICH_LR3_EL2, val);
		break;
	case 4:
		WriteICCReg(ICH_LR4_EL2, val);
		break;
	case 5:
		WriteICCReg(ICH_LR5_EL2, val);
		break;
	case 6:
		WriteICCReg(ICH_LR6_EL2, val);
		break;
	case 7:
		WriteICCReg(ICH_LR7_EL2, val);
		break;
	case 8:
		WriteICCReg(ICH_LR8_EL2, val);
		break;
	case 9:
		WriteICCReg(ICH_LR9_EL2, val);
		break;
	case 10:
		WriteICCReg(ICH_LR10_EL2, val);
		break;
	case 11:
		WriteICCReg(ICH_LR11_EL2, val);
		break;
	case 12:
		WriteICCReg(ICH_LR12_EL2, val);
		break;
	case 13:
		WriteICCReg(ICH_LR13_EL2, val);
		break;
	case 14:
		WriteICCReg(ICH_LR14_EL2, val);
		break;
	case 15:
		WriteICCReg(ICH_LR15_EL2, val);
		break;
	default:
		Fault("attempt to set out of range GIC LR");
	}
}

void GicVirtIC::Inject_IRq(uint32_t nr, vINTtype type)
{
	if (nr < _maxIRq)
	{
		if (
		    ((nr < _firstSPI) && vGicRedist->IRq_Enabled(nr)) ||		// nr == 0..31, what means it's SGI or PPI, so ask redistributor
		    ((nr >= _firstSPI) && vGicDist->IRq_Enabled(nr))			// nr == 32.., what means it's SPI, so ask distributor
		   )
		{
			uint64_t lrMask = ReadICCReg(ICH_ELRSR_EL2) & 0xf;
			uint8_t pos = FirstSetBit<uint16_t>(lrMask);

			if (pos < nrLRs)
			{
				uint64_t lr;

				if (vINTtype::Hardware == type)
				{
					lr = (1UL << 62) | (1UL << 61) | (1UL << 60) | (0x80UL << 48) | ((uint64_t)nr << 32) | nr;	// State (Pending), Group (1), Priority (0x80)
				}
				else // vINTtype::Software == type
				{
					lr = (1UL << 62) | (0UL << 61) | (1UL << 60) | (0x80UL << 48) | (1UL << 41) | nr;		// State (Pending), Group (1), Priority (0x80)
				}
				Set_LR(pos, lr);
			}
			else
			{
				Fault("gic: no free LRs, but queueing is not yet implemented");
			}
		}
		else
		{
			Info() << "warning: attempt to inject INT(" << nr << ") while it's disabled" << fmt::endl;
		}
	}
	else
	{
		Info() << "warning: attempt to inject unsupported ESPI INT(" << nr << ")" << fmt::endl;
	}
}

void GicVirtIC::Process_ISR(void)
{
	uint64_t eisr = ReadICCReg(ICH_EISR_EL2);

	while (eisr > 0)
	{
		uint8_t nr = FirstSetBit<uint16_t>(eisr);

		if (nr < nrLRs)
		{
			Set_LR(nr, 0);
		}

		ClearBit(eisr, nr);
	}
}

void GicVirtIC::MaintenanceIRqHandler(uint32_t nr)
{
	thisVIC->Process_ISR();
}

}; // namespace core
}; // namespace saturn
