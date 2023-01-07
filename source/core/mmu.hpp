#pragma once

#include <immu>
#include <system>

namespace saturn {
namespace core {

// Page definitions:
static const size_t _page_size = 4096;
static const size_t _page_mask = (_page_size - 1);
static const size_t _page_base = (~_page_mask);

// TBD: should be reworked to avoid compile time hardcoding
// Page tables parameters:
static const size_t _l0_size = 1;	// Single entry
static const size_t _l1_size = 4;	// 4GB of mapped address space
static const size_t _l2_size = 512;	// 1GB of memory per-table
static const size_t _l3_size = 512;	// 2MB of memory per-table

//         Level 3: page
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
// bits[47:12] - output address
//   bits[47:30] - for level 1
//   bits[47:21] - for level 2
//   bits[47:12] - for level 3
//
// bits[49:48] - RES0
// bit[50]     - GP ?
// bit[51]     - RES0
//
// bits[63:52] - upper block attributes
//   TBD:

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


// TBD:
//                Level 0, 1, 2: block
// ----------------------------------------------------

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

// 64-bit blob for LPAE table entry
using tt_desc_t = uint64_t;


class MemoryManagementUnit : public IMemoryManagementUnit {
public:
	MemoryManagementUnit();

public:
	void MemoryMap(uint64_t base_addr, size_t size, MMapType type);
	void MemoryUnmap(uint64_t base_addr, size_t size);

private:
	uint16_t FindFreeL3(void);
	void MapPage(uint64_t base_addr, MMapType type);


private:
	tt_desc_t (&PTable0)[];
	tt_desc_t (&PTable1)[];
	tt_desc_t (&PTable2)[][_l2_size];
	tt_desc_t (&PTable3)[][_l3_size];

	uint64_t FreeMaskL3;
};

}; // namespace core
}; // namespace saturn
