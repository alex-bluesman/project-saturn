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

#include "vm_config.hpp"

#include <core/iconsole>
#include <core/iic>
#include <mops>

namespace saturn {
namespace core {

// TBD: should be global const comming from BSP
static const size_t _nrINTs = 256;
static const size_t _nrMMaps = 16;

// Let's use data segment for configuration to avoid additional load on heap
static uint8_t _hwINTMask[_nrINTs / 8 + 1];
static Memory_Region _memRegions[_nrMMaps];

VM_Configuration::VM_Configuration()
	: hwINTMask(_hwINTMask)
	, memRegions(_memRegions)
	, nrRegions(0)
	, osEntry(0)
{
	// Clean up assigned interrupts mask
	for (int i = 0; i < (_nrINTs / 8); i++)
	{
		hwINTMask[i] = 0;
	}
}

VM_Configuration::~VM_Configuration()
{}

void VM_Configuration::VM_Assign_Interrupt(size_t nr)
{
	if (nr < _nrINTs)
	{
		hwINTMask[nr / 8] |= (1 << (nr % 8));
	}
}

bool VM_Configuration::VM_Own_Interrupt(size_t nr)
{
	bool ret = false;

	if (nr < _nrINTs)
	{
		ret = hwINTMask[nr / 8] & (1 << (nr % 8));
	}

	return ret;
}

void VM_Configuration::VM_Assign_Memory_Region(Memory_Region region)
{
	if (nrRegions < _nrMMaps)
	{
		MCopy<uint8_t>(&region, &memRegions[nrRegions], sizeof(Memory_Region));
		nrRegions++;
	}
	else
	{
		Error() << "VM: configuration exceeds memory regions table, please increase the size" << fmt::endl;
	}
}

void VM_Configuration::VM_Allocate_Resources(void)
{
	// Map IPA memory
	for (size_t i = 0; i < nrRegions; i++)
	{
		iMMU_VM().MemoryMap(memRegions[i]);
	}
}

void VM_Configuration::VM_Free_Resources(void)
{
	// Free IPA memory
	for (size_t i = 0; i < nrRegions; i++)
	{
		iMMU_VM().MemoryUnmap(memRegions[i]);
	}

	// Disable assigned physical interrupts
	for (size_t i = 0; i < _nrINTs; i++)
	{
		if (VM_Own_Interrupt(i))
		{
			iIC().IRq_Disable(i);
		}
	}
}

void VM_Configuration::VM_Set_Entry_Address(uint64_t addr)
{
	osEntry = addr;
}

uint64_t VM_Configuration::VM_Get_Entry_Address(void)
{
	return osEntry;
}

}; // namespace core
}; // namespace saturn
