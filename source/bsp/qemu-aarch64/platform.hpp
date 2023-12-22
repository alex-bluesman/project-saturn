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

#include "uart_pl011.hpp"
#include "virt_uart_pl011.hpp"

#include <bsp/ibsp>
#include <bsp/os_storage>

namespace saturn {
namespace bsp {

class QemuArm64Platform : public IBoardSupportPackage
{
public:
	QemuArm64Platform();

public:
	void Load_VM_Configuration(core::IVirtualMachineConfig&);
	void Start_Virtual_Devices(void);
	void Stop_Virtual_Devices(void);
	void Prepare_OS(struct AArch64_Regs&);

private:
	void Add_Image(uint64_t source, uint64_t target, size_t size);

private:
	OS_Type osType;

	// OS storage configuration
	size_t nrImages;
	OS_Storage_Entry (&osImages)[];

private:
	device::UartPl011* Uart;
	device::VirtUartPl011* VirtUart;
};

// Access to console
IBoardSupportPackage& iBSP(void);

}; // namespace bsp
}; // namespace saturn
