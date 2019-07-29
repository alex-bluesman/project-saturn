include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS +=

SOURCES := init.c console.c

OBJECTS := $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.S=.o)

CURRENT_DIR = "kernel"

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
	@-rm -f *.o .depend

.depend: depend

depend: $(SOURCES)
	@rm -f ./.depend
	@$(CC) $(CFLAGS) -MM $^ >> ./.depend;

-include .depend
