include $(TOP_DIR)/config.mk

src := platform.cpp		\
       early_uart_pl011.S	\
       uart_pl011.cpp

objs := $(src:.cpp=.o)
objs := $(objs:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "bsp"

.PHONY: build
build: built_in.o

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
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d

-include .depend
