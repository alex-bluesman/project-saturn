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

#include "cpu_interface.hpp"

#include <arm64/registers>
#include <core/iconsole>

namespace saturn {
namespace core {

CpuInterface::CpuInterface()
{
	// Enable access to CPU interface through system registers
	uint64_t reg = ReadICCReg(ICC_SRE_EL2);
	WriteICCReg(ICC_SRE_EL2, reg | 1);

	// Single priority group
	WriteICCReg(ICC_BPR1_EL1, 0);

	// Set priority mask to handle all interrupts
	WriteICCReg(ICC_PMR_EL1, 0xff);

	// EOIR provides priority drop functionality only.
	// DIR provides interrupt deactivation functionality.
	WriteICCReg(ICC_CTRL_EL1, 1 << 1);
	
	// Enable Group1 interrupts
	WriteICCReg(ICC_IGRPEN1_EL1, 1);

	Info() << "  /CPU interface initialized" << fmt::endl;
}

uint32_t CpuInterface::Read_Ack_IRq()
{
	uint32_t reg = ReadICCReg(ICC_IAR1_EL1);
	return (reg & 0xffffff); // INTID [23:0]
}

void CpuInterface::EOI(uint32_t id)
{
	// Decrease the priority for interrupt
	WriteICCReg(ICC_EOIR1_EL1, id);
	// Deactivate the interrupt
	WriteICCReg(ICC_DIR_EL1, id);
}

}; // namespace core
}; // namespace saturn
