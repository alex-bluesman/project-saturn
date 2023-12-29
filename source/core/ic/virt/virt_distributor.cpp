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

#include <bitops>
#include <bsp/platform>
#include <core/iconsole>
#include <core/ivirtic>
#include <core/ivmm>

namespace saturn {
namespace core {

static GicDistRegs _VGicDistState;

VirtGicDistributor::VirtGicDistributor(GicDistributor& dist)
	: gicDist(dist)
	, vGicState(_VGicDistState)
{
	mTrap = new MTrap(_gic_dist_addr, *this);

	gicDist.Load_State(vGicState);

	Info() << "vic: virtual distributor created" << fmt::endl;
}

VirtGicDistributor::~VirtGicDistributor()
{
	// TBD: flush pending interrupts

	delete mTrap;
}

void VirtGicDistributor::Read(uint64_t reg, void* data, AccessSize size)
{
	using Regs = GicDistributor::Dist_Regs;

	// DBG:
	//Log() << "vgicd: read from register offset 0x" << fmt::hex << reg << fmt::endl;

	switch (reg)
	{
	case Regs::CTRL:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.ctrl;
			break;
		}
	case Regs::TYPER:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.typer;
			break;
		}
	case Regs::IIDR:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.iidr;
			break;
		}
	case Regs::TYPER2:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.typer2;
			break;
		}
	case Regs::PIDR2:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			*val = vGicState.pidr2;
			break;
		}
	// TBD: think later to optimize the following section
	default:
		// case IGROUPR:
		if ((reg >= Regs::IGROUPR) && (reg < Regs::IGROUPR + sizeof(vGicState.igroupr)))
		{
			size_t index = (reg - Regs::IGROUPR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.igroupr[index];
			break;
		}
		else
		// case ISENABLER:
		if ((reg >= Regs::ISENABLER) && (reg < Regs::ISENABLER + sizeof(vGicState.isenabler)))
		{
			size_t index = (reg - Regs::ISENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.isenabler[index];
			break;
		}
		else
		// case ICENABLER:
		if ((reg >= Regs::ICENABLER) && (reg < Regs::ICENABLER + sizeof(vGicState.icenabler)))
		{
			size_t index = (reg - Regs::ICENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.isenabler[index];
			break;
		}
		else
		// case ISACTIVER:
		if ((reg >= Regs::ISACTIVER) && (reg < Regs::ISACTIVER + sizeof(vGicState.isactiver)))
		{
			size_t index = (reg - Regs::ISACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.isactiver[index];
			break;
		}
		else
		// case ICACTIVER:
		if ((reg >= Regs::ICACTIVER) && (reg < Regs::ICACTIVER + sizeof(vGicState.icactiver)))
		{
			size_t index = (reg - Regs::ICACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.icactiver[index];
			break;
		}
		else
		// case IPRIORITYR:
		if ((reg >= Regs::IPRIORITYR) && (reg < Regs::IPRIORITYR + sizeof(vGicState.ipriorityr)))
		{
			size_t index = (reg - Regs::IPRIORITYR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.ipriorityr[index];
			break;
		}
		else
		// case ICFGR:
		if ((reg >= Regs::ICFGR) && (reg < Regs::ICFGR + sizeof(vGicState.icfgr)))
		{
			size_t index = (reg - Regs::ICFGR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			*val = vGicState.icfgr[index];
			break;
		}
		else
		// case IROUTER:
		if ((reg >= Regs::IROUTER) && (reg < Regs::IROUTER + sizeof(vGicState.irouter)))
		{
			size_t index = (reg - Regs::IROUTER) / 8;
			uint64_t* val = static_cast<uint64_t*>(data);

			*val = vGicState.irouter[index];
			break;
		}
		else
		{
			// DBG:
			//Log() << "vgicd: unsupported read from register offset 0x" << fmt::hex << fmt::fill << reg << fmt::endl;
		}
	}
}

void VirtGicDistributor::Write(uint64_t reg, void* data, AccessSize size)
{
	using Regs = GicDistributor::Dist_Regs;

	// DBG:
	//Log() << "vgicd: write value 0x" << fmt::hex << *static_cast<uint32_t*>(data) << " to register offset 0x" << reg << fmt::endl;

	switch (reg)
	{
	case Regs::CTRL:
		{
			uint32_t* val = static_cast<uint32_t*>(data);
			vGicState.ctrl = *val;
			break;
		}
	// TBD: think later to optimize the following section
	default:
		// case IGROUPR:
		if ((reg >= Regs::IGROUPR) && (reg < Regs::IGROUPR + sizeof(vGicState.igroupr)))
		{
			size_t index = (reg - Regs::IGROUPR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.igroupr[index] = *val;
			break;
		}
		else
		// case ISENABLER:
		if ((reg >= Regs::ISENABLER) && (reg < Regs::ISENABLER + sizeof(vGicState.isenabler)))
		{
			size_t index = (reg - Regs::ISENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);
			// TBD: multiple bits could be set
			size_t nr = index * 32 + FirstSetBit(*val);

			// DBG:
			//Log() << "vgicd: set enable INT(" << nr << ")" << fmt::endl;

			if ((iVMM().Get_VM_State() == vm_state::running) && (iVMM().Guest_IRq(nr)))
			{
				gicDist.IRq_Enable(nr);
			}

			vGicState.isenabler[index] |= *val;
			break;
		}
		else
		// case ICENABLER:
		if ((reg >= Regs::ICENABLER) && (reg < Regs::ICENABLER + sizeof(vGicState.icenabler)))
		{
			size_t index = (reg - Regs::ICENABLER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);
			// TBD: multiple bits could be set
			size_t nr = index * 32 + FirstSetBit(*val);

			// DBG:
			//Log() << "vgicd: clear enable INT(" << nr << ")" << fmt::endl;

			if ((iVMM().Get_VM_State() == vm_state::running) && (iVMM().Guest_IRq(nr)))
			{
				gicDist.IRq_Disable(nr);
			}

			vGicState.isenabler[index] &= ~(*val);
			break;
		}
		else
		// case ISACTIVER:
		if ((reg >= Regs::ISACTIVER) && (reg < Regs::ISACTIVER + sizeof(vGicState.isactiver)))
		{
			size_t index = (reg - Regs::ISACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.isactiver[index] = *val;
			break;
		}
		else
		// case ICACTIVER:
		if ((reg >= Regs::ICACTIVER) && (reg < Regs::ICACTIVER + sizeof(vGicState.icactiver)))
		{
			size_t index = (reg - Regs::ICACTIVER) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.icactiver[index] = *val;
			break;
		}
		else
		// case IPRIORITYR:
		if ((reg >= Regs::IPRIORITYR) && (reg < Regs::IPRIORITYR + sizeof(vGicState.ipriorityr)))
		{
			size_t index = (reg - Regs::IPRIORITYR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.ipriorityr[index] = *val;
			break;
		}
		else
		// case ICFGR:
		if ((reg >= Regs::ICFGR) && (reg < Regs::ICFGR + sizeof(vGicState.icfgr)))
		{
			size_t index = (reg - Regs::ICFGR) / 4;
			uint32_t* val = static_cast<uint32_t*>(data);

			vGicState.icfgr[index] = *val;
			break;
		}
		else
		// case IROUTER:
		if ((reg >= Regs::IROUTER) && (reg < Regs::IROUTER + sizeof(vGicState.irouter)))
		{
			size_t index = (reg - Regs::IROUTER) / 8;
			uint64_t* val = static_cast<uint64_t*>(data);

			vGicState.irouter[index] = *val;
			break;
		}
		else
		{
			// DBG:
			//Log() << "vgicd: unsupported write to register offset 0x" << fmt::hex << fmt::fill << reg << fmt::endl; 
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
