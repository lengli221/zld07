#include "includes.h"

// /*
// ** 定时器 1ms
// */
// static void TIM5_Config(void){
// 	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure0;

// 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

// 	TIM_DeInit(TIM5);
// 	TIM_TimeBaseStructure0.TIM_Period = 199/*999*/;
// 	TIM_TimeBaseStructure0.TIM_Prescaler =59;
// 	TIM_TimeBaseStructure0.TIM_ClockDivision = TIM_CKD_DIV2;
// 	TIM_TimeBaseStructure0.TIM_CounterMode = TIM_CounterMode_Up;
// 	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure0);
// 	TIM_ARRPreloadConfig((TIM_TypeDef *)TIM5, ENABLE);
// 	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
// 	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);

// 	/*TIM_Cmd(TIM5, ENABLE);*/ 
// }

// /*
// ** 中断处理
// */
// // uint32 test1;
// // uint32 test2;
// void TIM5_IRQHandler(void){
// 	volatile uint8 temp = 0;


// 	if(TIM_GetITStatus((TIM_TypeDef *)TIM5, TIM_IT_Update) != RESET){
// 		TIM_ClearITPendingBit((TIM_TypeDef *)TIM5, TIM_IT_Update);
// // 		if(++test1>=1000){
// // 			test1 = 0;
// // 			if(++test2>=60){
// // 				temp = 0;
// // 			}
// // 		}
// 		/*
// 		** update AD Value
// 		*/
// 		update_AD_Value();
// 	}
// }

// /*
// ** Tim5 初始化
// */
// void init_Time5(void){
// 	TIM5_Config();
// }

