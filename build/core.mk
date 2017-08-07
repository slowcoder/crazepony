DIR = $(error DO NOT USE 'DIR')
my-dir = $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

OUT := out

BUILD_STM32F1XX_BINARY := $(my-dir)/build-stm32f1xx-target.mk

CLEAN_VARS := $(my-dir)/cleanvars.mk
