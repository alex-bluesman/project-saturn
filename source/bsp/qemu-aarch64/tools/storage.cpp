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

#include "storage.hpp"

#include <core/iconsole>
#include <core/immu>
#include <mops>

namespace saturn {
namespace bsp {

// TBD: size of OS storage
static const size_t _nrImages = 3;
// Local table to keep information about guest OS images
static OS_Storage_Entry _osImages[_nrImages];

OS_Storage::OS_Storage()
	: osImages(_osImages)
	, nrImages(0)
{}

void OS_Storage::Add_Image(uint64_t source, uint64_t target, size_t size)
{
	if (nrImages < _nrImages)
	{
		osImages[nrImages].sourcePA = source;
		osImages[nrImages].targetPA = target;
		osImages[nrImages].size = size;

		nrImages++;
	}
}

void OS_Storage::Load_Images()
{
	using namespace core;

	// Load OS images
	for (size_t i = 0; i < nrImages; i++)
	{
		OS_Storage_Entry& entry = osImages[i];
		Memory_Region sourceRegion = {entry.sourcePA, entry.sourcePA, entry.size, MMapType::Normal};
		Memory_Region targetRegion = {entry.targetPA, entry.targetPA, entry.size, MMapType::Normal};

		iMMU().MemoryMap(sourceRegion);
		iMMU().MemoryMap(targetRegion);

		Info() << "vmm: copy OS binary from storage to 0x" << fmt::fill << fmt::hex << entry.targetPA << ": ";

		MCopy<uint8_t>((void *)entry.sourcePA, (void *)entry.targetPA, entry.size);

		Raw() << "OK" << fmt::endl;

		iMMU().MemoryUnmap(targetRegion);
		iMMU().MemoryUnmap(sourceRegion);
	}
}

}; // namespace bsp
}; // namespace saturn
