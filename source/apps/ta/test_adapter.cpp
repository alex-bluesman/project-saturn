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

#include <core/iconsole>
#include <core/icpu>
#include <core/iheap>
#include <core/iirq>

#include <ringbuffer>

namespace saturn {
namespace apps {

using namespace core;

static const uint32_t _testIRQ = 10;

static void SGI0_Handler(uint32_t id)
{
	Log() << "     <- SGI " << _testIRQ << " handler triggerred" << fmt::endl;
}

void TA_Start(void)
{
	Log() << "app: testing adapter" << fmt::endl;

	Log() << "  //register INT(" << _testIRQ << ") handler and send SGI" << fmt::endl;
	IC().Register_IRq_Handler(_testIRQ, SGI0_Handler);
	IC().Send_SGI(1, _testIRQ);

	Log() << "  //request CPU information" << fmt::endl;
	Log() << "    <- CPU id = " << This_CPU().Id() << fmt::endl;

	Log() << "  //request heap state" << fmt::endl;
	Allocator().State();

	Log() << "  //call 'new' and 'delete'" << fmt::endl;
	uint64_t* val = new(uint64_t);
	Log() << "    <-- Got allocation at: 0x" << fmt::hex << fmt::fill << (uint64_t)val << fmt::endl;
	delete val;

	// Test ring buffer
	RingBuffer<uint32_t, 4> buf(rb::full_overwrite);

	for (uint32_t i = 0; i < 4; i++)
	{
		uint32_t val;
		if (buf.Out(val))
		{
			Log() << "empty ringbuffer returns value, test failed!" << fmt::endl;
		}
	}

	for (uint32_t i = 0; i < 10; i++)
	{
		buf.In(i);
	}

	for (uint32_t i = 0; i < 5; i++)
	{
		uint32_t val;
		if (buf.Out(val))
		{
			Log() << "ringbuffer out: " << val << fmt::endl;
		}
	}

}

}; // namespace apps
}; // namespace saturn
