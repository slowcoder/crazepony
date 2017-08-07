#pragma once

#include <stdint.h>

int adc_init(void);
int adc_getValues(uint16_t *pSample);
