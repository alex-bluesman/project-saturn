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

#pragma once

#include "console.hpp"
#include "cpu.hpp"
#include "heap.hpp"
#include "ic/ic_core.hpp"
#include "mm/mmu.hpp"
#include "vmm/vm_manager.hpp"

namespace saturn {
namespace core {

// Saturn core components:
static Heap* 			Saturn_Heap = nullptr;		// Heap object pointer to implement operators new/delete
static Console* 		Saturn_Console = nullptr;	// Console pointer for trace and logging
static IC_Core*			Saturn_IC = nullptr;		// Interrupt controller pointer for IRq management
static CpuInfo*			Local_CPU = nullptr;		// CPU information pointer
static VM_Manager*		Saturn_VMM = nullptr;		// Virtual machine manager subsystem

// Access to global core components:
//  - Console			(Log)
//  - Interrupt Controller	(IC)
//  - Memory Management		(MMU)
//  - Heap			(Allocator)

IConsole& iConsole(void)
{
	return *Saturn_Console;
}

IIC& iIC(void)
{
	return *Saturn_IC;
}

IHeap& iHeap(void)
{
	return *Saturn_Heap;
}

ICPU& iCPU(void)
{
	return *Local_CPU;
}

IVirtualMachineManager& iVMM(void)
{
	return *Saturn_VMM;
}

}; // namespace core
}; // namespace saturn


// Overload operators 'new' and 'delete' to use Saturn heap
using namespace saturn;

void* operator new(size_t size) noexcept
{
	return saturn::core::iHeap().Alloc(size);
}

void operator delete(void* base, size_t size) noexcept
{
	return saturn::core::iHeap().Free(base);
}

void* operator new[](size_t size) noexcept
{
	return saturn::core::iHeap().Alloc(size);
}

void operator delete[](void* base) noexcept
{
	return saturn::core::iHeap().Free(base);
}
