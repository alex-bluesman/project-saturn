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
