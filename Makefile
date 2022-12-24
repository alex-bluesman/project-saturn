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
	@$(QEMU_AARCH64) -m 1024 -cpu cortex-a57 -M virt -machine virtualization=on -nographic -kernel source/saturn
