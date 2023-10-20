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

#include "../gic/redistributor.hpp"

#include <mtrap>

namespace saturn {
namespace core {

class VirtGicRedistributor : public IVirtIO
{
public:
	VirtGicRedistributor(GicRedistributor&);
	~VirtGicRedistributor();

public:
	void Read(uint64_t addr, void* data, AccessSize size);
	void Write(uint64_t addr, void* data, AccessSize size);
	bool IRq_Enabled(uint32_t nr);

private:
	MTrap* mTrap;
	GicRedistributor& gicRedist;
	GicRedistRegs& vRedistState;
};

}; // namespace core
}; // namespace saturn
