#include "gic/distributor.hpp"
#include <fault>

namespace saturn {
namespace core {

static GicDistributor* GicDist = nullptr;

void Interrupts_Init()
{
	// TBD: we should distinguish primary CPU and secondary.
	//      GIC dist should be initialized only for primary CPU.
	Log() << "create interrupts infrastructure" << fmt::endl;

	GicDist = new GicDistributor();
	if (nullptr == GicDist)
	{
		Fault("GIC distributor allocation failed");
	}
}

}; // namespace core
}; // namespace saturn
