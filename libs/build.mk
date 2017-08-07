
STDPERIPH_CFLAGS := \
	-DUSE_STDPERIPH_DRIVER \
	-Ilibs/STM32F10x_StdPeriph_Driver/inc

FREERTOS_CFLAGS := \
	-Ilibs/CMSIS/Include \
	-Ilibs/CMSIS/Device/ST/STM32F1xx/Include \
	-Ilibs/FreeRTOS/Source/CMSIS_RTOS \
	-Ilibs/FreeRTOS/Source/include \
	-Ilibs/FreeRTOS/Source/portable/GCC/ARM_CM3 \

FREERTOS_SRCS := \
	libs/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c \
	libs/FreeRTOS/Source/list.c \
	libs/FreeRTOS/Source/timers.c \
	libs/FreeRTOS/Source/queue.c \
	libs/FreeRTOS/Source/croutine.c \
	libs/FreeRTOS/Source/event_groups.c \
	libs/FreeRTOS/Source/tasks.c \
	libs/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c \
	libs/FreeRTOS/Source/portable/MemMang/heap_4.c

STDPERIPH_SRCS := \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
	libs/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
	libs/STM32F10x_StdPeriph_Driver/src/misc.c \
