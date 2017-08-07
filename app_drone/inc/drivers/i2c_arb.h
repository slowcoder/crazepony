#pragma once

#include <stdint.h>
#include "error.h"

#define EEI2CARB_FOO    (EI2CARB_BASE+0)

err_code i2c_arb_init(void);
err_code i2c_arb_writereg(uint8_t addr,uint8_t reg,uint8_t *data,int len);
err_code i2c_arb_readreg(uint8_t addr,uint8_t reg,uint8_t *data,int len);
