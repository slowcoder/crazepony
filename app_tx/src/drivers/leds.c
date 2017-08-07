#include <stm32f10x.h>

#include "drivers/leds.h"

int leds_init(void) {
	GPIO_InitTypeDef	igpio;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	igpio.GPIO_Pin   = GPIO_Pin_12;
	igpio.GPIO_Speed = GPIO_Speed_10MHz;
	igpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&igpio);

	igpio.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	igpio.GPIO_Speed = GPIO_Speed_10MHz;
	igpio.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&igpio);

	GPIO_WriteBit(GPIOA,GPIO_Pin_12,1);

	GPIO_WriteBit(GPIOB,GPIO_Pin_4,0);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,1);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,0);
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,1);

	return 0;
}
