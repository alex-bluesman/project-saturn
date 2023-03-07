#include "cpu_interface.hpp"

#include <arm64/registers>
#include <iconsole>

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

	Log() << "  /CPU interface initialized" << fmt::endl;
}

}; // namespace core
}; // namespace saturn
