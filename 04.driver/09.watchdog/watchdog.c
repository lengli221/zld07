#include "includes.h"

/*
** 看门狗初始化
*/
void watchdogInit(void){
	/*Enable the LSI OSC*/
	RCC_LSICmd(ENABLE);

	/*Wait till LSI is ready*/
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}

	/*IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency dispersion)*/
	/*Enable write access to IWDG_PR and IWDG_RLR registers*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/*IWDG counter clock: LSI/32*/
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	 Counter Reload Value = 250ms/IWDG counter clock period
	                      = 250ms / (LSI/32)
	                      = 0.25s / (LsiFreq/32)
	                      = LsiFreq/(32 * 4)
	                      = LsiFreq/128
	*/
	IWDG_SetReload(40000/32);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/*Enable IWDG (the LSI oscillator will be enabled by hardware)*/
	IWDG_Enable();
}

/*
** 更新看门狗寄存器--喂狗
*/
void watchdogUpdate(void){
	IWDG_ReloadCounter();
}

