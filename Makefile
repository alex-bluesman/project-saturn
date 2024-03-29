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
