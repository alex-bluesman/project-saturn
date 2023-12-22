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

namespace saturn {
namespace bsp {

using namespace core;

// QEMU AArch64 BSP instance
static QemuArm64Platform* QemuArm64_BSP = nullptr;

// TBD: size of OS storage
static const size_t _nrImages = 3;
// Local table to keep information about guest OS images
static OS_Storage_Entry _osImages[_nrImages];

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
	, nrImages(0)
	, osImages(_osImages)
{
	Uart = new device::UartPl011();
	iConsole().RegisterUart(*Uart);
}

void QemuArm64Platform::Load_VM_Configuration(core::IVirtualMachineConfig& vmConfig)
{
	vmConfig.VM_Assign_Memory_Region({0x08080000, 0x08080000, 0x00020000, core::MMapType::Device});	// GIC ITS
	vmConfig.VM_Assign_Memory_Region({0x09010000, 0x09010000, 0x00001000, core::MMapType::Device});	// PL031 RTC
	vmConfig.VM_Assign_Memory_Region({0x09030000, 0x09030000, 0x00001000, core::MMapType::Device});	// PL061 GPIO controller
	vmConfig.VM_Assign_Memory_Region({0x0a000000, 0x0a000000, 0x00004000, core::MMapType::Device});	// Vi

	vmConfig.VM_Assign_Memory_Region({0x40000000, 0x40000000, 0x20000000, core::MMapType::Normal});	// Normal RAM memory

	vmConfig.VM_Set_Entry_Address(0x41000000);

	vmConfig.VM_Assign_Interrupt(0);	// SGI
	vmConfig.VM_Assign_Interrupt(1);	// SGI
	vmConfig.VM_Assign_Interrupt(2);	// SGI
	vmConfig.VM_Assign_Interrupt(3);	// SGI
	vmConfig.VM_Assign_Interrupt(4);	// SGI
	vmConfig.VM_Assign_Interrupt(5);	// SGI
	vmConfig.VM_Assign_Interrupt(6);	// SGI

	vmConfig.VM_Assign_Interrupt(23);	// AMBA clock
	vmConfig.VM_Assign_Interrupt(27);	// Virtual generic timer
	vmConfig.VM_Assign_Interrupt(34);	// VirtIO

	osType = OS_Type::Linux;

	Add_Image(0x7e000000, 0x41000000, 0x0149c000);		// Kernel
	Add_Image(0x7f500000, 0x43000000, 0x00002000);		// Device tree
	Add_Image(0x7f510000, 0x50000000, 0x00567000);		// Root filesystem

#if 0
	// Asteroid
	vmConfig->VM_Add_Memory_Region({0x41000000, 0x41000000, BlockSize::L2_Block, MMapType::Normal});
	vmConfig->VM_Set_Entry(0x41000000);

	vmConfig->VM_Assign_Interrupt(27);	// Virtual generic timer

	vmConfig->VM_Add_Image(0x7e000000, 0x41000000, 0x00007000);	// Asteroid kernel
#endif	
}

void QemuArm64Platform::Add_Image(uint64_t source, uint64_t target, size_t size)
{
	if (nrImages < _nrImages)
	{
		osImages[nrImages].sourcePA = source;
		osImages[nrImages].targetPA = target;
		osImages[nrImages].size = size;

		nrImages++;
	}
}

void QemuArm64Platform::Prepare_OS(struct AArch64_Regs& guestContext)
{
	if (OS_Type::Linux == osType)
	{
		guestContext.x0 = 0x43000000;		// Device tree address
	}

	// Load OS images
	for (size_t i = 0; i < nrImages; i++)
	{
		OS_Storage_Entry& entry = osImages[i];
		Memory_Region sourceRegion = {entry.sourcePA, entry.sourcePA, entry.size, MMapType::Normal};
		Memory_Region targetRegion = {entry.targetPA, entry.targetPA, entry.size, MMapType::Normal};

		iMMU().MemoryMap(sourceRegion);
		iMMU().MemoryMap(targetRegion);

		Info() << "vmm: copy OS binary from storage to 0x" << fmt::fill << fmt::hex << entry.targetPA << ": ";

		MCopy<uint8_t>((void *)entry.sourcePA, (void *)entry.targetPA, entry.size);

		Raw() << "OK" << fmt::endl;

		iMMU().MemoryUnmap(targetRegion);
		iMMU().MemoryUnmap(sourceRegion);
	}
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
