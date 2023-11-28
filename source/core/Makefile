include $(TOP_DIR)/config.mk

src := main.cpp				\
       heap.cpp				\
       console.cpp			\
       cpu.cpp				\
       exceptions.cpp			\
       vector.S				\
       ic/ic_core.cpp			\
       ic/gic/cpu_interface.cpp		\
       ic/gic/distributor.cpp		\
       ic/gic/redistributor.cpp		\
       ic/virt/virt_distributor.cpp	\
       ic/virt/virt_redistributor.cpp	\
       ic/virt/virt_ic.cpp		\
       mm/mm_core.cpp			\
       mm/mmu.cpp			\
       mm/trap.cpp			\
       vmm/vm_config.cpp		\
       vmm/vm_manager.cpp

objs := $(src:.cpp=.o)
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
	@$(CXX) $(ASMFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d
	@echo "[CLEAN]		$(CURRENT_DIR)/ic/*.o"
	@-rm -f ic/gic/*.o ic/gic/*.d ic/virt/*.o ic/virt/*.d ic/*.o ic/*.d
	@echo "[CLEAN]		$(CURRENT_DIR)/mm/*.o"
	@-rm -f mm/*.o mm/*.d
	@echo "[CLEAN]		$(CURRENT_DIR)/vmm/*.o"
	@-rm -f vmm/*.o vmm/*.d

-include .depend
