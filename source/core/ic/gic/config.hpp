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

// According to the GIC manual:
//
// INTID       Interrupt Type
// --------------------------
//    0 - 15         SGI
//   16 - 31         PPI
//   32 - 1019       SPI
// 1020 - 1023     Special
// 1024 - 1055        -
// 1056 - 1119       PPI
// 1120 - 4095        -

// According to the GICv3 Architecture Specification, the maximal SPI number could be 1020.
// TBD: due to we do not support ESPIs and LPIs, let's limit table size to 1020.
static const size_t _maxIRq = 1020;

// Basic constants to avoid magic numbers
static const size_t _nrSGIs = 16;
static const size_t _nrPPIs = 16;
static const size_t _nrSPIs = 988;

static const size_t _firstSGI = 0;
static const size_t _firstPPI = _firstSGI + _nrSGIs;
static const size_t _firstSPI = _firstPPI + _nrPPIs;

// TBD: let's limit SPI number to QEMU AArch64 configuration - 256 lines
static const size_t _gicd_nr_lines = 256;

}; // namespace core
}; // namespace saturn
