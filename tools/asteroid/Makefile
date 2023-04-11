include $(TOP_DIR)/config.mk

.PHONY: all
all: build
	@echo "[LD]		asteroid.bin"
	@$(LD)  -EL -Map asteroid.map		\
		-T asteroid.lds			\
		source/built_in.o		\
		-o asteroid.bin
	@echo "[OBJCOPY]	asteroid"
	@$(OBJCOPY) -O binary -S asteroid.bin asteroid

.PHONY: build
build:
	@make --no-print-directory -C source $@

.PHONY: clean
clean:
	@make --no-print-directory -C source $@
	@echo "[CLEAN]		asteroid.bin asteroid asteroid.map"
	@rm -f asteroid.bin asteroid asteroid.map
