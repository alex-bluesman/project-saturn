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

#include <core/ivmm>

namespace saturn {
namespace core {

// TBD: should be global const comming from BSP
static const size_t _nrINTs = 256;

class VM_Configuration {
public:
	VM_Configuration()
	{
		for (int i = 0; i < (_nrINTs / 8); i++)
		{
			hwINTMask[i] = 0;
		}
	}

public:
	void Assign_Interrupt(size_t nr)
	{
		if (nr < _nrINTs)
		{
			hwINTMask[nr / 8] |= (1 << (nr % 8));
		}
	}

	bool Hardware_Interrupt(size_t nr)
	{
		bool ret = false;

		if (nr < _nrINTs)
		{
			ret = hwINTMask[nr / 8] &= (1 << (nr % 8));
		}

		return ret;
	}
public:
	uint8_t	hwINTMask[_nrINTs / 8];
};

class VM_Manager : public IVirtualMachineManager
{
public:
	VM_Manager();
	~VM_Manager();

public:
	void Start_VM();
	void Stop_VM();
	vm_state Get_VM_State();

public:
	bool Guest_IRq(uint32_t nr);

private:
	vm_state		vmState;

private:
	// TBD: could not fit heap frame
	VM_Configuration*	vmConfig;
};

}; // namespace core
}; // namespace saturn
