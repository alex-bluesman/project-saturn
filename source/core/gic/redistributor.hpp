#pragma once

#include <mmap>

namespace saturn {
namespace core {

class GicRedistributor
{
public:
	GicRedistributor();

private:
	MMap* Regs;
};

}; // namespace core
}; // namespace saturn
