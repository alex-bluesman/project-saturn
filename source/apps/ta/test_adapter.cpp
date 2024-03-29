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

#ifdef ENABLE_TESTING

#include <core/iconsole>
#include <core/icpu>
#include <core/iheap>
#include <core/iic>
#include <core/immu>

#include <lib/list>

#include <io>
#include <ringbuffer>

namespace saturn {
namespace apps {

using namespace core;


// INT test data
static const uint32_t _testINT = 10;
static uint32_t valueINT = 0;

static void SGI_Handler(uint32_t id)
{
	valueINT = _testINT;
	Log() << "     <- SGI " << id << " handler triggerred" << fmt::endl;
}

static bool INT_Smoke_Test(void)
{
	Log() << "  /register INT(" << _testINT << ") handler and send SGI" << fmt::endl;
	iIC().Register_IRq_Handler(_testINT, SGI_Handler);
	iIC().Send_SGI(1, _testINT);

	bool ret;

	if (_testINT == valueINT)
	{
		ret = true;
		Info() << "ta: " << __func__ << ": PASSED" << fmt::endl;
	}
	else
	{
		ret = false;
		Info() << "ta: " << __func__ << ": FAILED" << fmt::endl;
	}

	return ret;
}

static bool CPU_Smoke_Test(void)
{
	uint16_t cpu_id = iCPU().Id();

	Log() << "  /request CPU information" << fmt::endl;
	Log() << "    <- CPU id = " << cpu_id << fmt::endl;

	bool ret;

	if (0 == cpu_id)
	{
		ret = true;
		Info() << "ta: " << __func__ << ": PASSED" << fmt::endl;
	}
	else
	{
		ret = false;
		Info() << "ta: " << __func__ << ": FAILED" << fmt::endl;
	}

	return ret;
}

static bool HEAP_Smoke_Test(void)
{
	Log() << "  /call 'new' and 'delete'" << fmt::endl;
	uint64_t* val = new(uint64_t);

	bool ret;

	if (nullptr != val)
	{
		delete val;
		ret = true;
		Info() << "ta: " << __func__ << ": PASSED" << fmt::endl;
	}
	else
	{
		ret = false;
		Info() << "ta: " << __func__ << ": FAILED" << fmt::endl;
	}

	return ret;
}

static bool RINGBUFFER_Smoke_Test(void)
{
	// Test ring buffer
	RingBuffer<uint32_t, 4> buf(rb::full_overwrite);

	bool ret = true;

	Log() << "/try to out element from empty ring buffer" << fmt::endl;
	uint32_t val;
	if (buf.Out(val))
	{
		Log() << "  !empty ringbuffer returns value" << fmt::endl;
		ret = false;
	}

	Log() << "/fill ring buffer with data overwrite" << fmt::endl;
	for (uint32_t i = 0; i < 10; i++)
	{
		buf.In(i);
	}

	if (buf.Out(val) && val == 6)
	{
		ret = true;
		Info() << "ta: " << __func__ << ": PASSED" << fmt::endl;
	}
	else
	{
		ret = false;
		Info() << "ta: " << __func__ << ": FAILED" << fmt::endl;
	}

	return ret;
}

static bool MMU_Smoke_Test(void)
{
	uint64_t pa  = 0x41000000;
	uint64_t va1 = 0x41000000;
	uint64_t va2 = 0x42000000;

	// map the same physical address to different virtual addresses with
	// different block sizes
	core::iMMU().MemoryMap(va1, pa, BlockSize::L2_Block, MMapType::Normal);
	core::iMMU().MemoryMap(va2, pa, BlockSize::L3_Page, MMapType::Normal);

	// Ensure both addresses contain no random data
	Write<uint64_t>(va1, 0);
	Write<uint64_t>(va2, 0);

	uint64_t value = 0xcafe;
	bool ret;

	Write<uint64_t>(va1, value);

	if (Read<uint64_t>(va2) == value)
	{
		ret = true;
		Info() << "ta: " << __func__ << ": PASSED" << fmt::endl;
	}
	else
	{
		ret = false;
		Info() << "ta: " << __func__ << ": FAILED" << fmt::endl;
	}

	core::iMMU().MemoryUnmap(va1, BlockSize::L2_Block);
	core::iMMU().MemoryUnmap(va2, BlockSize::L3_Page);

	return ret;
}

static void LIST_Smoke_Test(void)
{
	//iHeap().State();

	Log() << "ta: " << __func__ << fmt::endl;
	lib::List<int> list;

	Log() << "/fill list with values" << fmt::endl;
	list.push_back(1);
	list.push_back(2);
	list.push_back(3);

	Log() << "/iterate list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	Log() << "/pop back single element:" << fmt::endl;
	list.pop_back();

	Log() << "/iterate list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	Log() << "/push back single element:" << fmt::endl;
	list.push_back(8);

	Log() << "/iterate list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	list.pop_back();
	list.pop_back();
	list.pop_back();

	Log() << "/iterate empty list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	Log() << "/push back single element:" << fmt::endl;
	list.push_back(9);

	Log() << "/iterate list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	list.pop_back();

	Log() << "/fill list with values" << fmt::endl;
	list.push_back(10);
	list.push_back(11);
	list.push_back(12);

	{
		lib::List<int>::Iterator it = list.begin();
		++it;
		it = list.erase(it);
		it = list.erase(it);
		it = list.erase(list.begin());
	}

	Log() << "/iterate list:" << fmt::endl;
	for (lib::List<int>::Iterator it = list.begin(); it != list.end(); ++it)
	{
		Log() << "  /val: " << *it << fmt::endl;
	}

	list.push_back(10);
	list.push_back(10);
	list.push_back(10);
}

void TA_Start(void)
{
	Log() << "app: testing adapter" << fmt::endl;

	INT_Smoke_Test();
	CPU_Smoke_Test();
	HEAP_Smoke_Test();
	RINGBUFFER_Smoke_Test();
	MMU_Smoke_Test();
	LIST_Smoke_Test();
}

}; // namespace apps
}; // namespace saturn

#else

namespace saturn {
namespace apps {

void TA_Start(void)
{}

}; // namespace apps
}; // namespace saturn

#endif // ENABLE_TESTING
