include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS += -MMD -MP -fno-rtti

src := main.cpp console.c mmu.c uart_pl011.c core.cpp heap.cpp

objs := $(src:.c=.o)
objs := $(objs:.cpp=.o)
objs := $(objs:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "core"

.PHONY: build
build: built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CC) $(AFLAGS) -c $< -o $@

%.o: %.c
	@echo "[CC]		$(CURRENT_DIR)/$@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o

-include .depend
