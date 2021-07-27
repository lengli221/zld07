#include "includes.h"

void TIM4_Triger_ADC_Config(void){
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_InternalClockConfig(TIM4);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 99/*6000*/;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 59/*1000*/;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCStructInit(&TIM_OCInitStructure);

	//Output Compare Timing Mode configuration: Channel1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = 0x10;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	//TIM_PrescalerConfig(TIM4, 60000, TIM_PSCReloadMode_Immediate);
	TIM_PrescalerConfig(TIM4, 6000, TIM_PSCReloadMode_Immediate);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_ITConfig(TIM4, TIM_IT_CC4 , ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

