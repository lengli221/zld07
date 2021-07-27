#ifndef __CALCPHASEVOLCUR_H_
#define __CALCPHASEVOLCUR_H_

/*
** 三相电压电流计算结构体声明
*/
typedef struct{
	float abcPV[3];/*A,B,C相电压*/
	float abcPC[3];/*A,B,C相电流*/
}CalcPhaseVC;

/*-----------------------------------三相电压校准-------------------------------------------------*/
#pragma pack(1)
typedef struct{
	bool flag[3];/*是否已完成校准标志*/	
	float k[3];/*三相系数校准值*/
	float b[3];
	uint16 volt[3];/*三相电压*/
}PhaseVoltAdjustLogic;
#pragma pack()
/*------------------------------------------------------------------------------------------------*/

/*
** get_计算三相电压电流
** 		策略:单点计算
*/
CalcPhaseVC get_CalcPhaseVC(void);

/*
** 获取低压侧AD值
*/
uint16 get_LvAdValue(uint8 ch);

/*
** 获取高压侧AD值
*/
uint16 get_HvAdValue(uint8 ch);

/*
** 设置校准系数
*/
void set_PhaseVoltAdjust(uint8 ch,bool flag,float k,float b);
/*
** 复位初始化校准系数
*/
void reset_PhaseVoltAdjust(void);
#endif

