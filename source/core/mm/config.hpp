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

// Saturn follows the approach of static resources split and allocation,
// so this means that we create the configuration once during boot and
// do not perform any dynamic allocations during runtime. This gives us
// possibility to statically define the sizes of translation tables based
// on hardware configuration. Each table size is 4KB, so we could save
// quite a lot RAM with fine-tunned configuration.

// TBD: should be reworked to avoid compile time hardcoding
// Page tables parameters:
static const size_t _l0_size = 1;		// Single entry
static const size_t _l1_size = 4;		// 4GB of mapped address space
static const size_t _l2_size = 512;		// 1GB of memory per-table
static const size_t _l3_size = 512;		// 2MB of memory per-table

// Number of tables for hypervisor level-3 page mapping. Hypervisor uses them
// to map I/O peripheral regions. And due to only certain devices are used
// (like UART and GIC), no need to create many tables
static const size_t _l3_tables = 16;

}; // namespace core
}; // namespace saturn
