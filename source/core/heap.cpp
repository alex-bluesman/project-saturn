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

#include "heap.hpp"

#include <core/iconsole>
#include <system>

namespace saturn {
namespace core {

// Saturn heap definition
static Data_Block<16> _data16[_heap_size] __section(".heap");
static Data_Block<32> _data32[_heap_size] __section(".heap");
static Data_Block<48> _data48[_heap_size] __section(".heap");
static Data_Block<64> _data64[_heap_size] __section(".heap");

// Template to initialize list of data blocks
template<typename T, size_t _heap_size>
static void Heap_List_Init(list_head_t *avail, list_head_t *alloc, T *data)
{
	List_Init(avail);
	List_Init(alloc);

	for (int i = 0; i < _heap_size; ++i)
	{
		data[i].Id = 1;
		List_Add(&data[i].List, avail);
	}
}

// Template to allocate memory block from the heap
template<typename T>
static void* Heap_Get_Block(list_head_t* avail, list_head_t* alloc)
{
	void* block = nullptr;

	if (List_Size(avail) > 0)
	{
		list_head_t *entry = avail->next;
		List_Del(entry);
		List_Add(entry, alloc);

		block = List_Entry(entry, T, List)->Data;
	}

	return block;
}

template<typename T>
static bool Heap_Free_Block(list_head_t* avail, list_head_t* alloc, void* block)
{
	list_head_t *entry = alloc->next;
	bool found = false;

	while (entry != alloc)
	{
		if (block == (void*)List_Entry(entry, T, List)->Data)
		{
			List_Del(entry);
			List_Add(entry, avail);
			found = true;
			break;
		}
		entry = entry->next;
	}

	return found;
}

// Template to dump blocks in the heap list
template<typename T>
static void Dump_Block_List(list_head_t* head)
{
	size_t i = 0;
	list_head_t *entry = head->next;
	while (entry != head)
	{
		Log() << "      Data[" << i++ << "] = 0x" << fmt::hex << fmt::fill
			<< (unsigned long)List_Entry(entry, T, List)->Data << fmt::endl;
		entry = entry->next;
	}
}


// Heap class implementation
Heap::Heap(void)
	: Data16(_data16)
	, Data32(_data32)
	, Data48(_data48)
	, Data64(_data64)
{
	Heap_List_Init<Data_Block<16>, _heap_size>(&List16_Available, &List16_Allocated, Data16);
	Heap_List_Init<Data_Block<32>, _heap_size>(&List32_Available, &List32_Allocated, Data32);
	Heap_List_Init<Data_Block<48>, _heap_size>(&List48_Available, &List48_Allocated, Data48);
	Heap_List_Init<Data_Block<64>, _heap_size>(&List64_Available, &List64_Allocated, Data64);
}

void* Heap::Alloc(size_t size)
{
	void* block = nullptr;

	if (size <= 16)
	{
		block = Heap_Get_Block<Data_Block<16>>(&List16_Available, &List16_Allocated);
	}
	else if (size <= 32)
	{
		block = Heap_Get_Block<Data_Block<32>>(&List32_Available, &List32_Allocated);
	}
	else if (size <= 48)
	{
		block = Heap_Get_Block<Data_Block<48>>(&List48_Available, &List48_Allocated);
	}
	else if (size <= 64)
	{
		block = Heap_Get_Block<Data_Block<64>>(&List64_Available, &List64_Allocated);
	}

	return block;
}

void Heap::Free(void *base)
{
	if (base < &Data16[_heap_size])
	{
		Heap_Free_Block<Data_Block<16>>(&List16_Available, &List16_Allocated, base);
	}
	else
	if (base < &Data32[_heap_size])
	{
		Heap_Free_Block<Data_Block<32>>(&List32_Available, &List32_Allocated, base);
	}
	else
	if (base < &Data48[_heap_size])
	{
		Heap_Free_Block<Data_Block<48>>(&List48_Available, &List48_Allocated, base);
	}
	else
	if (base < &Data64[_heap_size])
	{
		Heap_Free_Block<Data_Block<64>>(&List64_Available, &List64_Allocated, base);
	}
}

void Heap::State(void)
{
	Log() << "Saturn heap state:" << fmt::endl;

	Log() << "  Data block 16 bytes:" << fmt::endl;
	Log() << "    Available = " << List_Size(&List16_Available) << fmt::endl;
	Dump_Block_List<Data_Block<16>>(&List16_Available);
	Log() << "    Allocated = " << List_Size(&List16_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<16>>(&List16_Allocated);

	Log() << "  Data block 32 bytes:" << fmt::endl;
	Log() << "    Available = " << List_Size(&List32_Available) << fmt::endl;
	Dump_Block_List<Data_Block<32>>(&List32_Available);
	Log() << "    Allocated = " << List_Size(&List32_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<32>>(&List32_Allocated);

	Log() << "  Data block 48 bytes:" << fmt::endl;
	Log() << "    Available = " << List_Size(&List48_Available) << fmt::endl;
	Dump_Block_List<Data_Block<48>>(&List48_Available);
	Log() << "    Allocated = " << List_Size(&List48_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<48>>(&List48_Allocated);

	Log() << "  Data block 64 bytes:" << fmt::endl;
	Log() << "    Available = " << List_Size(&List64_Available) << fmt::endl;
	Dump_Block_List<Data_Block<64>>(&List64_Available);
	Log() << "    Allocated = " << List_Size(&List64_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<64>>(&List64_Allocated);
}

} // namespace core
} // namespace saturn
