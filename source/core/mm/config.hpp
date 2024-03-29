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

static const size_t _ptable_size = 512;		// 512 etries per-table
static const size_t _l0_size = 1;		// Single entry which covers 512GB, should be enough...

// Saturn follows the approach of static resources split and allocation,
// so this means that we create the configuration once during boot and
// do not perform any dynamic allocations during runtime. This gives us
// possibility to statically define number of translation tables based
// on hardware configuration. Each table size is 4KB, so we could save
// quite a lot RAM with fine-tunned configuration.
static const size_t _l3_tables = 16;

}; // namespace core
}; // namespace saturn
