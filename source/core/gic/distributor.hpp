#pragma once

#include <mmap>

namespace saturn {
namespace core {

class GicDistributor
{
public:
	GicDistributor();

public:
	void SendSGI();

private:
	MMap* Regs;
	size_t linesNumber;
};

}; // namespace core
}; // namespace saturn
