export TOP_DIR := $(shell pwd)
include $(TOP_DIR)/config.mk

all:
	@echo "Project Saturn:	build"
	@echo "Arch: $(ARCH)"
	@echo "Machine: $(MACHINE)"
	@echo
	@make --no-print-directory -C source $@
	@echo
	@echo "Done"

clean:
	@echo "Project Saturn: clean"
	@echo "Arch: $(ARCH)"
	@echo "Machine: $(MACHINE)"
	@echo
	@make --no-print-directory -C source $@
	@echo
	@echo "Done"

run_qemu:
	@echo "Start Saturn kernel:"
	@echo "--------------------"
	@$(QEMU_AARCH64) -machine virt,gic_version=3 -machine virtualization=true -cpu cortex-a57 -smp 4 -machine type=virt -m 512M -nographic -kernel source/saturn
