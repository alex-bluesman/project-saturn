# General

Project Saturn is a hypervisor for Aarch64 platforms.

Copyright (c) 2022
Alexander Smirnov <alex.bluesman.smirnov@gmail.com>

# QEMU Aarch64 Target

### Prerequisites

1. Toolchain: `aarch64-unknown-linux-gnu-` from linaro.org website
2. QEMU: standard Linux package qemu-system-aarch64

### Build

To build Saturn for QEMU machine the following commads could be used:

```
 $ export CROSS_COMPILE=$(TOOLCHAIN_ROOT)/bin/aarch64-unknown-linux-gnu-
 $ make
```

NOTE: Please replace $(WORKDIR) by the correct path to toolchain.

### Run

To run Saturn in QEMU the following command could be used:

```
$ make run_qemu
```
### License

Licensed under the MIT License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

  http://opensource.org/licenses/MIT
