# General

Asteroid is a small AArch64 bare metal application intended to run in EL1 mode and
check basic environment functionality. The main use case is to test Saturn hypervisor
Virtual Machine environment and features.

For today Asteroid is built using C++ interfaces and parts of Saturn implementation.
Asteroid contains:
 - Boot code to run as bare-metal application
 - Console (fully compatible with Saturn API)
 - UART PL011 driver (without RX flow)

# License
 
Licensed under the MIT License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

    http://opensource.org/licenses/MIT
