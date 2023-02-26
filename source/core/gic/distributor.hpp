#pragma once

#include <mmap>

namespace saturn {
namespace core {

enum Gic_Dist_Regs
{
	PIDR2 = 0xffe8,
};

class GicDistributor
{
public:
	GicDistributor();

private:
	MMap* Regs;
};

}; // namespace core
}; // namespace saturn
