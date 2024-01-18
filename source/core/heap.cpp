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

// (!) Heap pre-allocated data pools, must be used as carefully
static Data_Block<16> _pool16[_heap_size] __section(".heap");
static Data_Block<32> _pool32[_heap_size] __section(".heap");
static Data_Block<48> _pool48[_heap_size] __section(".heap");
static Data_Block<64> _pool64[_heap_size] __section(".heap");

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

		e.data = pool[i].data;
	}
}

void Heap::Data_Pools_Init(void)
{
	using db16 = Data_Block<16>;
	Pool_Init<db16>(_pool16);
	pool16.available.assign(&_pool16[0].element, &_pool16[_heap_size - 1].element);

	using db32 = Data_Block<32>;
	Pool_Init<db32>(_pool32);
	pool32.available.assign(&_pool32[0].element, &_pool32[_heap_size - 1].element);

	using db48 = Data_Block<48>;
	Pool_Init<db48>(_pool48);
	pool48.available.assign(&_pool48[0].element, &_pool48[_heap_size - 1].element);

	using db64 = Data_Block<64>;
	Pool_Init<db64>(_pool64);
	pool64.available.assign(&_pool64[0].element, &_pool64[_heap_size - 1].element);
}

// Heap class implementation
Heap::Heap(void)
	: pool16(16)
	, pool32(32)
	, pool48(48)
	, pool64(64)
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

	return block;
}

void Heap::Free(void *base)
{
	if (pool16.Free_Block(base) ||
	    pool32.Free_Block(base) ||
	    pool48.Free_Block(base) ||
	    pool64.Free_Block(base))
	{}
}

void Heap::State(void)
{
	Log() << "Saturn heap state:" << fmt::endl;

	pool16.State();
	pool32.State();
	pool48.State();
	pool64.State();
}

} // namespace core
} // namespace saturn
