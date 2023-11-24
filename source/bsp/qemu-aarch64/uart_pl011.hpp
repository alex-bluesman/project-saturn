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

#include <dev/uart>

namespace saturn {

// Forward declaration
class MMap;

namespace device {

struct Pl011Regs
{
	uint32_t cr;
	uint32_t imsc;
	uint32_t periphid[4];
	uint32_t pcellid[4];
};

// PL011 registers table:
enum Pl011_Regs {
	TDR		= 0x00,		// Data register
	FR		= 0x18,		// Flag register
	IBRD		= 0x24,		// Integer baud rate register
	FBRD		= 0x28,		// Fractional baud rate register
	LCR_H		= 0x2c,		// Line control register
	CR		= 0x30,		// Control register
	RIS		= 0x3c,		// Raw interrupt status register
	IMSC		= 0x38,		// Interrupt mask register
	ICR		= 0x44,		// Interrupt clear register
	PERIPHID0	= 0xfe0,	// Peripheral ID[0..3]
	PCELLID0	= 0xff0		// UART PrimeCell ID[0..3]
};

enum Reg_CR {
	Enable	= 1 << 0	// UART enable bit
};

enum Reg_FR {
	Busy	= 1 << 3	// UART busy bit
};

enum Pl011_INT {
	RX	= 1 << 4	// RX interrupt
};

class UartPl011 : public IUartDevice
{
public:
	UartPl011();
//	~UartPl011();

public:
	void Rx(uint8_t *buff, size_t len);
	void Tx(uint8_t *buff, size_t len);
	void HandleIRq(void);

public:
	void Load_State(Pl011Regs& regs);

private:
	// INT handling routine
	static void UartIRqHandler(uint32_t);
	static UartPl011* Self;

private:
	MMap* Regs;
};

}; // namespace device
}; // namespace saturn
