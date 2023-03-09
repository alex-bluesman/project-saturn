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

#include <iconsole>
#include <iheap>
#include <list>

namespace saturn {
namespace core {

class IConsole;

// Data block structure
template <size_t _block_size>
struct Data_Block
{
	int Id;
	char Data[_block_size];
	list_head_t List;
};

class Heap : public IHeap
{
public:
	Heap();

public:
	virtual void*	Alloc(size_t size) override;
	virtual void	Free(void* base) override;

// Debugging interface
public:
	void	State(IConsole& console);

private:
	Data_Block<16> (&Data16)[];
	list_head_t	List16_Available;
	list_head_t	List16_Allocated;

	Data_Block<32> (&Data32)[];
	list_head_t	List32_Available;
	list_head_t	List32_Allocated;

	Data_Block<48> (&Data48)[];
	list_head_t	List48_Available;
	list_head_t	List48_Allocated;

	Data_Block<64> (&Data64)[];
	list_head_t	List64_Available;
	list_head_t	List64_Allocated;

};

}; // namespace core
}; // namespace saturn
