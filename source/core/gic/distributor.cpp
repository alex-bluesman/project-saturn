#include "distributor.hpp"
#include <iconsole>
#include <platform/qemuarm64>

namespace saturn {
namespace core {

GicDistributor::GicDistributor()
{
	Regs = new MMap(MMap::IO_Region(_gic_dist_addr));

	Log() << "GIC" << fmt::endl;

	uint32_t ArchRev = (Regs->Read<uint32_t>(Gic_Dist_Regs::PIDR2) >> 4) & 0x0f;
	Log() << "    found GICv" << ArchRev << fmt::endl;
}

}; // namespace core
}; // namespace saturn
