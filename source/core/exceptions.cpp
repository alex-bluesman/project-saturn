#include <arm64/registers>
#include <iconsole>

extern saturn::uint64_t saturn_vector;

namespace saturn {
namespace core {

void Exceptions_Init()
{
	WriteArm64Reg(VBAR_EL2, (uint64_t)&saturn_vector);
}

static void Print_Registers(struct AArch64_Regs* Regs)
{
	Log() << fmt::endl << "AArch64 general purpose registers state:" << fmt::endl;

	Log() << fmt::fill << fmt::hex
	      <<  "  x0  = 0x" << (int32_t)Regs->x0 << "  x1  = 0x" << (int32_t)Regs->x1
	      <<  "  x2  = 0x" << (int32_t)Regs->x2 << "  x3  = 0x" << (int32_t)Regs->x3
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x4  = 0x" << (int32_t)Regs->x4 << "  x5  = 0x" << (int32_t)Regs->x5
	      <<  "  x6  = 0x" << (int32_t)Regs->x6 << "  x7  = 0x" << (int32_t)Regs->x7
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x8  = 0x" << (int32_t)Regs->x8 <<  "  x9  = 0x" << (int32_t)Regs->x9
	      <<  "  x10 = 0x" << (int32_t)Regs->x10 << "  x11 = 0x" << (int32_t)Regs->x11
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x12 = 0x" << (int32_t)Regs->x12 << "  x13 = 0x" << (int32_t)Regs->x13
	      <<  "  x14 = 0x" << (int32_t)Regs->x14 << "  x15 = 0x" << (int32_t)Regs->x15
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x16 = 0x" << (int32_t)Regs->x16 << "  x17 = 0x" << (int32_t)Regs->x17
	      <<  "  x18 = 0x" << (int32_t)Regs->x18 << "  x19 = 0x" << (int32_t)Regs->x19
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x20 = 0x" << (int32_t)Regs->x20 << "  x21 = 0x" << (int32_t)Regs->x21
	      <<  "  x22 = 0x" << (int32_t)Regs->x22 << "  x23 = 0x" << (int32_t)Regs->x23
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x24 = 0x" << (int32_t)Regs->x24 << "  x25 = 0x" << (int32_t)Regs->x25
	      <<  "  x26 = 0x" << (int32_t)Regs->x26 << "  x27 = 0x" << (int32_t)Regs->x27
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  x28 = 0x" << (int32_t)Regs->x28 << "  x29 = 0x" << (int32_t)Regs->x29
	      <<  "  lr =  0x" << (int32_t)Regs->lr <<  "  sp =  0x" << (int32_t)Regs->sp
	      << fmt::endl;
	Log() << fmt::fill << fmt::hex
	      <<  "  pc  = 0x" << (int32_t)Regs->pc
	      << fmt::endl;

	Log() << fmt::endl << "AArch64 control registers state:" << fmt::endl;

	Log() << fmt::fill << fmt::hex
	      <<  "  cpsr = 0x" << (int32_t)Regs->cpsr
	      << fmt::endl;
}

static void Fault_Mode(struct AArch64_Regs* Regs)
{
	Print_Registers(Regs);

	Log() << fmt::endl << "Fault Mode: execution stopped" << fmt::endl;

	for (;;);
}

}; // namespace core
}; // namespace saturn

extern "C" {

using namespace saturn;

void Sync_Abort(struct AArch64_Regs* Regs)
{
	core::Log() << "Exception: Synchronous Abort" << core::fmt::endl;

	core::Fault_Mode(Regs);
}

void System_Error(struct AArch64_Regs* Regs)
{
	core::Log() << "Exception: System Error" << core::fmt::endl;

	core::Fault_Mode(Regs);
}


}
