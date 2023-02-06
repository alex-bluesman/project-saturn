#include <arm64/registers>
#include <iconsole>

extern saturn::uint64_t saturn_vector;

namespace saturn {
namespace core {

void Exceptions_Init()
{
	WriteArm64Reg(VBAR_EL2, (uint64_t)&saturn_vector);
}

void Fault_Mode(void)
{
	Log() << "Fault Mode: Execution stopped" << fmt::endl;
	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" {

using namespace saturn::core;

void Sync_Abort()
{
	Log() << "Exception: Synchronous Abort" << fmt::endl;
	Fault_Mode();
}

}
