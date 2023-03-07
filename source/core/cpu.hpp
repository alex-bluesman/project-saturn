#pragma once

#include <icpu>

namespace saturn {
namespace core {

class CpuInfo : public ICpuInfo
{
public:
	CpuInfo();

public:
	uint64_t Id();

private:
	uint64_t CoreId;
};

}; // namespace core
}; // namespace saturn
