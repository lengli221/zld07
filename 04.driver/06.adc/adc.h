#ifndef __ADC_H_
#define __ADC_H_

#define cSampleTimes													1
#define cChannelNum   												6

#define cSampleTimes_3												1
#define cChannelNum_3   												3

#define ADC1_DR_ADDRESS    										((uint32_t)0x4001204C)
#define ADC3_DR_ADDRESS    										((uint32_t)0x4001224C)


/*
** ADC1 采样通道说明
** 0.1.2-->相电压(ABC) 
** 3.环境温度
*/
#define APhaseVol_AD_1												0
#define Temper1_ADResult   											3
#define Temper2_ADResult  											4

/*
** ADC3 采样通道说明
** 0.1.2-->相电流(ABC)
*/
#define APhaseCur_ADC_3											0

/*
** ADC 初始化
*/
void AD_UseInit(void);
/*
** ADC1 获取ADC采样AD Value
*/
uint16 ADC_GetADResultAD(uint8 channel);
/*
** ADC3 获取ADC采样AD Value
*/
uint16 ADC3_GetADResultAD(uint8 ch);
#endif

