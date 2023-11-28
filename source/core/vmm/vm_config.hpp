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

#include <basetypes>
#include <core/immu>

namespace saturn {
namespace core {

// TBD: should be global const comming from BSP
static const size_t _nrINTs = 256;
static const size_t _nrMMaps = 16;

// Data types used for configuration:
enum class OS_Type
{
	Default,
	Linux
};

class VM_Configuration {
public:
	VM_Configuration();
	~VM_Configuration();

// VM interrupts management:
public:
	void VM_Assign_Interrupt(size_t nr);
	bool VM_Own_Interrupt(size_t nr);

// VM memory management:
public:
	void VM_Add_Memory_Region(Memory_Region region);
	void VM_Map_All(void);
	void VM_Unmap_All(void);

// VM guest OS management:
public:
	void VM_Set_Guest_OS(OS_Type type);
	void VM_Set_Entry(uint64_t addr);

private:
	uint8_t	(&hwINTMask)[];

	Memory_Region (&memRegions)[];
	size_t nrRegions;

public:
	OS_Type osType;
	uint64_t osEntry;
};

}; // namespace core
}; // namespace saturn
