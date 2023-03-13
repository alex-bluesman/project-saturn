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

#include <basetypes>

namespace saturn {
namespace core {

using IRHandler = void(*)(uint8_t);

// Forward declaration
class CpuInterface;
class GicDistributor;
class GicRedistributor;

class IC_Core
{
public:
	IC_Core();

public:
	void Local_IRq_Disable();
	void Local_IRq_Enable();
	void Send_SGI(uint32_t targetList, uint8_t id);
	void Handle_IRq();

private:
	CpuInterface* CpuIface;
	GicDistributor* GicDist;
	GicRedistributor* GicRedist;
	IRHandler (&IRq_Table)[];

private:
	static void Default_Handler(uint8_t);
};

}; // namespace core
}; // namespace saturn
