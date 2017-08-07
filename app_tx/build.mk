LOCAL_PATH := $(my-dir)
include $(CLEAN_VARS)

include $(LOCAL_PATH)/libs/build.mk

APP_LOCAL_SRCS := \
	src/startup_stm32f103xb.s \
	src/system_stm32f1xx.c \
	src/drivers/adc.c \
	src/drivers/buttons.c \
	src/drivers/leds.c \
	src/drivers/nrf24l01plus/hal_stm32.c \
	src/drivers/nrf24l01plus/nrf24.c \
	src/eventpump.c \
	src/log.c \
	src/main.c \
	src/semihosting.c \
	$(STDPERIPH_SRCS) \
	$(FREERTOS_SRCS)

APP_LOCAL_CFLAGS := \
	-I$(LOCAL_PATH) \
	-I$(LOCAL_PATH)/inc \
	$(STDPERIPH_CFLAGS) \
	$(FREERTOS_CFLAGS) \

APP_LOCAL_CFLAGS += \
	-DSTM32F103xB \
	-DSTM32F10X_MD \
	-DUSE_STDPERIPH_DRIVER \
	-DHSE_VALUE=8000000

include $(CLEAN_VARS)
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/../scripts/ld -TSTM32F103T8Ux_FLASH.ld
LOCAL_PROJECT := app_tx
LOCAL_SRCS := $(APP_LOCAL_SRCS)
LOCAL_CFLAGS := $(APP_LOCAL_CFLAGS)
LOCAL_TARGET := app_tx
include $(BUILD_STM32F1XX_BINARY)
