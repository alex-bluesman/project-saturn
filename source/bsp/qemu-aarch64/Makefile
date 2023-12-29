include $(TOP_DIR)/config.mk

src := platform.cpp			\
       devices/early_uart_pl011.S	\
       devices/uart_pl011.cpp		\
       devices/virt_uart_pl011.cpp	\
       tools/storage.cpp

objs := $(src:.cpp=.o)
objs := $(objs:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "bsp"
DEFCONFIG ?= "linux"

.PHONY: generate
generate:
	@echo "[GEN]		config for $(DEFCONFIG)"
	@mkdir -p generated
	@$(PYTHON) $(TOP_DIR)/scripts/build_config.py -i configs/$(DEFCONFIG).json -o generated/saturn_config.hpp

.PHONY: build
build: generate built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CXX) $(ASMFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/devices/*.o"
	@-rm -f devices/*.o devices/*.d
	@echo "[CLEAN]		$(CURRENT_DIR)/tools/*.o"
	@-rm -f tools/*.o tools/*.d
	@-rm -f tools/*.o tools/*.d
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d
	@-rm -rf generated

-include .depend
