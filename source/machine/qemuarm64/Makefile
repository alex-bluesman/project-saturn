include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS +=

SOURCES := source/early_uart.S source/uart_pl011.c

OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.S=.o)

CURRENT_DIR = "machine/qemuarm64"

build: built_in.o

built_in.o: $(OBJECTS)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.S
	@echo "[AS]		$(CURRENT_DIR)/$@"
	@$(CC) $(AFLAGS) -c $< -o $@

%.o: %.c
	@echo "[CC]		$(CURRENT_DIR)/$@"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o source/*.o .depend

.depend: depend

depend: $(SOURCES)
	@rm -f ./.depend
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend;

-include .depend
