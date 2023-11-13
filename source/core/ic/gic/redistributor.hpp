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

struct GicRedistRegs
{
	uint32_t ctrl;
	uint64_t typer;
	uint32_t waker;
	uint32_t igroupr0;
	uint32_t isenabler0;
	uint32_t icenabler0;
	uint32_t isactiver0;
	uint32_t icactiver0;
	uint32_t ipriorityr;
	uint32_t pidr2;
};

class GicRedistributor
{
public:
	enum Redist_Regs
	{
		CTRL		= 0x0000,
		TYPER		= 0x0008,
		WAKER		= 0x0014,
		IGROUPR0	= 0x0080,
		ISENABLER0	= 0x0100,
		ICENABLER0	= 0x0180,
		ICACTIVER0	= 0x0380,
		IPRIORITYR	= 0x0400,
		PIDR2		= 0xffe8,
		SGI_offset	= 0x10000	// 64K SGI region offset
	};

public:
	GicRedistributor();

public:
	void IRq_Enable(uint32_t id);
	void IRq_Disable(uint32_t id);

public:
	void Load_State(GicRedistRegs&);

private:
	void RW_Complete(void);

private:
	void Save_State(void);

private:
	MMap* Regs;
	GicRedistRegs& bootState;
};

}; // namespace core
}; // namespace saturn
