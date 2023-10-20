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

#include "virt_redistributor.hpp"

#include <bsp/platform>
#include <core/iconsole>
#include <core/iic>
#include <core/ivirtic>

namespace saturn {
namespace core {

static GicRedistRegs _VGicRedistState;

VirtGicRedistributor::VirtGicRedistributor(GicRedistributor& redist)
	: gicRedist(redist)
	, vRedistState(_VGicRedistState)
{
	mTrap = new MTrap(_gic_redist_addr, *this);

	gicRedist.Load_State(vRedistState);

	Info() << "vic: virtual redistributor created" << fmt::endl;
}

VirtGicRedistributor::~VirtGicRedistributor()
{
	// TBD: flush pending interrupts
}

void VirtGicRedistributor::Read(uint64_t reg, void* data, AccessSize size)
{
	using Regs = GicRedistributor::Redist_Regs;

	if (reg >= Regs::SGI_offset)
	{
		// SGI and PPI frame (64KB offset according to GICv3 spec)
		reg -= Regs::SGI_offset;

		switch (reg)
		{
		case Regs::CTRL:
			{
			}
		default:
			break;
		}
	}
}

void VirtGicRedistributor::Write(uint64_t reg, void* data, AccessSize size)
{
	using Regs = GicRedistributor::Redist_Regs;

	switch (reg)
	{
	case Regs::CTRL:
		{
		}
	default:
		break;
	}
}

bool VirtGicRedistributor::IRq_Enabled(uint32_t nr)
{
	uint32_t reg = vRedistState.isenabler0;

	return reg & (1 << (nr % 32));
}

}; // namespace core
}; // namespace saturn
