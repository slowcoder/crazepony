CROSS_COMPILE ?= arm-none-eabi

STM32F1_CC  := $(CROSS_COMPILE)-gcc
STM32F1_OBJCOPY := $(CROSS_COMPILE)-objcopy

STM32F1_CFLAGS := -O0 -g -Wall -mlittle-endian -mthumb -mcpu=cortex-m3 -mthumb-interwork
STM32F1_CFLAGS += -fdata-sections -ffunction-sections
STM32F1_CFLAGS += -fno-builtin
STM32F1_CFLAGS += -std=gnu99

STM32F1_LDFLAGS := -g
STM32F1_LDFLAGS += -Wl,--gc-sections
STM32F1_LDFLAGS += --specs=nano.specs -lc -lnosys

ALL_TARGETS := $(ALL_TARGETS) $(OUT)/$(LOCAL_TARGET)

$(LOCAL_TARGET)_CFLAGS := $(LOCAL_CFLAGS)
$(LOCAL_TARGET)_LDFLAGS := $(LOCAL_LDFLAGS)
$(LOCAL_TARGET)_LIBS := $(LOCAL_LIBS)
$(LOCAL_TARGET)_OUT := $(OUT)/$(TARGET_TYPE)/$(LOCAL_TARGET)
$(LOCAL_TARGET)_SRCS += $(patsubst %, $(LOCAL_PATH)/%, $(LOCAL_SRCS))
$(LOCAL_TARGET)_OBJS += \
        $(patsubst %.c,out/$(LOCAL_PROJECT)/stm32f1xx/obj/%.o, $(filter %.c, $($(LOCAL_TARGET)_SRCS)))
$(LOCAL_TARGET)_OBJS += \
        $(patsubst %.s,out/$(LOCAL_PROJECT)/stm32f1xx/obj/%.o, $(filter %.s, $($(LOCAL_TARGET)_SRCS)))
$(LOCAL_TARGET)_OBJS += \
        $(patsubst %.S,out/$(LOCAL_PROJECT)/stm32f1xx/obj/%.o, $(filter %.S, $($(LOCAL_TARGET)_SRCS)))
ALL_OBJS := $(ALL_OBJS) $($(LOCAL_TARGET)_OBJS)

$($(LOCAL_TARGET)_OBJS): L_CFLAGS := $($(LOCAL_TARGET)_CFLAGS)
$(OUT)/$(LOCAL_TARGET): L_LDFLAGS := $($(LOCAL_TARGET)_LDFLAGS)
$(OUT)/$(LOCAL_TARGET): L_LIBS := $($(LOCAL_TARGET)_LIBS)

out/$(LOCAL_PROJECT)/stm32f1xx/obj/%.o:%.c
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(STM32F1_CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(STM32F1_CFLAGS) $(L_CFLAGS) $<
	@$(STM32F1_CC) $(STM32F1_CFLAGS) $(L_CFLAGS) -c -o $@ $<

out/$(LOCAL_PROJECT)/stm32f1xx/obj/%.o:%.s
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(STM32F1_CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(STM32F1_CFLAGS) $(L_CFLAGS) $<
	@$(STM32F1_CC) $(STM32F1_CFLAGS) $(L_CFLAGS) -c -o $@ $<

$(OUT)/$(LOCAL_TARGET): $($(LOCAL_TARGET)_OBJS) $($(LOCAL_TARGET)_LIBS)
	@echo "LD $@"
	@$(STM32F1_CC) $(STM32F1_CFLAGS) $(L_CFLAGS) -o $@.elf $^ $(STM32F1_LDFLAGS) $(L_LDFLAGS) $(L_LIBS) -Xlinker -Map=$@.map
	@$(STM32F1_OBJCOPY) -O ihex $@.elf $@.hex

$(LOCAL_TARGET)_MAKEFILE := $(LOCAL_PATH)/$(makefile-name)

# Make all the targets objs depend on the makefile.
# This causes a rebuild for the project if the makefile changes
$($(LOCAL_TARGET)_OBJS): $($(LOCAL_TARGET)_MAKEFILE)
