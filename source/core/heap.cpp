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

#include "mm/config.hpp"
#include "heap.hpp"

#include <core/iconsole>
#include <system>

namespace saturn {
namespace core {

// (!) Heap part for MMU.
// Note: we can't use Data_Block_List here because MMU requires pages to be aligned, what is not
// possible within the structure mentioned above, because one element size is (_page_size + 16).
// So let's manually create this special list for MMU.
static Data_Block<_page_size>    _mmu_pages[_l3_tables] __section(".heap") __align(_page_size);
static lib::List<void*>::Element _mmu_pool[_l3_tables]  __section(".heap");

// (!) Heap pre-allocated data pools, must be used as carefully
static Data_Block_List<16> _pool16[_heap_size] __section(".heap");
static Data_Block_List<32> _pool32[_heap_size] __section(".heap");
static Data_Block_List<48> _pool48[_heap_size] __section(".heap");
static Data_Block_List<64> _pool64[_heap_size] __section(".heap");

// Let's have heap start pointer for some debugging stuff in MMU module
const void* _heap_start = &_mmu_pages[0];

Data_Pool::Data_Pool(size_t s)
	: available(lib::Allocator_Type::none)
	, allocated(lib::Allocator_Type::none)
	, block_size(s)
{}

size_t Data_Pool::Block_Size(void)
{
	return block_size;
}

bool Data_Pool::Has_Free_Block(void)
{
	return (available.size() > 0);
};

void* Data_Pool::Get_Block(void)
{
	void* block = nullptr;

	if (available.size() > 0)
	{
		auto it = available.begin();
		available.pop_front();
		allocated.push_back(it.raw());
		block = *it;
	}

	return block;
};

bool Data_Pool::Free_Block(void* base)
{
	bool ret = false;
	auto it = allocated.find(base);

	if (it != allocated.end())
	{
		allocated.erase(it);
		available.push_back(it.raw());
		ret = true;
	}

	return ret;
};

void Data_Pool::State(void)
{
	Log() << "  data pool " << block_size << " bytes:" << fmt::endl;

	Log() << "    available = " << available.size() << fmt::endl;
	for (auto it = available.begin(); it != available.end(); ++it)
	{
		Log() << "      0x" << fmt::hex << fmt::fill << (uint64_t)*it << fmt::endl;
	}

	Log() << "    allocated = " << allocated.size() << fmt::endl;
	for (auto it = allocated.begin(); it != allocated.end(); ++it)
	{
		Log() << "      0x" << fmt::hex << fmt::fill << (uint64_t)*it << fmt::endl;
	}
}

template<typename T, size_t S = _heap_size>
static void Pool_Init(T (&pool)[])
{
	for (int i = 0; i < S; ++i)
	{
		auto& e = pool[i].element;

		if (i > 0)
		{
			e.prev = &pool[i - 1].element;
		}

		if (i < (S - 1))
		{
			e.next = &pool[i + 1].element;
		}

		e.data = &pool[i].data;
	}
}

template<typename T, size_t S = _heap_size>
static void Pool_Init(lib::List<void*>::Element (&elements)[], T (&blocks)[])
{
	for (int i = 0; i < S; ++i)
	{
		auto& e = elements[i];
		auto& data = blocks[i];

		if (i > 0)
		{
			e.prev = &elements[i - 1];
		}

		if (i < (S - 1))
		{
			e.next = &elements[i + 1];
		}

		e.data = &data;
	}
}


void Heap::Data_Pools_Init(void)
{
	using dbl16 = Data_Block_List<16>;
	Pool_Init<dbl16>(_pool16);
	pool16.available.assign(&_pool16[0].element, &_pool16[_heap_size - 1].element);

	using dbl32 = Data_Block_List<32>;
	Pool_Init<dbl32>(_pool32);
	pool32.available.assign(&_pool32[0].element, &_pool32[_heap_size - 1].element);

	using dbl48 = Data_Block_List<48>;
	Pool_Init<dbl48>(_pool48);
	pool48.available.assign(&_pool48[0].element, &_pool48[_heap_size - 1].element);

	using dbl64 = Data_Block_List<64>;
	Pool_Init<dbl64>(_pool64);
	pool64.available.assign(&_pool64[0].element, &_pool64[_heap_size - 1].element);

	using dbl4k = Data_Block<_page_size>;
	Pool_Init<dbl4k, _l3_tables>(_mmu_pool, _mmu_pages);
	pool4k.available.assign(&_mmu_pool[0], &_mmu_pool[_heap_size - 1]);
}

// Heap class implementation
Heap::Heap(void)
	: pool16(16)
	, pool32(32)
	, pool48(48)
	, pool64(64)
	, pool4k(_page_size)
{
	Data_Pools_Init();
}

void* Heap::Alloc(size_t size)
{
	void* block = nullptr;

	if (size <= pool16.Block_Size() && pool16.Has_Free_Block())
	{
		block = pool16.Get_Block();
	}
	else
	if (size <= pool32.Block_Size() && pool32.Has_Free_Block())
	{
		block = pool32.Get_Block();
	}
	else
	if (size <= pool48.Block_Size() && pool48.Has_Free_Block())
	{
		block = pool48.Get_Block();
	}
	else
	if (size <= pool64.Block_Size() && pool64.Has_Free_Block())
	{
		block = pool64.Get_Block();
	}
	else
	if (size <= pool4k.Block_Size() && pool4k.Has_Free_Block())
	{
		block = pool4k.Get_Block();
	}

	return block;
}

void Heap::Free(void *base)
{
	if (pool16.Free_Block(base) ||
	    pool32.Free_Block(base) ||
	    pool48.Free_Block(base) ||
	    pool64.Free_Block(base) ||
	    pool4k.Free_Block(base))
	{}
}

void Heap::State(void)
{
	Log() << "Saturn heap state:" << fmt::endl;

	pool16.State();
	pool32.State();
	pool48.State();
	pool64.State();
	pool4k.State();
}

} // namespace core
} // namespace saturn
