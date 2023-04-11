include $(TOP_DIR)/config.mk

AFLAGS +=
CFLAGS += -MMD -MP

src := apps_main.cpp		\
       cmdline/cmdline.cpp	\
       ta/test_adapter.cpp

objs := $(src:.cpp=.o)
deps := $(objs:.o=.d)

CURRENT_DIR = "apps"

.PHONY: build
build: built_in.o

built_in.o: $(objs)
	@echo "[LD]		$(CURRENT_DIR)/$@"
	@$(LD) $(LDFLAGS) -r -o $@ $^

%.o: %.cpp
	@echo "[CXX]		$(CURRENT_DIR)/$@"
	@$(CXX) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "[CLEAN]		$(CURRENT_DIR)/*.o"
	@-rm -f *.o *.d
	@-rm -f ta/*.o ta/*.d
	@-rm -f cmdline/*.o cmdline/*.d

-include .depend
