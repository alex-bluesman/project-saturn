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
#include <core/ivmm>

namespace saturn {
namespace core {

class VM_Configuration : public IVirtualMachineConfig {
public:
	VM_Configuration();
	~VM_Configuration();

// Configuration interface:
public:
	void VM_Assign_Interrupt(size_t nr);
	void VM_Assign_Memory_Region(Memory_Region region);
	void VM_Set_Entry_Address(uint64_t addr);


// VM resources management:
public:
	void VM_Allocate_Resources(void);
	void VM_Free_Resources(void);
	bool VM_Own_Interrupt(size_t nr);
	uint64_t VM_Get_Entry_Address(void);

private:
	// INT configuration
	uint8_t	(&hwINTMask)[];

	// MMU configuration
	Memory_Region (&memRegions)[];
	size_t nrRegions;

	// Entry address for guest operating system
	uint64_t osEntry;
};

}; // namespace core
}; // namespace saturn
