#include "cpu.hpp"

#include <arm64/registers>
#include <iconsole>

namespace saturn {
namespace core {

static CpuInfo* Local_CPU = nullptr;

CpuInfo::CpuInfo()
{
	uint64_t mpidr = ReadArm64Reg(MPIDR_EL1);

	if (mpidr & (1 << 30))
	{
		Log() << "found single processor system" << fmt::endl;
		CoreId = 0;
	}
	else
	{
		Log() << "found SMP system" << fmt::endl;
		uint64_t affinity = mpidr & 0xff00ffffff;

		// TBD: let's limit to 16 cores only. It needs more investigation later
		//      how to properly handle SMP
		CoreId = affinity & 0xf;
		Log() << "register CPU core " << CoreId << fmt::endl;
	}
}

uint64_t CpuInfo::Id()
{
	return CoreId;
}

void Register_CPU()
{
	Local_CPU = new CpuInfo();
}

ICpuInfo* This_CPU()
{
	return Local_CPU;
}

}; // namespace core
}; // namespace saturn
