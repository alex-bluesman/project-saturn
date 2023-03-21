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

#include <core/iconsole>
#include <dev/uart>

namespace saturn {
namespace core {

using namespace device;

class Console : public IConsole
{
//public:
//	void * operator new(long unsigned int size);
//	void operator delete(void * p, unsigned long){};

public:
	Console();
	Console(UartDevice&);

public:
	Console& operator<<(fmt format);
	Console& operator<<(llevel level);

	Console& operator<<(char const *msg);

	Console& operator<<(int32_t num);
	Console& operator<<(uint32_t num);
	Console& operator<<(int64_t num);
	Console& operator<<(uint64_t num);

	Console& operator<<(size_t num);

public:
	void RegisterUart(UartDevice&);

private:
	Console& SignedToStr(int64_t num, uint8_t fillSize = 0);
	Console& UnsignedToStr(uint64_t num, uint8_t fillSize = 0);

	inline void ShowLogLevel(void);

private:
	bool isActive;
	bool isHex;
	bool isFill;
	bool isLevel;

	UartDevice* uart;

	llevel currentMsgLevel;
};

}; // namespace core
}; // namespace saturn
