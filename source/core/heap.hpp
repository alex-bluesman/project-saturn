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

#include <core/iheap>
#include <lib/list>

namespace saturn {
namespace core {

// Data block structure
template <size_t _block_size>
struct Data_Block
{
	char data[_block_size];
	lib::List<void*>::Element element;
};

// Data pool which manages the blocks
class Data_Pool
{
public:
	Data_Pool(size_t s);

public:
	inline size_t Block_Size(void);
	inline bool Has_Free_Block(void);
	void* Get_Block(void);
	bool Free_Block(void* base);
// Debugging interface
public:
	void State(void);

public:
	lib::List<void*> available;
	lib::List<void*> allocated;
	size_t block_size;
};

// Heap orchestrator
class Heap : public IHeap
{
public:
	Heap();

public:
	virtual void*	Alloc(size_t size) override;
	virtual void	Free(void* base) override;

// Debugging interface
public:
	void	State(void);

private:
	void	Data_Pools_Init();

private:
	Data_Pool pool16;
	Data_Pool pool32;
	Data_Pool pool48;
	Data_Pool pool64;
};

}; // namespace core
}; // namespace saturn
