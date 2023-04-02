include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS += -MMD -MP

src := main.cpp			\
       heap.cpp			\
       console.cpp		\
       cpu.cpp			\
       uart_pl011.cpp		\
       exceptions.cpp		\
       vector.S			\
       ic/ic_core.cpp		\
       ic/gic/cpu_interface.cpp	\
       ic/gic/distributor.cpp	\
       ic/gic/redistributor.cpp	\
       mm/mm_core.cpp		\
       mm/mmu.cpp

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
	@$(CXX) $(AFLAGS) -c $< -o $@

%.o: %.c
	@echo "[CC]		$(CURRENT_DIR)/$@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d
	@echo "[CLEAN]		$(CURRENT_DIR)/ic/*.o"
	@-rm -f ic/gic/*.o ic/gic/*.d ic/*.o ic/*.d
	@echo "[CLEAN]		$(CURRENT_DIR)/mm/*.o"
	@-rm -f mm/*.o mm/*.d

-include .depend
