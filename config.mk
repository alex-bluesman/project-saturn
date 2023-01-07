MACHINE := qemuarm64

SATURN_CONFIG = -DSTACK_SIZE=1024

AFLAGS += $(SATURN_CONFIG) -I$(TOP_DIR)/source/include
CFLAGS += $(SATURN_CONFIG) -I$(TOP_DIR)/source/include -fno-rtti -fno-exceptions


# Toolchain parameters
AS      = $(CROSS_COMPILE)as
CC      = $(CROSS_COMPILE)gcc
CXX     = $(CROSS_COMPILE)g++
LD      = $(CROSS_COMPILE)ld
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

MKIMAGE = mkimage

QEMU_AARCH64 = qemu-system-aarch64
