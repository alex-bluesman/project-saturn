#pragma once

#include <dev/uart>

namespace saturn {
namespace device {

class UartPl011 : public UartDevice
{
public:
	UartPl011();
//	~UartPl011();
public:
	void Init(void);
	void Rx(uint8_t *buff, size_t len);
	void Tx(uint8_t *buff, size_t len);
};

}; // namespace device
}; // namespace saturn
