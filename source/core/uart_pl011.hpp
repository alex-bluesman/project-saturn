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


class UartPl011 : public UartDevice
{
public:
	UartPl011();
//	~UartPl011();

public:
	void Rx(uint8_t *buff, size_t len);
	void Tx(uint8_t *buff, size_t len);
	void EnableRx(void);
	void HandleIRq(void);

private:
	// INT handling routine
	static void UartIRqHandler(uint32_t);
	static UartPl011* Self;

private:
	MMap* Regs;
};

}; // namespace device
}; // namespace saturn
