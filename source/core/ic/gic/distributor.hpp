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

#include "config.hpp"

#include <mmap>

namespace saturn {
namespace core {

enum Dist_Regs
{
	CTRL		= 0x0000,
	TYPER		= 0x0004,
	IGROUPR		= 0x0080,
	ISENABLER	= 0x0100,
	ICENABLER	= 0x0180,
	ISACTIVER	= 0x0300,
	ICACTIVER	= 0x0380,
	IPRIORITYR	= 0x0400,
	ICFGR		= 0x0c00,
	IROUTER		= 0x6100,
	PIDR2		= 0xffe8,
};

struct GicDistRegs
{
	uint32_t ctrl;
	uint32_t typer;
	uint32_t igroupr[_gicd_nr_lines / 32];
	uint32_t isenabler[_gicd_nr_lines / 32];
	uint32_t icenabler[_gicd_nr_lines / 32];
	uint32_t isactiver[_gicd_nr_lines / 32];
	uint32_t icactiver[_gicd_nr_lines / 32];
	uint32_t ipriorityr[_gicd_nr_lines / 4];
	uint32_t icfgr[_gicd_nr_lines / 16];
	uint64_t irouter[_gicd_nr_lines];
	uint32_t pidr2;
};

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

public:
	void Load_State(GicDistRegs&);

private:
	inline void RW_Complete(void);

private:
	void Save_State(void);

private:
	MMap* Regs;
	size_t linesNumber;
	GicDistRegs& bootState;
};

}; // namespace core
}; // namespace saturn
