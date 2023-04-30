include $(TOP_DIR)/config.mk

src := head.S

objs := $(src:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "boot"

.PHONY: build
build: built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CXX) $(ASMFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o

-include .depend
