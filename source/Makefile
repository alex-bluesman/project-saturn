include $(TOP_DIR)/config.mk

.PHONY: all
all: build
	@echo "[LD]		saturn.bin"
	@$(LD)  -EL -Map saturn.map		\
		-T boot/saturn.lds		\
		boot/built_in.o			\
		core/built_in.o			\
		apps/built_in.o			\
		-o saturn.bin
	@echo "[OBJCOPY]	saturn"
	@$(OBJCOPY) -O binary -S saturn.bin saturn

.PHONY: build
build:
	@make -s --no-print-directory -C boot $@
	@make -s --no-print-directory -C core $@
	@make -s --no-print-directory -C apps $@

.PHONY: clean
clean:
	@make -s --no-print-directory -C boot $@
	@make -s --no-print-directory -C core $@
	@make -s --no-print-directory -C apps $@
	@echo "[CLEAN]		saturn.bin saturn saturn.map"
	@rm -f saturn.bin saturn saturn.map
