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

#include <bsp/platform>
#include <core/iconsole>
#include <core/iic>
#include <core/ivirtic>

namespace saturn {
namespace core {

static GicDistRegs _VGicDistState;

VirtGicDistributor::VirtGicDistributor(GicDistributor& dist)
	: gicDist(dist)
	, vGicState(_VGicDistState)
{
	mTrap = new MTrap(_gic_dist_addr, *this);

	gicDist.Load_State(vGicState);
}

VirtGicDistributor::~VirtGicDistributor()
{
	// TBD: flush pending interrupts
}

void VirtGicDistributor::Read(uint64_t reg, void* data, AccessSize size)
{
	switch (reg)
	{
	case CTRL:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.ctrl;
			break;
		}
	case TYPER:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.typer;
			break;
		}
	case PIDR2:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.pidr2;
			break;
		}
	// TBD: think later to optimize the following section
	default:
		// case IGROUPR:
		if ((reg >= IGROUPR) && (reg < IGROUPR + sizeof(vGicState.igroupr)))
		{
			size_t index = (reg - IGROUPR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.igroupr[index];
			break;
		}
		else
		// case ISENABLER:
		if ((reg >= ISENABLER) && (reg < ISENABLER + sizeof(vGicState.isenabler)))
		{
			size_t index = (reg - ISENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.isenabler[index];
			break;
		}
		else
		// case ICENABLER:
		if ((reg >= ICENABLER) && (reg < ICENABLER + sizeof(vGicState.icenabler)))
		{
			size_t index = (reg - ICENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.icenabler[index];
			break;
		}
		else
		// case ISACTIVER:
		if ((reg >= ISACTIVER) && (reg < ISACTIVER + sizeof(vGicState.isactiver)))
		{
			size_t index = (reg - ISACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.isactiver[index];
			break;
		}
		else
		// case ICACTIVER:
		if ((reg >= ICACTIVER) && (reg < ICACTIVER + sizeof(vGicState.icactiver)))
		{
			size_t index = (reg - ICACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.icactiver[index];
			break;
		}
		else
		// case IPRIORITYR:
		if ((reg >= IPRIORITYR) && (reg < IPRIORITYR + sizeof(vGicState.ipriorityr)))
		{
			size_t index = (reg - IPRIORITYR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.ipriorityr[index];
			break;
		}
		else
		// case IROUTER:
		if ((reg >= IROUTER) && (reg < IROUTER + sizeof(vGicState.irouter)))
		{
			size_t index = (reg - IROUTER) / 8;
			uint64_t* val = static_cast<uint64_t*>(data);

			*val = vGicState.irouter[index];
			break;
		}
		else
		{
			Info() << "vgicd: unsupported read from register offset 0x" << fmt::hex << fmt::fill << reg << fmt::endl; 
		}
	}
}

void VirtGicDistributor::Write(uint64_t reg, void* data, AccessSize size)
{
	switch (reg)
	{
	case CTRL:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			vGicState.ctrl = *val;
			break;
		}
	// TBD: think later to optimize the following section
	default:
		// case IGROUPR:
		if ((reg >= IGROUPR) && (reg < IGROUPR + sizeof(vGicState.igroupr)))
		{
			size_t index = (reg - IGROUPR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.igroupr[index] = *val;
			break;
		}
		else
		// case ISENABLER:
		if ((reg >= ISENABLER) && (reg < ISENABLER + sizeof(vGicState.isenabler)))
		{
			size_t index = (reg - ISENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.isenabler[index] = *val;
			
			// TBD: only for testing purpose
			iVirtIC().Inject_VM_IRq(32);
			// -----------------------------

			break;
		}
		else
		// case ICENABLER:
		if ((reg >= ICENABLER) && (reg < ICENABLER + sizeof(vGicState.icenabler)))
		{
			size_t index = (reg - ICENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.icenabler[index] = *val;
			break;
		}
		else
		// case ISACTIVER:
		if ((reg >= ISACTIVER) && (reg < ISACTIVER + sizeof(vGicState.isactiver)))
		{
			size_t index = (reg - ISACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.isactiver[index] = *val;
			break;
		}
		else
		// case ICACTIVER:
		if ((reg >= ICACTIVER) && (reg < ICACTIVER + sizeof(vGicState.icactiver)))
		{
			size_t index = (reg - ICACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.icactiver[index] = *val;
			break;
		}
		else
		// case IPRIORITYR:
		if ((reg >= IPRIORITYR) && (reg < IPRIORITYR + sizeof(vGicState.ipriorityr)))
		{
			size_t index = (reg - IPRIORITYR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.ipriorityr[index] = *val;
			break;
		}
		else
		// case IROUTER:
		if ((reg >= IROUTER) && (reg < IROUTER + sizeof(vGicState.irouter)))
		{
			size_t index = (reg - IROUTER) / 8;
			uint64_t* val = static_cast<uint64_t*>(data);

			vGicState.irouter[index] = *val;
			break;
		}
		else
		{
			Info() << "vgicd: unsupported write to register offset 0x" << fmt::hex << fmt::fill << reg << fmt::endl; 
		}
	}
}

bool VirtGicDistributor::IRq_Enabled(uint32_t nr)
{
	uint32_t reg = vGicState.isenabler[(nr / 32)];
	
	return reg & (1 << (nr % 32));
}

}; // namespace core
}; // namespace saturn
