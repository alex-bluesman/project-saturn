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

#include <core/iic>
#include <core/ivirtic>

namespace saturn {
namespace core {

// Forward declaration
class CpuInterface;
class GicDistributor;
class GicRedistributor;
class GicVirtIC;

class IC_Core : public IIC, public IVirtIC
{
public:
	IC_Core();

// Saturn Core API:
public:
	void Local_IRq_Disable();
	void Local_IRq_Enable();
	void IRq_Enable(uint32_t);
	void IRq_Disable(uint32_t);

public:
	void Send_SGI(uint32_t targetList, uint8_t id);
	void Handle_IRq();
	void Register_IRq_Handler(uint32_t, IRqHandler);

// Guest VM API:
public:
	void Start_Virt_IC();
	void Stop_Virt_IC();
	void Inject_VM_IRq(uint32_t, vINTtype);

private:
	CpuInterface* CpuIface;
	GicDistributor* GicDist;
	GicRedistributor* GicRedist;

	GicVirtIC* GicVIC;

	IRqHandler (&IRq_Table)[];

private:
	static void Default_Handler(uint32_t);
};

}; // namespace core
}; // namespace saturn
