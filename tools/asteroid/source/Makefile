include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS += -MMD -MP

src := head.S			\
       main.cpp			\
       console.cpp		\
       heap.cpp			\
       uart_pl011.cpp

objs := $(src:.cpp=.o)
objs := $(objs:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "tools/asteroid"

 .PHONY: build
build: built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CXX) $(AFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d

-include .depend