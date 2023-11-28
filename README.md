# General

Project Saturn is a lightweight hypervisor for AArch64 platforms which provides
static hardware resources split. The static configuration makes possible to
significantly reduce the complexity of hypervisor architecture, what simplifies
configuration and qualification. In general 10K lines of source code should be
enough to run two Linux images in parallel.

Project scope (not fully implemented yet):

 - The Saturn core
 - Reference BSP:
   * QEMU AArch64 (source/bsp)
 - Configuration tools (not available for today)
 - Guest images and respective source projects:
   * Yocto Linux 4.2.3 for AArch64
   * Asteroid micro kernel (tools/asteroid)

Additionally could be provided on request:
 - Requirements specification
 - Architecture design
 - Test suit

Copyright (c) 2023
Alexander Smirnov <alex.bluesman.smirnov@gmail.com>

# QEMU Aarch64 Target

### Prerequisites

1. Toolchain: `aarch64-unknown-linux-gnu-` from linaro.org website
2. QEMU: standard Linux package qemu-system-aarch64
3. In case using Linux as guest VM, then content of repository `https://github.com/alex-bluesman/os-images.git` should be fetched to `ìmages`

### Build

To build Saturn for QEMU machine the following commads could be used:

```
 $ export CROSS_COMPILE=$(TOOLCHAIN_ROOT)/bin/aarch64-unknown-linux-gnu-
 $ make
```

NOTE: Please replace $(WORKDIR) by the correct path to toolchain.

### Run

To run Saturn in QEMU the following command could be used:

For Asteroid target:
```
$ python3 scripts/run_qemu.py
```

For Linux target:
```
$ python3 scripts/run_qemu.py -g linux
```

### License

Licensed under the MIT License (the "License"); you may not use this file except
in compliance with the License. You may obtain a copy of the License at

  http://opensource.org/licenses/MIT
