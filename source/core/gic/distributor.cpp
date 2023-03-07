#include "distributor.hpp"

#include <arm64/registers>
#include <icpu>
#include <fault>
#include <platform/qemuarm64>

namespace saturn {
namespace core {

enum Gic_Dist
{
	CTRL		= 0x0000,
	TYPER		= 0x0004,
	ICENABLER	= 0x0180,
	ICACTIVER	= 0x0380,
	IPRIORITYR	= 0x0400,
	ICFGR		= 0x0c00,
	IROUTER		= 0x6100,
	PIDR2		= 0xffe8,
};

GicDistributor::GicDistributor()
{
	// TBD: check return value
	Regs = new MMap(MMap::IO_Region(_gic_dist_addr));

	uint32_t ArchRev = (Regs->Read<uint32_t>(Gic_Dist::PIDR2) >> 4) & 0x0f;
	Log() << "  /found GICv" << ArchRev << fmt::endl;

	if (ArchRev != 3)
	{
		Fault("gic: unsupported version");
	}

	// Disable distributor until setup is complete
	Regs->Write<uint32_t>(Gic_Dist::CTRL, 0);

	// Maximum SPI INTID could be calculated by: 32 * (N + 1)
	size_t ITLinesNumber = Regs->Read<uint32_t>(Gic_Dist::TYPER) & 0x1f;
	size_t linesNumber = 32 * (ITLinesNumber + 1);
	Log() << "  /found " << linesNumber << " interrupt lines" << fmt::endl;

	// So we have now:
	//   SGIs: 0  - 15
	//   PPIs: 16 - 31
	//   SPIs: 32 - SPINumber

	// First let's put SPIs active low
	for (size_t i = 32; i < linesNumber; i += 16) // 2 bits per line, so 16 lines per register
	{
		Regs->Write<uint32_t>(Gic_Dist::ICFGR + (i / 16) * 4, 0);
	}

	// Set default SPI priority to the middle of the possible range
	for (size_t i = 32; i < linesNumber; i += 4)
	{
		Regs->Write<uint32_t>(Gic_Dist::IPRIORITYR + (i / 4) * 4, 0x80808080);
	}

	for (size_t i = 32; i < linesNumber; i += 32)
	{
		Regs->Write<uint32_t>(Gic_Dist::ICENABLER + (i / 32) * 4, 0xffffffff);
		Regs->Write<uint32_t>(Gic_Dist::ICACTIVER + (i / 32) * 4, 0xffffffff);
	}

	// Wait until write operations are done
	// TBD: should we introduce timeout?
	while (Regs->Read<uint32_t>(Gic_Dist::CTRL) & (1 << 31));

	// Enable the distributor
	Regs->Write<uint32_t>(Gic_Dist::CTRL, (1 << 4) | (1 << 1) | (1 << 0)); // EnableGrp0 | EnableGrp1NS | ARE_NS

	// TBD: let's assume that we have only 16 cores maximum, so the only Aff0 is used
	uint32_t affinity = This_CPU()->Id() & 0x7;

	// Disable broadcast rounting
	affinity &= ~(1 << 31);

	// Route all the SPIs to current CPU
	for (size_t i = 32; i < linesNumber; i++)
	{
		Regs->Write<uint32_t>(Gic_Dist::IROUTER, affinity);
	}

	Log() << "  /distributor initialized" << fmt::endl;
}

void GicDistributor::SendSGI()
{
	// Generate SGI nr 0 to current CPU
	WriteICCReg(ICC_SGI1R_EL1, 1);
}

}; // namespace core
}; // namespace saturn
