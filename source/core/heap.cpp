#include <system>

#include "heap.hpp"

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
static void Dump_Block_List(IConsole& console, list_head_t* head)
{
	size_t i = 0;
	list_head_t *entry = head->next;
	while (entry != head)
	{
		console << "      Data[" << i++ << "] = 0x" << fmt::hex << fmt::fill
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
	if (base <= &Data16[_heap_size])
	{
		Heap_Free_Block<Data_Block<16>>(&List16_Available, &List16_Allocated, base);
	}
	//TBD
}

void Heap::State(IConsole& console)
{
	console << "Saturn heap state:" << fmt::endl;

	console << "  Data block 16 bytes:" << fmt::endl;
	console << "    Available = " << List_Size(&List16_Available) << fmt::endl;
	Dump_Block_List<Data_Block<16>>(console, &List16_Available);
	console << "    Allocated = " << List_Size(&List16_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<16>>(console, &List16_Allocated);

	console << "  Data block 32 bytes:" << fmt::endl;
	console << "    Available = " << List_Size(&List32_Available) << fmt::endl;
	Dump_Block_List<Data_Block<32>>(console, &List32_Available);
	console << "    Allocated = " << List_Size(&List32_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<32>>(console, &List32_Allocated);

	console << "  Data block 48 bytes:" << fmt::endl;
	console << "    Available = " << List_Size(&List48_Available) << fmt::endl;
	Dump_Block_List<Data_Block<48>>(console, &List48_Available);
	console << "    Allocated = " << List_Size(&List48_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<48>>(console, &List48_Allocated);

	console << "  Data block 64 bytes:" << fmt::endl;
	console << "    Available = " << List_Size(&List64_Available) << fmt::endl;
	Dump_Block_List<Data_Block<64>>(console, &List64_Available);
	console << "    Allocated = " << List_Size(&List64_Allocated) << fmt::endl;
	Dump_Block_List<Data_Block<64>>(console, &List64_Allocated);
}

} // namespace core
} // namespace saturn
