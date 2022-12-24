include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS += -MMD -MP

src := head.S early_uart_pl011.S

objs := $(src:.c=.o)
objs := $(objs:.S=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "boot"

.PHONY: build
build: built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@echo $(objs)
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CXX) $(AFLAGS) -c $< -o $@

%.o: %.c
	@echo "[CC]		$(CURRENT_DIR)/$@"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o

-include .depend
