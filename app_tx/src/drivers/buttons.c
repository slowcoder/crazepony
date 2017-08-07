#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>
#include "eventpump.h"

#include "drivers/buttons.h"

// Pins are 0 when pressed
// PB1 = MODE
// PB3 = +
// PA8 = -

void EXTI1_IRQHandler(void) {
	if( EXTI_GetITStatus(EXTI_Line1) != RESET ) {
		uint32_t s = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);

		s |= (0<<16);

		eventpump_post_irq(eEventPumpSource_Buttons,s);

		EXTI_ClearFlag(EXTI_Line1);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}

}

void EXTI3_IRQHandler(void) {
	if( EXTI_GetITStatus(EXTI_Line3) != RESET ) {
		uint32_t s = !GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3);

		s |= (1<<16);

		eventpump_post_irq(eEventPumpSource_Buttons,s);

		EXTI_ClearFlag(EXTI_Line3);
		EXTI_ClearITPendingBit(EXTI_Line3);
	}

}

void EXTI9_5_IRQHandler(void) {
	if( EXTI_GetITStatus(EXTI_Line8) != RESET ) {
		uint32_t s = !GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8);

		s |= (2<<16);

		eventpump_post_irq(eEventPumpSource_Buttons,s);

		EXTI_ClearFlag(EXTI_Line8);
		EXTI_ClearITPendingBit(EXTI_Line8);
	}

}

err_code buttons_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Connect PB1 to EXTI8
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line    = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
	EXTI_InitStructure.EXTI_Line    = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	EXTI_InitStructure.EXTI_Line    = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable IRQ(s)
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return ERROR_OK;
}
