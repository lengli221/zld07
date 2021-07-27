#ifndef __CANFILTERDESIGN_H
#define __CANFILTERDESIGN_H

/*
**目标地址之#Define声明
*/
#define SrcAddr_Define							(uint8)0x21

/*
** CAN之标识符结构体
*/
typedef struct{
	uint8 src;/*源地址*/
	uint8 dst;/*目标地址*/
}IdAddr;

/*
**Identifier 组成部分
*/
typedef union{
	uint32 id;
	struct{
		uint32 res0:1;
		uint32 rtr:1;
		uint32 ide:1;
		uint32 src:8;
		uint32 dst:8;
		uint32 res1:13;
	}bits;
}IdMakeUp;

/*
**CAN之滤波设计结构体
*/
typedef struct{
	IdMakeUp id;
	IdMakeUp idMask;
}FilterDesign;

/*
** CAN之系统滤波
*/
typedef struct{
	FilterDesign filterDesign1;/*系统滤波之CAN1*/
}SysFilterDesign;

/*
** 系统之获取滤波参数
*/
FilterDesign Sys_GetFilterDesign1(void);
/*
** 系统之获取源,目标地址
*/
IdAddr Sys_GetIdAddr(void);
/*
** 系统之滤波设计初始化参数
*/
void SysFilterDesign_ParamInit(void);

#endif

