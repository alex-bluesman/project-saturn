include $(TOP_DIR)/config.mk

.PHONY: build
build:
	@make -s --no-print-directory -C arch/$(ARCH) $@
	@make -s --no-print-directory -C kernel $@
	@make -s --no-print-directory -C machine/$(MACHINE) $@

.PHONY: clean
clean:
	@make -s --no-print-directory -C arch/$(ARCH) $@
	@make -s --no-print-directory -C kernel $@
	@make -s --no-print-directory -C machine/$(MACHINE) $@
	@echo "[CLEAN]		saturn.bin saturn saturn.map"
	@rm -f saturn.bin saturn saturn.map

.PHONY: all
all: build
	@echo "[LD]		saturn.bin"
	@$(LD) -EL -Map saturn.map			\
		-T machine/$(MACHINE)/saturn.lds	\
		arch/$(ARCH)/built_in.o			\
		kernel/built_in.o			\
		machine/$(MACHINE)/built_in.o		\
		-o saturn.bin
	@echo "[OBJCOPY]	saturn"
	@$(OBJCOPY) -O binary -S saturn.bin saturn
