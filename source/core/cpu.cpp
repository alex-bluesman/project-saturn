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
#include <core/iconsole>

namespace saturn {
namespace core {

CpuInfo::CpuInfo()
{
	uint64_t mpidr = ReadArm64Reg(MPIDR_EL1);

	if (mpidr & (1 << 30))
	{
		Info() << "found single processor system" << fmt::endl;
		CoreId = 0;
	}
	else
	{
		Info() << "found SMP system" << fmt::endl;
		uint64_t affinity = mpidr & 0xff00ffffff;

		// TBD: let's limit to 16 cores only. It needs more investigation later
		//      how to properly handle SMP
		CoreId = affinity & 0xf;
		Info() << "register CPU core " << CoreId << fmt::endl;
	}
}

uint64_t CpuInfo::Id()
{
	return CoreId;
}

}; // namespace core
}; // namespace saturn
