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

#include <arm64/registers>
#include <fault>

namespace asteroid {

// According to the GICv3 Architecture Specification, the maximal SPI number could be 1020.
// TBD: due to we do not support ESPIs and LPIs, let's limit table size to 1020.
static const size_t _maxIRq = 1020;
// Arm strongly recomments that maintenance interrupts are configured to use INTID 25.
static const uint32_t _maintenance_int = 25;

static const uint64_t _gic_dist_addr = 0x08000000;

// TBD: think about better allocation for this data block
static IRqHandler _IRq_Table[_maxIRq];

enum Dist_Regs
{
	CTRL		= 0x0000,
	TYPER		= 0x0004,
	IGROUPR		= 0x0080,
	ISENABLER	= 0x0100,
	ICENABLER	= 0x0180,
	ISACTIVER	= 0x0300,
	ICACTIVER	= 0x0380,
	IPRIORITYR	= 0x0400,
	ICFGR		= 0x0c00,
	IROUTER		= 0x6100,
	PIDR2		= 0xffe8,
};

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

	for (int i = 0; i < _maxIRq; i++)
	{
		IRq_Table[i] = Default_Handler;
	}

	// Single priority group
	WriteICCReg(ICC_BPR1_EL1, 0);

	// Set priority mask to handle all interrupts
	WriteICCReg(ICC_PMR_EL1, 0xff);

	// EOIR provides priority drop functionality only.
	// DIR provides interrupt deactivation functionality.
	WriteICCReg(ICC_CTRL_EL1, 1 << 1);
	
	// Enable Group1 interrupts
	WriteICCReg(ICC_IGRPEN1_EL1, 1);

	Info() << "GIC initialization complete" << fmt::endl;
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
		// TBD
	}
	else
	{
		Error() << "error: invalid SGI id = " << id << fmt::endl;
	}
}

void IC_Core::Handle_IRq()
{
	uint32_t id = ReadICCReg(ICC_IAR1_EL1) & 0xffffff;

	if (id < _maxIRq)
	{
		IRq_Table[id](id);
	}
	else
	{
		Error() << "error: received INT with ID (" << id << ") out of supported range" << fmt::endl;
	}

	// Decrease the priority for interrupt
	WriteICCReg(ICC_EOIR1_EL1, id);
	// Deactivate the interrupt
	WriteICCReg(ICC_DIR_EL1, id);
}

void IC_Core::Register_IRq_Handler(uint32_t id, IRqHandler handler)
{
	if (id < _maxIRq)
	{
		IRq_Table[id] = handler;

		if (id >= 32)
		{
			// Set INT enable
			Write<uint32_t>(_gic_dist_addr + Dist_Regs::ISENABLER + (id / 32) * 4, 1 << (id % 32));
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

}; // namespace asteroid
