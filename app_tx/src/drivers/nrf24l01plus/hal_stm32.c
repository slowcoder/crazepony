#include <stdint.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>


#define CE_BANK  GPIOA
#define CE_PIN   GPIO_Pin_15
#define CSN_BANK GPIOA
#define CSN_PIN  GPIO_Pin_4
#define SCK_BANK GPIOA
#define SCK_PIN  GPIO_Pin_5
#define MISO_BANK GPIOA
#define MISO_PIN  GPIO_Pin_6
#define MOSI_BANK GPIOA
#define MOSI_PIN  GPIO_Pin_7

void nrf24_setupPins() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	// CE Pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = CE_PIN;
	GPIO_Init(CE_BANK, &GPIO_InitStructure);

	// CSN Pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = CSN_PIN;
	GPIO_Init(CSN_BANK, &GPIO_InitStructure);

	// SPI1
	SPI_InitTypeDef  SPI_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin   = SCK_PIN | MOSI_PIN | MISO_PIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);

	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = 8;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
}

void nrf24_ce_digitalWrite(uint8_t state) {
	GPIO_WriteBit(CE_BANK,CE_PIN,!!state);
}

void nrf24_csn_digitalWrite(uint8_t state) {
	GPIO_WriteBit(CSN_BANK,CSN_PIN,!!state);
}

uint8_t spi_transfer(uint8_t tx) {
	uint8_t rx;

	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1,tx);
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	rx = SPI_I2S_ReceiveData(SPI1);

	return rx;
}
