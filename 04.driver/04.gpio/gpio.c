#include "includes.h"

/*
** 固件引脚配置
*/
void GPIO_Init_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF , ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG , ENABLE);

	/*
	** 输入引脚配置
	*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	/*
	** PF11:烟感信号 PF12:防雷信号 PF13:急停信号
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	/*
	** 输出引脚配置
	*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	

	/*
	** PF10--运行闪烁灯
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	/*
	** PG3:故障灯12V PG4--PG5:交流散热风扇 PG6:故障灯 PG7:控制辅电 PG8:交流断路器 PG10:升级闪烁灯
	*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4| GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_10;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	

	/*
	** 固件初始化设置
	**			1.控制板之升级灯闪烁关闭
	**			2.交流断路器控制--控制主回路和辅电
	**			3.AC控制辅电
	**			4.故障灯
	**			5.风扇控制
	**			6.故障灯PG3 12V故障
	*/
	Upgr_FlickerLedOff;Ac_Ctr_On;AcSlave_Ctr_On;FaultLed_Off;Fan_Off;Fan2_Off;FaultLed_Off_12V;
}

/*
** 固件控制 Output
** @param:
**			label-->固件标号
**			flag -->控制逻辑标志
*/
void firewareCtr(uint8 label, bool flag){
	switch(label){
		case setFirewareUpgre_Label:
			if(flag == true){
				Upgr_FlickerLedOn;
			}else{
				Upgr_FlickerLedOff;
			}
			break;
		case setFirewareFan_Label:
			if(flag == true){
				Fan_On;
				Fan2_On;
			}else{
				Fan_Off;
				Fan2_Off;
			}
			break;
		case setFireACSlaveCtr_label:
			if(flag == true){
				AcSlave_Ctr_On;
			}else{
				AcSlave_Ctr_Off;
			}
			break;
		case setFireACCtr_Label:
			if(flag == true){
				Ac_Ctr_Off;
			}else{
				Ac_Ctr_On;
			}
			break;
		case setFireFalutLed_Label:
			if(flag == true){
				FaultLed_On;
				FaultLed_On_12V;
			}else{
				FaultLed_Off;
				FaultLed_Off_12V;
			}
			break;
		case setFireCtrRunLed_Label:
			if(flag == true){
				Ctr_RunLed_On;
			}else{
				Ctr_RunLed_Off;
			}	
			break;
	}
}

/*
** 固件状态获取 Input
** @param:
**			label-->固件标号
** @return:
**			1-->故障
**			0-->正常
*/
uint8 firewareYxIn(uint8 label){
	switch(label){
		case getYxIn_Emer:
			return ((Yx_In_Emer == 0)?1:0);
		case getYxIn_Smoke:
			return ((Yx_In_Smoke == 0)?0:1);
		case getYxIn_Anti:
			return ((Yx_In_Anti == 0)?0:1);
	}
	
	return 0;
}

