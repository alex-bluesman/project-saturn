export TOP_DIR := $(shell pwd)
include $(TOP_DIR)/config.mk

.PHONY: all
all: saturn asteroid

.PHONY: saturn
saturn:
	@echo
	@echo "Project Saturn: build"
	@echo "Machine: $(MACHINE)"
	@echo
	@make --no-print-directory -C source
	@echo
	@echo "Done"

.PHONY: asteroid
asteroid:
	@echo
	@echo "Asteroid: build"
	@echo "Machine: $(MACHINE)"
	@echo
	@make --no-print-directory -C tools/asteroid
	@echo
	@echo "Done"

clean:
	@echo
	@echo "Project Saturn: clean"
	@echo
	@make --no-print-directory -C source $@
	@make --no-print-directory -C tools/asteroid $@
	@echo
	@echo "Done"

.PHONY: run_qemu
run_qemu:
	@echo "Start Saturn kernel:"
	@echo "--------------------"
	@$(QEMU_AARCH64)							\
		-machine virt,gic_version=3					\
		-machine virtualization=true					\
		-cpu cortex-a57 						\
		-smp 4								\
		-machine type=virt						\
		-m 1024M							\
		-nographic							\
		-device loader,file=tools/asteroid/asteroid,addr=0x41000000	\
		-device loader,file=source/saturn,addr=0x7fc00000		\
		-device loader,addr=0x7fc00000,cpu-num=0
