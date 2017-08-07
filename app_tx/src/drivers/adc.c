#include <string.h>

#include <stm32f10x_adc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

#include "drivers/adc.h"
#include "log.h"

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
static volatile uint16_t ADCDMABuffer[4];
static uint16_t cachedValues[4];
static volatile int iNumIRQs = 0;

static void gpio_conf(void) {
	GPIO_InitTypeDef ginit;

	GPIO_StructInit(&ginit);
	ginit.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	ginit.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_Init(GPIOA, &ginit);
}

static void dma_conf(void) {
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCDMABuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

void DMA1_Channel1_IRQHandler(void) {


	if( DMA_GetITStatus(DMA1_FLAG_TC1) ) {
		iNumIRQs++;
		// TODO: LOCK
		cachedValues[0] = ADCDMABuffer[0];
		cachedValues[1] = ADCDMABuffer[1];
		cachedValues[2] = ADCDMABuffer[2];
		cachedValues[3] = ADCDMABuffer[3];
	}

	DMA_ClearITPendingBit(DMA1_IT_TC1);
    DMA_ClearITPendingBit(DMA1_IT_GL1);
}

int adc_init(void) {
	ADC_InitTypeDef ainit;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	gpio_conf();
	dma_conf();

	ADC_DeInit(ADC1);
	ainit.ADC_Mode               = ADC_Mode_Independent;
	ainit.ADC_ScanConvMode       = ENABLE;
	ainit.ADC_ContinuousConvMode = ENABLE;
	ainit.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
	ainit.ADC_DataAlign          = ADC_DataAlign_Right;
	ainit.ADC_NbrOfChannel       = 4;
	ADC_Init(ADC1, &ainit);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);

	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);

	// Calibrate
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

    //Enable DMA1 channel IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	return 0;
}

int adc_getValues(uint16_t *pSample) {
	if( pSample == NULL ) return -1;

	pSample[0] = cachedValues[0];
	pSample[1] = cachedValues[1];
	pSample[2] = cachedValues[2];
	pSample[3] = cachedValues[3];

//	LOG("IRQs=%i",iNumIRQs);

	return 0;
}
