MACHINE := qemuarm64

include $(TOP_DIR)/scripts/build.mk

DCONF := -DCONFIG_EARLY_PRINT		# Use early console output
