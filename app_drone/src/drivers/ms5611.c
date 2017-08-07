#include <string.h>
#include "drivers/i2c_arb.h"
#include "log.h"

#include "drivers/ms5611.h"

#define I2C_ADDR (0b11101100)

#define CMD_RESET 0x1E
#define CMD_PROM_READ 0xA6
#define CMD_CONVERSION 0x48

int ms5611_init(void) {

	i2c_arb_writereg(I2C_ADDR>>1,0x1E,NULL,0);

	LOG("MS5611 Initialized");

	return 0;
}

#if 0
#define MS5611_ADDR         0xEE
#define MS561101BA_RESET 0x1E

	IIC_Start();
    IIC_Send_Byte(MS5611_ADDR);
	IIC_Wait_Ack();
    IIC_Send_Byte(MS561101BA_RESET);
	IIC_Wait_Ack();	
    IIC_Stop();
#endif
