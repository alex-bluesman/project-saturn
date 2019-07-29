AS      = $(CROSS_COMPILE)as
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
STRIP   = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

MKIMAGE = mkimage

QEMU_AARCH64 = qemu-system-aarch64

include $(TOP_DIR)/source/machine/$(MACHINE)/config.mk
