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

#include <mtrap>

namespace saturn {
namespace device {

class VirtUartPl011 : public IVirtIO
{
public:
	VirtUartPl011();
	~VirtUartPl011();

public:
	void Read(uint64_t addr, void* data, AccessSize size);
	void Write(uint64_t addr, void* data, AccessSize size);

private:
	MTrap* mTrap;
};

}; // namespace device
}; // namespace saturn
