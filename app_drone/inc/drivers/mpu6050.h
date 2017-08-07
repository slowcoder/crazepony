#pragma once

#include "error.h"

#define EMPU6050_IDENT   (EMPU6050_BASE + 0)
#define EMPU6050_NO_INIT (EMPU6050_BASE + 1)

err_code mpu6050_init(void);
err_code mpu6050_read_accel(int16_t *pX,int16_t *pY,int16_t *pZ);
