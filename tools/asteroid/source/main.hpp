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

#include "console.hpp"
#include "heap.hpp"
#include "ic_core.hpp"

using namespace asteroid;

namespace saturn {
namespace core {

static Console*	Asteroid_Console = nullptr;	// Console pointer for trace and logging
static Heap*	Asteroid_Heap = nullptr;	// Heap object pointer to implement operators new/delete
static IC_Core* Asteroid_IC = nullptr;		// Interrupt controller pointer

IConsole& iConsole(void)
{
	return *Asteroid_Console;
}

IHeap& iHeap(void)
{
	return *Asteroid_Heap;
}

IIC& iIC(void)
{
	return *Asteroid_IC;
}

}; // namespace core
}; // namespace saturn

using namespace saturn;
using namespace saturn::core;

void* operator new(size_t size) noexcept
{
	return iHeap().Alloc(size);
}

void operator delete(void* base, size_t size) noexcept
{
	return iHeap().Free(base);
}

void* operator new[](size_t size) noexcept
{
	return iHeap().Alloc(size);
}

void operator delete[](void* base) noexcept
{
       return iHeap().Free(base);
}
