MACHINE := qemu-aarch64

SATURN_CONFIG := -DSTACK_SIZE=1024

INCLUDES := -I$(TOP_DIR)/source/include			\
	    -I$(TOP_DIR)/source/bsp/$(MACHINE)/include

ASMFLAGS := $(SATURN_CONFIG) $(INCLUDES)
CXXFLAGS := $(SATURN_CONFIG) $(INCLUDES) -MMD -MP -fno-rtti -fno-exceptions
LDFLAGS  :=


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
