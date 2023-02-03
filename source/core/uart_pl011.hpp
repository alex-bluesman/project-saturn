#pragma once

#include <dev/uart>


namespace saturn {

// Forward declaration
class MMap;

namespace device {


class UartPl011 : public UartDevice
{
public:
	UartPl011();
//	~UartPl011();

public:
	void Rx(uint8_t *buff, size_t len);
	void Tx(uint8_t *buff, size_t len);

private:
	MMap* Reg;
};

}; // namespace device
}; // namespace saturn
