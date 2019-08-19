# General

Project Saturn is a hypervisor for Aarch64 platforms.

Copyright (c) 2019
Alexander Smirnov <alex.bluesman.smirnov@gmail.com>

# QEMU Aarch64 Target

### Prerequisites

1. Toolchain: `gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu` from linaro.org website
2. QEMU: standard Linux package qemu-system-aarch64

### Build

To build Saturn for QEMU machine the following commads could be used:

```
 $ export CROSS_COMPILE=$(WORKDIR)/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
 $ make
```

NOTE: Please replace $(WORKDIR) by the correct path to toolchain.

### Run

To run Saturn in QEMU the following command could be used:

```
$ make run_qemu
```
