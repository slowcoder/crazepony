#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include "stm32f10x_i2c.h"
#include "log.h"

#include "drivers/i2c_arb.h"

#define TIMEOUT_TICKS 40

static void enable(void);
static void disable(void);

static int wait_flag(uint32_t flag) {
	portTickType s,n;

	s = xTaskGetTickCount();

	while( !I2C_GetFlagStatus(I2C1,flag) ) {
		n = xTaskGetTickCount();

		if( n > (s+TIMEOUT_TICKS) ) return -1;
		else vTaskDelay(0);
	}
	return 0;
}

static int wait_event(uint32_t event) {
	portTickType s,n;

	s = xTaskGetTickCount();

	while( !I2C_CheckEvent(I2C1,event) ) {
		n = xTaskGetTickCount();

		if( n > (s+TIMEOUT_TICKS) ) return -1;
		else vTaskDelay(0);
	}
	return 0;
}

static int read_reg(uint8_t address,uint8_t reg,uint8_t *buff,int len) {
	int i;

	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	/* Test on BUSY Flag */
	//if( wait_flag(I2C_FLAG_BUSY) != 0 ) return -1;

	/* Enable the I2C peripheral */
	I2C_GenerateSTART(I2C1, ENABLE);
	if( wait_flag(I2C_FLAG_SB) != 0 ) return -2;
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
	if( wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != 0 ) return -3;
	I2C_SendData(I2C1,reg);
	if( wait_flag(I2C_FLAG_TXE) != 0 ) return -4;

	// Restart
	I2C_GenerateSTART(I2C1, ENABLE);
	if( wait_flag(I2C_FLAG_SB) != 0 ) return -5;

	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Receiver);
	if( wait_event(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != 0 ) return -6;


	for(i=0;i<len;i++) {
		if( wait_event(I2C_EVENT_MASTER_BYTE_RECEIVED) != 0 ) return -7;
		buff[i] = I2C_ReceiveData(I2C1);
	}

    /*enable NACK bit */
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
//	if( wait_flag(I2C_FLAG_STOPF) != 0 ) return -11;

    return 0;
}

static int write_reg(uint8_t address,uint8_t reg,uint8_t *buff,int len) {
	int i;

	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	/* Test on BUSY Flag */
	//if( wait_flag(I2C_FLAG_BUSY) != 0 ) return -1;

	/* Enable the I2C peripheral */
	I2C_GenerateSTART(I2C1, ENABLE);
	if( wait_flag(I2C_FLAG_SB) != 0 ) return -2;
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
	if( wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != 0 ) return -3;
	I2C_SendData(I2C1,reg);
	if( wait_flag(I2C_FLAG_TXE) != 0 ) return -4;

	for(i=0;i<len;i++) {
		I2C_SendData(I2C1,buff[i]);
		if( wait_flag(I2C_FLAG_TXE) != 0 ) return -5;
	}

    /*enable NACK bit */
    I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    
    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
//	if( wait_flag(I2C_FLAG_STOPF) != 0 ) return -11;

    return 0;
}


static void enable(void) {
    I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* I2C1 configuration */
	I2C_InitStructure.I2C_Mode        = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle   = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack         = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed  = 100000;
	I2C_Init(I2C1, &I2C_InitStructure);

	/*enable I2C*/
	I2C_Cmd(I2C1,ENABLE);
}

static void disable(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	I2C_Cmd(I2C1,DISABLE);
	I2C_DeInit(I2C1);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

err_code i2c_arb_readreg(uint8_t addr,uint8_t reg,uint8_t *data,int len) {
	int r;

	enable();
	r = read_reg(addr,reg,data,len);
	disable();

	return r;
}

err_code i2c_arb_writereg(uint8_t addr,uint8_t reg,uint8_t *data,int len) {
	int r;

	enable();
	r = write_reg(addr,reg,data,len);
	disable();

	return r;
}

err_code i2c_arb_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef  I2C_InitStructure;

    /* I2C1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    vTaskDelay(1);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* I2C1 SDA and SCL configuration */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C1);
#if 0
	I2C1->CR1 = 0x1;
	vTaskDelay(1);
	I2C1->CR1 = 0x0;
	vTaskDelay(1);
#endif
    /* I2C1 configuration */
    I2C_InitStructure.I2C_Mode        = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle   = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack         = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed  = 100000;
    I2C_Init(I2C1, &I2C_InitStructure);

    /*enable I2C*/
    I2C_Cmd(I2C1,ENABLE);

	return 0;
}
