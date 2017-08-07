#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <FreeRTOS.h>
#include <task.h>

#include "drivers/motors.h"

// CCR1 = RL
// CCR2 = RR
// CCR3 = FR
// CCR4 = FL

static void tim_config(void) {
	GPIO_InitTypeDef        GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       TIM_OCInitStructure;

	// PA0 = TIM2_1
	// PA1 = TIM2_2
	// PA2 = TIM2_3
	// PA3 = TIM2_4

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period        = 32768;
	TIM_TimeBaseStructure.TIM_Period        = 1024;
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse       = 0;
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM2, ENABLE);

	TIM_Cmd(TIM2, ENABLE);

//	TIM2->CCR1 = 8000;
	TIM2->CCR1 = 100;
	TIM2->CCR1 = 0;
//	TIM2->CCR4 = 100;
}

err_code motors_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Turn off the motors
	GPIO_WriteBit(GPIOA,GPIO_Pin_0,0);
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,0);
	GPIO_WriteBit(GPIOA,GPIO_Pin_2,0);
	GPIO_WriteBit(GPIOA,GPIO_Pin_3,0);

	tim_config();

	return ERROR_OK;
}

// CCR1 = RL
// CCR2 = RR
// CCR3 = FR
// CCR4 = FL

void     motors_set_output(int fr,int rr,int rl,int fl) {
	if( fr >= 0 ) {
		if( fr > 1023 ) fr = 1023;
		TIM2->CCR3 = fr;
	}
	if( rr >= 0 ) {
		if( rr > 1023 ) rr = 1023;
		TIM2->CCR2 = rr;
	}
	if( rl >= 0 ) {
		if( rl > 1023 ) rl = 1023;
		TIM2->CCR1 = rl;
	}
	if( fl >= 0 ) {
		if( fl > 1023 ) fl = 1023;
		TIM2->CCR4 = fl;
	}
}
