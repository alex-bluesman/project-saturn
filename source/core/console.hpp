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
#include <ringbuffer>
#include <system>

namespace saturn {
namespace core {

using namespace device;

static const size_t _rx_size = 16;
static const size_t _tx_size = 8 * _page_size; // Reserve 32K buffer to cache log messages on boot

enum systemKeys
{
	cmdMode = 0x09,		// 'CTRL + i' enter command mode
	cmdBeep = 0x62,		// 'b': make a beep for testing purposes
	cmdShutdown = 0x73	// 's': shutdown running VM
};

class Console : public IConsole
{
public:
	Console();

// Output interface
public:
	Console& operator<<(fmt format);
	Console& operator<<(llevel level);

	Console& operator<<(char const *msg);
	Console& operator<<(char c);

	Console& operator<<(int32_t num);
	Console& operator<<(uint32_t num);
	Console& operator<<(int64_t num);
	Console& operator<<(uint64_t num);

	Console& operator<<(size_t num);

// Input interface
public:
	char GetChar(iomode mode);

// UART driver interface
public:
	void RegisterUart(IUartDevice&);
	bool RxChar(char sym);
	bool RxFifoEmpty();

public:
	void SetLevel(llevel);

private:
	Console& SignedToStr(int64_t num, uint8_t fillSize = 0);
	Console& UnsignedToStr(uint64_t num, uint8_t fillSize = 0);

	inline void ShowLogLevel(void);

private:
	bool isActive;
	bool isHex;
	bool isFill;
	bool isLevel;

	IUartDevice* uart;

	llevel currentMsgLevel;
	llevel consoleLevel;

	// RX/TX buffering
	RingBuffer<char, _rx_size> *rxBuffer;
	RingBuffer<char, _tx_size> *txBuffer;

	bool cmdMode;
};

}; // namespace core
}; // namespace saturn
