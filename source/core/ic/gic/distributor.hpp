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

#include <mmap>

namespace saturn {
namespace core {

class GicDistributor
{
public:
	GicDistributor();

public:
	void Send_SGI(uint32_t targetList, uint8_t id);

public:
	size_t Get_Max_Lines();
	void IRq_Enable(uint32_t);
	void IRq_Disable(uint32_t);

private:
	inline void RW_Complete(void);

private:
	MMap* Regs;
	size_t linesNumber;
};

}; // namespace core
}; // namespace saturn
