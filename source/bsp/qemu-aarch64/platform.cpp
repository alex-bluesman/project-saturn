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

#include "platform.hpp"

#include <core/iconsole>
#include <core/ivmm>
#include <mops>

#include "generated/saturn_config.hpp"

namespace saturn {
namespace bsp {

using namespace core;

// QEMU AArch64 BSP instance
static QemuArm64Platform* QemuArm64_BSP = nullptr;

IBoardSupportPackage& iBSP(void)
{
	return *QemuArm64_BSP;
}

void BSP_Init(void)
{
	QemuArm64_BSP = new QemuArm64Platform;
}

QemuArm64Platform::QemuArm64Platform()
	: Uart(nullptr)
	, VirtUart(nullptr)
{
	Uart = new device::UartPl011();
	iConsole().RegisterUart(*Uart);

	osStorage = new OS_Storage;
}

void QemuArm64Platform::Load_VM_Configuration(core::IVirtualMachineConfig& vmConfig)
{
	// Load generated configuration
	generated::VM_Configuration(vmConfig);
	generated::OS_Storage_Configuration(*osStorage);
}

void QemuArm64Platform::Prepare_OS(struct AArch64_Regs& guestContext)
{
	if (osStorage->Get_OS_Type() == OS_Type::Linux)
	{
		guestContext.x0 = 0x43000000;		// Device tree address
	}

	osStorage->Load_Images();
}

void QemuArm64Platform::Start_Virtual_Devices(void)
{
	if (nullptr == VirtUart)
	{
		VirtUart = new device::VirtUartPl011(*Uart);
	}
}

void QemuArm64Platform::Stop_Virtual_Devices(void)
{
	// TBD: shutdown Uart
}

}; // namespace device
}; // namespace saturn
