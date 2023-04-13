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

// MAIR contains the following attributes:
//
// index  value      description
// ------------------------------------------------
// 0_00   0000_0000  Strongly ordered/device nGnRnE
// 0_01   0100_0100  In/Out non-cachable
// 0_10   1010_1010  Write-through
// 0_11   1110_1110  Write-back
// 1_00   0000_0100  Device nGnRE
// 1_01   -
// 1_10   -
// 1_11   1111_1111  Write-back alloc

#define MAIR0_ATTR		0xeeaa4400UL
#define MAIR1_ATTR		0xff000004UL
#define MAIR_ATTR64		(MAIR1_ATTR << 32 | MAIR0_ATTR)

// We want to build the following mapping hierarchy:
//
//          L0       L1       L2        L3      Page
//  --------------------------------------------------
//  ... | 9 bits | 9 bits | 9 bits | 9 bits | 12 bits |
//  --------------------------------------------------
//     48       39       30        21

#define L1_ADDR_SHIFT		30			// Address shift for L1 page tables entries
#define L2_ADDR_SHIFT		21			// Address shift for L2 page tables entries
#define L3_ADDR_SHIFT		12			// Address shift for L3 page tables entries

// Initial value for TCR_EL2:
//
//                    TG1    |    EPD1    |   SH0_IS   | ORGN0_WBWA | IRGN0_WBWA | T0SZ = 48 bits
#define TCR_EL2 ( (2U << 30) | (1U << 23) | (3U << 12) | (1U << 10) | (1U << 8) | (64 - 48) )

#define TABLE_INDEX_MASK	0x1ff				// (512 - 1)


// See mm/ttable.hpp for explanation of bits encoding

// Translation Table Entry: Table
//                                   T           V
#define TTE_TABLE		((1U << 1) | (1U << 0))

// Translation Table Entry: Page
//                                   nG           AF           SH          AP          NS        ATTR          T           P
#define TTE_PAGE_DEVICE		((1U << 11) | (1U << 10) | (2U << 8) | (1U << 6) | (1U << 5) | (4U << 2) | (1U << 1) | (1U << 0))
#define TTE_PAGE_NORMAL		((1U << 11) | (1U << 10) | (3U << 8) | (1U << 6) | (1U << 5) | (7U << 2) | (1U << 1) | (1U << 0))

// Translation Table Entry pattern: block
//                                   nG           AF           SH          AP          NS        ATTR          P
#define TTE_BLOCK_DEVICE	((1U << 11) | (1U << 10) | (2U << 8) | (1U << 6) | (1U << 5) | (4U << 2) | (1U << 0))
#define TTE_BLOCK_NORMAL	((1U << 11) | (1U << 10) | (3U << 8) | (1U << 6) | (1U << 5) | (7U << 2) | (1U << 0))
