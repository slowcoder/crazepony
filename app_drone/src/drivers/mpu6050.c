#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "drivers/i2c_arb.h"
#include "log.h"

#include "drivers/mpu6050.h"

#define I2C_ADDR (0x68<<1)

#define REG_ACCEL_XOUT_H 0x3B
#define REG_ACCEL_XOUT_L 0x3C
#define REG_ACCEL_YOUT_H 0x3D
#define REG_ACCEL_YOUT_L 0x3E
#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40

#define REG_SIGNAL_PATH_RESET 0x68
#define REG_USER_CTRL         0x6A
#define REG_PWR_MGMT_1        0x6B
#define REG_PWR_MGMT_2        0x6C
#define REG_WHO_AM_I          0x75

static struct {
	int bInitialized;
} state;

static int regWrite(uint8_t reg,uint8_t val) {
	i2c_arb_writereg(I2C_ADDR,reg,&val,1);

	return 0;
}

static void reset(void) {
	regWrite(REG_PWR_MGMT_1, 1<<7);    // Device reset, no sleep
	regWrite(REG_USER_CTRL,1);         // Reset via user control
	regWrite(REG_SIGNAL_PATH_RESET,7); // Reset gyro,accel and temp
}

static int ident(void) {
	uint8_t id;
	int r;

	r = i2c_arb_readreg(I2C_ADDR,REG_WHO_AM_I,&id,1);
	if( r != 0 ) return r;

	if( id != 0x68 ) {
		LOG("ID=0x%02x <- Wrong",id);
		return -1001;
	}

	return 0;
}

#define NUM_RETRIES 5

err_code mpu6050_init(void) {
	int r;
	int retries,found = 0;

	state.bInitialized = 0;

	reset();

	for(retries=0;retries<NUM_RETRIES;retries++) {

		r = ident();
		if( r != 0 ) {
			LOG("MPU6050 ident failure: %d",r);
			reset();
			vTaskDelay(250);
		} else {
			found = 1;
			break;
		}
	}

	if( !found ) {
		LOG("MPU6050 failure..");
		return EMPU6050_IDENT;
	}

	state.bInitialized = 1;
	LOG("MPU6050 Initialized");

	return ERROR_OK;
}

err_code mpu6050_read_accel(int16_t *pX,int16_t *pY,int16_t *pZ) {
	int r;
	uint8_t raw[6];

	if( !state.bInitialized ) return EMPU6050_NO_INIT;

	r = i2c_arb_readreg(I2C_ADDR,REG_ACCEL_XOUT_H,raw,6);
	if( r != ERROR_OK ) return r;

	if( pX != NULL ) *pX = ((int8_t)raw[0]<<8) | raw[1];
	if( pY != NULL ) *pY = ((int8_t)raw[2]<<8) | raw[3];
	if( pZ != NULL ) *pZ = ((int8_t)raw[4]<<8) | raw[5];

	return ERROR_OK;
}
