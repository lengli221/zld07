#ifndef __PHASEVCLIMITANALSZE_H
#define __PHASEVCLIMITANALSZE_H

/*
** 定时器时间基准时间
*/
#define tickTimeBase_V									(uint32)60000

/*
** 相电压阈值分析标志结构体
*/
typedef union{
	uint8 flag;
	struct{
		uint8 a:1;/*A相*/		
		uint8 b:1;/*B相*/
		uint8 c:1;/*C相*/
		uint8 res:5;
	}bits;
}PVol;

typedef struct{
	uint8 flag;
	struct{
		uint8 a:1;/*A相*/		
		uint8 b:1;/*B相*/
		uint8 c:1;/*C相*/
		uint8 res:5;
	}bits;
}PCur;

typedef struct{
	PVol pVol;
	PCur pCur;
}PhaseVCAnalyze;

/*
** 相电压/相电流异常处理策略
** 		策略一:关闭所有充电器
*/
bool PhaseVCAbnCloseAllCharger(void);
#endif

