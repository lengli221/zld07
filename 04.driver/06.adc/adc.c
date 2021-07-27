#include "includes.h"

uint16 ADCConvertedValue[cSampleTimes][cChannelNum];
uint16 ADCConvdValue3[cSampleTimes_3][cChannelNum_3];

/*
** ADC1 引脚初始化
*/
static void ADC_GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	/*
	** PC0 PC1 PC2 :AB三相电压采样 PC4:环境温度A通道检测 PC5:环境温度B通道检测
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init((GPIO_TypeDef *)GPIOC,(GPIO_InitTypeDef *)&GPIO_InitStructure);
}

/*
** ADC3 引脚初始化
*/
static void ADC3_GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOF, ENABLE);
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	/*
	** PF6 PF7 PF8:三相电流采样
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
}

/*
** ADC1 DMA初始化配置
*/
static void ADC1_DMA_Config(void){
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);											
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	DMA_DeInit(DMA2_Stream0);																				
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue[0][0];/*(uint32_t)&ADCConvertedValue;*/
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize =  cSampleTimes * cChannelNum;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);	
}

/*
** ADC3 DMA初始化配置
*/
static void ADC3_DMA_Config(void){
	DMA_InitTypeDef DMA_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	DMA_InitStructure.DMA_Channel = DMA_Channel_2;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC3_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvdValue3[0][0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = cSampleTimes_3 * cChannelNum_3;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream1, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream1, ENABLE);	
}

/*
** ADC 通用寄存器配置
*/
static void ADC_CommConfig(void){
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	TIM4_Triger_ADC_Config();
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;								
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;									
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;			
	ADC_CommonInit(&ADC_CommonInitStructure);	
}

/*
** ADC1 初始化
*/
static void ADC1_Init(void){
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;												
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;														
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;												
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;					
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;							
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												
	ADC_InitStructure.ADC_NbrOfConversion = cChannelNum;												
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 5,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_18, 6,  ADC_SampleTime_144Cycles);

	ADC_VBATCmd(ENABLE);
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);													
	ADC_DMACmd(ADC1, ENABLE);																					
	ADC_Cmd(ADC1, ENABLE);	
}

/*
** ADC3 初始化
*/
static void ADC3_Init(void){
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;												
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;														
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;												
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;					
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;							
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;												
	ADC_InitStructure.ADC_NbrOfConversion = cChannelNum_3;												
	ADC_Init(ADC3, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 1,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 2,  ADC_SampleTime_144Cycles);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_6, 3,  ADC_SampleTime_144Cycles);

	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);													
	ADC_DMACmd(ADC3, ENABLE);																					
	ADC_Cmd(ADC3, ENABLE);	
}

/*
** ADC 初始化
*/
void AD_UseInit(void){
	/*
	** ADC1 引脚初始化
	*/
	ADC_GPIO_Config();
	/*
	** ADC3 引脚初始化
	*/
	ADC3_GPIO_Config();
	/*
	** ADC1 DMA初始化配置
	*/	
	ADC1_DMA_Config();
	/*
	** ADC3 DMA初始化配置
	*/
	ADC3_DMA_Config();
	/*
	** ADC 通用寄存器配置
	*/	
	ADC_CommConfig();
	/*
	** ADC1 初始化
	*/	
	ADC1_Init();
	/*
	** ADC3 初始化
	*/
	ADC3_Init();	
}

/*
** ADC1 获取ADC采样AD Value
*/
uint16 ADC_GetADResultAD(uint8 channel){
	uint8 i=0;
	uint16 vol=0;
	unsigned long long tmp=0;


	for(i=0;i<cSampleTimes;i++)
	{
		tmp += ADCConvertedValue[i][channel];
	}
	tmp /= cSampleTimes;
	vol = tmp;
	
	return(vol);
}

/*
** ADC3 获取ADC采样AD Value
*/
uint16 ADC3_GetADResultAD(uint8 ch){
	uint8 i=0;
	uint16 vol=0;
	unsigned long long tmp=0;


	for(i=0;i<cSampleTimes_3;i++)
	{
		tmp += ADCConvdValue3[i][ch];
	}
	tmp /= cSampleTimes;
	vol = tmp;
	
	return(vol);	
}



