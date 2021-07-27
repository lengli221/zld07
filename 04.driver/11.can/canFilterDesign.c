#include "includes.h"

/*
** 系统之滤波设计
*/
 SysFilterDesign sysFilterDesign;

/*
** 系统之元目标地址
*/
IdAddr sysIdAddr;

/*
** 系统之滤波设计之目标地址根据柜子数目来限定地址值
*/
uint8 SysFilterDesign_DstAddrLimit(void){
	uint8 i=0;
	uint8 ret = 0;
	uint8 param = SysCtr_AllDoorNum - 0x01;
	
	for(i=7;i>0;i--){/*目标地址最高使用8位地址*/
		if(param&(0x01<<i)){
			ret = (i+1);
			break;
		}	
	}
	return ret;
}

/*
** 系统之滤波器设计
*/
void SysFilterDesign_Param(void){
	/*
	** CAN1之滤波设计
	*/
	sysFilterDesign.filterDesign1.id.bits.rtr = 0;/*数据帧*/
	sysFilterDesign.filterDesign1.id.bits.ide = 1;/*扩展帧*/
	sysFilterDesign.filterDesign1.id.bits.src = 0x00;/*源地址--通讯小板*/
	sysFilterDesign.filterDesign1.id.bits.dst = SrcAddr_Define;/*目标地址--控制板*/

	/*
	** 源地址必须在地址限定范围内
	*/
	sysFilterDesign.filterDesign1.idMask.bits.src = (uint8)(0xFF<<SysFilterDesign_DstAddrLimit());
	/*
	** 目标地址必须匹配
	*/
	sysFilterDesign.filterDesign1.idMask.bits.dst = (uint8)0xFF;
}

/*
** 系统之获取滤波参数
*/
FilterDesign Sys_GetFilterDesign1(void){
	return(sysFilterDesign.filterDesign1);
}

/*
** 系统之源目标地址初始化
*/
void Sys_IdAddrInit(void){
	sysIdAddr.src = SrcAddr_Define;
	sysIdAddr.dst = 0;
}

/*
** 系统之获取源,目标地址
*/
IdAddr Sys_GetIdAddr(void){
	return(sysIdAddr);
}

/*
** 系统之滤波设计初始化参数
*/
void SysFilterDesign_ParamInit(void){
	memset((uint8*)&sysFilterDesign.filterDesign1.id.id,0x00,sizeof(SysFilterDesign));
	memset((uint8*)&sysIdAddr.src,0x00,sizeof(IdAddr));
	/*
	** 系统之滤波器设计
	*/
	SysFilterDesign_Param();
	/*
	** 系统之源目标地址初始化
	*/
	Sys_IdAddrInit();	
}

