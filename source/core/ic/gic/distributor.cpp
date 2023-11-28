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

#include "config.hpp"
#include "distributor.hpp"

#include <arm64/registers>
#include <bsp/platform>
#include <core/icpu>
#include <fault>
#include <mops>

namespace saturn {
namespace core {

// Default boot state of GIC distributor. This state will be used
// as initial for virtual GIC
static GicDistRegs _GicDistBootState;

GicDistributor::GicDistributor()
	: bootState(_GicDistBootState)
{
	// TBD: check return value
	Regs = new MMap(MMap::IO_Region(_gic_dist_addr));

	// Save the GIC state before we modify it
	Save_State();

	uint32_t ArchRev = (Regs->Read<uint32_t>(Dist_Regs::PIDR2) >> 4) & 0x0f;
	Info() << "  /found GICv" << ArchRev << fmt::endl;

	if (ArchRev != 3)
	{
		Fault("gic: unsupported version");
	}

	// Disable distributor until setup is complete
	Regs->Write<uint32_t>(Dist_Regs::CTRL, 0);

	// Maximum SPI INTID could be calculated by: 32 * (N + 1)
	size_t ITLinesNumber = Regs->Read<uint32_t>(Dist_Regs::TYPER) & 0x1f;
	linesNumber = 32 * (ITLinesNumber + 1);
	Info() << "  /found " << linesNumber << " interrupt lines" << fmt::endl;

	// So we have now:
	//   SGIs: 0  - 15
	//   PPIs: 16 - 31
	//   SPIs: 32 - SPINumber

	// First let's put SPIs active low
	for (size_t i = _firstSPI; i < linesNumber; i += 16) // 2 bits per line, so 16 lines per register
	{
		Regs->Write<uint32_t>(Dist_Regs::ICFGR + (i / 16) * 4, 0);
	}

	// Set default SPI priority to the middle of the possible range
	for (size_t i = _firstSPI; i < linesNumber; i += 4)
	{
		Regs->Write<uint32_t>(Dist_Regs::IPRIORITYR + (i / 4) * 4, 0x80808080);
	}

	for (size_t i = _firstSPI; i < linesNumber; i += 32)
	{
		Regs->Write<uint32_t>(Dist_Regs::ICENABLER + (i / 32) * 4, 0xffffffff);
		Regs->Write<uint32_t>(Dist_Regs::ICACTIVER + (i / 32) * 4, 0xffffffff);
	}

	// Set all SPIs as non-secure Group 1
	for (size_t i = _firstSPI; i < linesNumber; i += 32)
	{
		Regs->Write<uint32_t>(Dist_Regs::IGROUPR + (i / 32) * 4, 0xffffffff);
	}

	// Wait until write operations are done
	RW_Complete();

	// Enable the distributor
	Regs->Write<uint32_t>(Dist_Regs::CTRL, (1 << 4) | (1 << 1) | (1 << 0)); // EnableGrp0 | EnableGrp1NS | ARE_NS

	// TBD: let's assume that we have only 16 cores maximum, so the only Aff0 is used
	uint32_t affinity = iCPU().Id() & 0x7;

	// Disable broadcast rounting
	affinity &= ~(1 << 31);

	// Route all the SPIs to current CPU
	for (size_t i = _firstSPI; i < linesNumber; i++)
	{
		Regs->Write<uint32_t>(Dist_Regs::IROUTER + i * 8, affinity);
	}

	Info() << "  /distributor initialized" << fmt::endl;
}

void GicDistributor::Send_SGI(uint32_t targetList, uint8_t id)
{
	// Generate SGI id to current PE cluster
	WriteICCReg(ICC_SGI1R_EL1, (id << 24) | targetList);
}

size_t GicDistributor::Get_Max_Lines()
{
	return linesNumber;
}

void GicDistributor::IRq_Enable(uint32_t id)
{
	// Only for SPIs, SGI and PPI managed by redistributor
	if (id >= _firstSPI)
	{
		// Set INT enable
		Regs->Write<uint32_t>(Dist_Regs::ISENABLER + (id / 32) * 4, 1 << (id % 32));
		RW_Complete();
	}
}

void GicDistributor::IRq_Disable(uint32_t id)
{
	if (id >= _firstSPI)
	{
		// Clear INT enable
		Regs->Write<uint32_t>(Dist_Regs::ICENABLER + (id / 32) * 4, 1 << (id % 32));
		RW_Complete();
	}
}

void GicDistributor::RW_Complete(void)
{
	// TBD: should we introduce timeout?
	while (Regs->Read<uint32_t>(Dist_Regs::CTRL) & (1 << 31)); // Check RWP bit (Register Write Pending)
}

void GicDistributor::Save_State(void)
{
	bootState.ctrl = Regs->Read<uint32_t>(Dist_Regs::CTRL);
	bootState.typer = Regs->Read<uint32_t>(Dist_Regs::TYPER);
	bootState.iidr = Regs->Read<uint32_t>(Dist_Regs::IIDR);
	bootState.typer2 = Regs->Read<uint32_t>(Dist_Regs::TYPER2);

	for (size_t i = 0; i < linesNumber / 32; i++)
	{
		bootState.igroupr[i] = Regs->Read<uint32_t>(Dist_Regs::IGROUPR + i * 4);
		bootState.isenabler[i] = Regs->Read<uint32_t>(Dist_Regs::ISENABLER + i * 4);
		bootState.icenabler[i] = Regs->Read<uint32_t>(Dist_Regs::ICENABLER + i * 4);
		bootState.isactiver[i] = Regs->Read<uint32_t>(Dist_Regs::ISACTIVER + i * 4);
		bootState.icactiver[i] = Regs->Read<uint32_t>(Dist_Regs::ICACTIVER + i * 4);
	}


	for (size_t i = 0; i < linesNumber / 4; i++)
	{
		bootState.ipriorityr[i] = Regs->Read<uint32_t>(Dist_Regs::IPRIORITYR + i * 4);
	}

	for (size_t i = 0; i < linesNumber / 16; i++)
	{
		bootState.icfgr[i] = Regs->Read<uint32_t>(Dist_Regs::ICFGR + i * 4);
	}

	for (size_t i = 0; i < linesNumber; i++)
	{
		bootState.irouter[i] = Regs->Read<uint32_t>(Dist_Regs::IROUTER + i * 8);
	}

	bootState.pidr2 = Regs->Read<uint32_t>(Dist_Regs::PIDR2);
}

void GicDistributor::Load_State(GicDistRegs& regs)
{
	MCopy<uint8_t>(&bootState, &regs, sizeof(struct GicDistRegs));
}

}; // namespace core
}; // namespace saturn
