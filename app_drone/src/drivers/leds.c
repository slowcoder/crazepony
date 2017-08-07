#include <stm32f10x.h>

#include "drivers/leds.h"

int leds_init(void) {
	GPIO_InitTypeDef	igpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	igpio.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_11;
	igpio.GPIO_Speed = GPIO_Speed_10MHz;
	igpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&igpio);

	igpio.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_3;
	igpio.GPIO_Speed = GPIO_Speed_10MHz;
	igpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&igpio);

	GPIO_WriteBit(GPIOA,GPIO_Pin_8 ,0);
	GPIO_WriteBit(GPIOA,GPIO_Pin_11,0);
	GPIO_WriteBit(GPIOB,GPIO_Pin_1 ,0);
	GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,0);

	return 0;
}
