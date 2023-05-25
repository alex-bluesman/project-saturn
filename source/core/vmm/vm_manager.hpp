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

class VM_Manager : public IVirtualMachineManager
{
public:
	VM_Manager();
	~VM_Manager();

public:
	void Start_VM();
	void Stop_VM();
	vm_state Get_VM_State();

private:
	vm_state	vmState;
};

}; // namespace core
}; // namespace saturn
