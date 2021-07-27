#ifndef __GPIO_H_
#define __GPIO_H_

/*
** Output
*/
#define Upgr_FlickerLedOn								(GPIOG->BSRRL = GPIO_Pin_10)/*升级闪烁灯*/
#define Upgr_FlickerLedOff								(GPIOG->BSRRH = GPIO_Pin_10)
#define Fan_On												(GPIOG->BSRRL = GPIO_Pin_4)/*交流风扇*/
#define Fan_Off												(GPIOG->BSRRH = GPIO_Pin_4)
#define Fan2_On											(GPIOG->BSRRL = GPIO_Pin_5)
#define Fan2_Off											(GPIOG->BSRRH = GPIO_Pin_5)

#define FaultLed_On										(GPIOG->BSRRL = GPIO_Pin_6)
#define FaultLed_Off										(GPIOG->BSRRH = GPIO_Pin_6)
#define FaultLed_On_12V								(GPIOG->BSRRL = GPIO_Pin_3)
#define FaultLed_Off_12V								(GPIOG->BSRRH = GPIO_Pin_3)

#define AcSlave_Ctr_On									(GPIOG->BSRRL = GPIO_Pin_7)
#define AcSlave_Ctr_Off									(GPIOG->BSRRH = GPIO_Pin_7)

#define Ac_Ctr_On											(GPIOG->BSRRH = GPIO_Pin_8)
#define Ac_Ctr_Off											(GPIOG->BSRRL = GPIO_Pin_8)

/*
** 主控运行灯
*/
#define Ctr_RunLed_On									(GPIOF->BSRRL = GPIO_Pin_10)
#define Ctr_RunLed_Off									(GPIOF->BSRRH = GPIO_Pin_10)

/*
** Input 
*/
#define Yx_In_Emer										(GPIOF->IDR & GPIO_Pin_13)
#define Yx_In_Smoke										(GPIOF->IDR & GPIO_Pin_11)/*烟感:高--故障 低--正常 故障断开接触器*/
#define Yx_In_Anti											(GPIOF->IDR & GPIO_Pin_12)/*防雷:高--故障 低--正常*/

/*
** 固件标号 Output
*/
enum{
	setFirewareUpgre_Label = 0x00,/*升级闪烁灯标号*/
	setFirewareFan_Label = 0x01,/*风扇标号*/
	setFireACSlaveCtr_label = 0x02,/*控制辅电*/
	setFireACCtr_Label = 0x03,/*控制主电*/
	setFireFalutLed_Label = 0x04,/*故障灯*/
	setFireCtrRunLed_Label = 0x05,/*主控运行灯*/
};

/*
** 固件标号 Input
*/
enum{
	getYxIn_Emer = 0x00,/*急停*/
	getYxIn_Smoke = 0x01,/*烟感*/
	getYxIn_Anti = 0x02,/*防雷*/
};

/*
** 固件引脚配置
*/
void GPIO_Init_Config(void);
/*
** 固件控制 Output
** @param:
**			label-->固件标号
**			flag -->控制逻辑标志
*/
void firewareCtr(uint8 label, bool flag);
/*
** 固件状态获取 Input
** @param:
**			label-->固件标号
** @return:
**			1-->故障
**			0-->正常
*/
uint8 firewareYxIn(uint8 label);
#endif

