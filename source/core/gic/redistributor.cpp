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

#include "redistributor.hpp"

#include <arm64/registers>
#include <iconsole>
#include <platform/qemuarm64>

namespace saturn {
namespace core {

enum Gic_Redist
{
	CTRL		= 0x0000,
	WAKER		= 0x0014,
	IGROUPR0	= 0x0080,
	ISENABLER0	= 0x0100,
	ICENABLER0	= 0x0180,
	ICACTIVER0	= 0x0380,
	IPRIORITYR	= 0x0400,
	SGI_offset	= 0x10000	// 64K SGI region offset
};

GicRedistributor::GicRedistributor()
{
	// TBD: check return value
	Regs = new MMap(MMap::IO_Region(_gic_redist_addr));

	// Wake up redistributor
	uint32_t reg = Regs->Read<uint32_t>(Gic_Redist::WAKER);
	reg &= ~(1 << 1); // Clear ProcessorSleep flag
	Regs->Write<uint32_t>(Gic_Redist::WAKER, reg);

	// Wait until redistributor is available
	// TBD: should we introduce timeout?
	while (Regs->Read<uint32_t>(Gic_Redist::WAKER) & (1 << 2));

	for (size_t i = 0; i < 16; i += 4)
	{
		Regs->Write<uint32_t>(SGI_offset + Gic_Redist::IPRIORITYR + (i / 4) * 4, 0x40404040);
	}

	for (size_t i = 16; i < 32; i += 4)
	{
		Regs->Write<uint32_t>(SGI_offset + Gic_Redist::IPRIORITYR + (i / 4) * 4, 0x80808080);
	}

	// Disable all the SGIs and PPIs on start up
	Regs->Write<uint32_t>(SGI_offset + Gic_Redist::ICACTIVER0, 0xffffffff);

	// Disable all the SGIs and PPIs on start up
	Regs->Write<uint32_t>(SGI_offset + Gic_Redist::ICENABLER0, 0xffff0000);
	Regs->Write<uint32_t>(SGI_offset + Gic_Redist::ISENABLER0, 0x0000ffff);

	// Group 1
	Regs->Write<uint32_t>(SGI_offset + Gic_Redist::IGROUPR0, 0xffffffff);

	// Wait until write operations are done
	// TBD: should we introduce timeout?
	while (Regs->Read<uint32_t>(Gic_Redist::CTRL) & (1 << 31));

	Log() << "  //redistributor initialized" << fmt::endl;
}

}; // namespace core
}; // namespace saturn