ARCH := aarch64

INCS := -I$(TOP_DIR)/source/arch/$(ARCH)/include -I$(TOP_DIR)/source/machine/$(MACHINE)/include -I$(TOP_DIR)/source/kernel/include

AFLAGS  := -mcpu=cortex-a53 $(INCS) $(DCONF)
CFLAGS  := -O2 -Wall -mcpu=cortex-a53 -std=gnu89 $(INCS) $(DCONF)
LDFLAGS :=
