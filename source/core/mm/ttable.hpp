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
#include <system>

namespace saturn {
namespace core {

//                Level 0, 1, 2: Table
// ----------------------------------------------------
// bit[0]      - indicates if descriptor is valid
// bit[1]      - descriptor type: table(1) or memory block(0)
// bits[11:2]  - ignored
// bits[47:12] - next-level table address [47:12]
// bits[51:48] - RES0
// bits[58:52] - ignored
// bit[59]     - privileged execute-never PXNTable
// bit[60]     - execute-never control UXNTable
// bits[62:61] - access permissions limit for subsequent level APTable
// bit[63]     - non-secure memory access NSTable
using lpae_table_t = struct __packed {
	// For valid table should be: valid = 1, type = 1
	uint64_t valid:1;
	uint64_t type:1;

	uint64_t ignored0:10;

	// Next-level table address
	uint64_t addr:36;

	uint64_t res0:4;
	uint64_t ignored1:7;

	// Upper attributes
	uint64_t pxn:1;
	uint64_t xn:1;
	uint64_t ap:2;
	uint64_t ns:1;
};

//                Level 0, 1, 2: block
// ----------------------------------------------------
//
// bit[0]      - indicates if descriptor is valid
// bit[1]      - descriptor type: table(1) or memory block(0)
//
// bits[11:2]  - lower block attributes
//   bits[4:2] - attribute index AttrIndx[2:0] in MAIR
//   bit[5]    - non-secure bit
//   bits[7:6] - data access permissions AP[2:1]
//   bits[9:8] - shareability field
//   bit[10]   - access flag
//   bit[11]   - not global bit
//
// bits[15:12] - RES0
// bit[16]     - nT bit
// bits[20:17] - RES0
//
// bits[47:21] - output address
//   bits[47:30] - for level 1
//   bits[47:21] - for level 2
//
// bits[49:48] - RES0
//
// bits[63:50] - upper block attributes
//   bit[50]     - guarded page field
//   bit[51]     - dirty bit modifier
//   bit[52]     - contiguous bit
//   bits[54:53] - execute never
//   bits[58:55] - RES0
//   bits[62:59] - page based hardware attributes
//   bit[63]     - IGNORED/reserved for System MMU
using lpae_block_t = struct __packed {
	// For valid page should be: valid = 1, type = 1
	uint64_t valid:1;
	uint64_t type:1;

	// Low attributes
	uint64_t attr:3;
	uint64_t ns:1;
	uint64_t ap:2;
	uint64_t sh:2;
	uint64_t af:1;
	uint64_t ng:1;
	
	uint64_t res0:4;
	uint64_t nt:1;
	uint64_t res1:4;

	// Output address
	uint64_t addr:27;

	uint64_t res2:3;

	// Upper attributes
	uint64_t dbm:1;
	uint64_t cont:1;
	uint64_t pxn:1;
	uint64_t uxn:1;
	uint64_t ignored0:4;
	uint64_t pbha:4;
	uint64_t ignored1:1;
};

//         Level 3: page
// ----------------------------------------------------
//
// bit[0]      - indicates if descriptor is valid
// bit[1]      - descriptor type: page(1)
//
// bits[11:2]  - lower block attributes
//   bits[4:2] - attribute index AttrIndx[2:0] in MAIR
//   bit[5]    - non-secure bit
//   bits[7:6] - data access permissions AP[2:1]
//   bits[9:8] - shareability field
//   bit[10]   - access flag
//   bit[11]   - not global bit
//
// bits[47:12] - output address
// bits[49:48] - RES0
//
// bits[63:50] - upper block attributes
//   bit[50]     - guarded page field
//   bit[51]     - dirty bit modifier
//   bit[52]     - contiguous bit
//   bits[54:53] - execute never
//   bits[58:55] - RES0
//   bits[62:59] - page based hardware attributes
//   bit[63]     - IGNORED/reserved for System MMU
using lpae_page_t = struct __packed {
	// For valid page should be: valid = 1, type = 1
	uint64_t valid:1;
	uint64_t type:1;

	// Low attributes
	uint64_t attr:3;
	uint64_t ns:1;
	uint64_t ap:2;
	uint64_t sh:2;
	uint64_t af:1;
	uint64_t ng:1;

	// Output address
	uint64_t addr:36;

	uint64_t res0:3;

	// Upper attributes
	uint64_t dbm:1;
	uint64_t cont:1;
	uint64_t pxn:1;
	uint64_t uxn:1;
	uint64_t ignored0:4;
	uint64_t pbha:4;
	uint64_t ignored1:1;
};

static const size_t _l1_addr_shift = 30;	// 1GB granularity
static const size_t _l2_addr_shift = 21;	// 2MB granularity
static const size_t _l3_addr_shift = 12;	// 4KB granularity

enum LPAE_Type
{
	Block = 0,
	Table = 1,
	Page = 1
};

}; // namespace core
}; // namespace saturn
