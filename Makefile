include build/core.mk
ROOT_DIR = := $(my-dir)

# "Forward declaration", makefile-style. Needs be be before you include
# all the sub-project makefiles
all: phonyall

include app_tx/build.mk
include app_drone/build.mk

.PHONY: phonyall
phonyall: $(ALL_TARGETS)
	@echo "Finished building: $^"

ALL_DEPS := $(patsubst %.o,%.d,$(ALL_OBJS))

clean:
	@echo "Cleaning"
	@rm -f $(ALL_TARGETS) $(ALL_OBJS) $(ALL_DEPS)

#@rm -f $(remove-output-dirs)

ifneq ("$(MAKECMDGOALS)","clean")
-include $(ALL_DEPS)
else
remove-output-dirs := $(shell $(RM) -r $(sort $(dir $(ALL_OBJS))) $(OUT) app/gen)
endif

include Makefile.flash
