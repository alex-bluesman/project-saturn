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

// Forward declaration
class GicDistributor;
class VirtGicDistributor;
class GicRedistributor;
class VirtGicRedistributor;

enum class VICState
{
	Stopped,
	Started,
	Failed
};

class GicVirtIC
{
public:
	GicVirtIC(GicDistributor&, GicRedistributor&);

public:
	void Start(void);
	void Stop(void);
	void Inject_IRq(uint32_t nr);
	void Process_ISR(void);

private:
	void Set_LR(uint8_t id, uint64_t val);

private:
	// Maintenance INT handling routine
	static void MaintenanceIRqHandler(uint32_t);

private:
	GicDistributor& GicDist;
	VirtGicDistributor* vGicDist;

	GicRedistributor& GicRedist;
	VirtGicRedistributor* vGicRedist;

	VICState vState;

private:
	uint8_t nrLRs;
	uint16_t lrMask;
};

}; // namespace core
}; // namespace saturn
