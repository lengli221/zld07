#ifndef __CABINETCTRINFO_H_
#define __CABINETCTRINFO_H_

/*
** 控制信息之接触器控制命令
*/
void cabCtrInfo_EventId_ContactorFun(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 控制信息之更新SOC参数
*/
void cabCtrInfo_EventId_UpdateSocPara(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 充电时间阈值
*/
void cabCtrInfo_EventId_ChgTime(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 充电器过温阈值
*/
void cabCtrInfo_EventId_ChgOverTemp(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 充电器仓过温阈值
*/
void cabCtrInfo_EventId_ChgDoorOT(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 整柜控制信息之系统温度相关信息
*/
void cabCtrInfo_EventId_SysTempInfo(uint8 itemDatalen,uint8* rx,
	uint8*replyItemDataLen,uint8* tx);
/*
** 控制板执行BootLoader策略:
**		1.接收远程升级指令
**		2.等待执行回复,800ms之后重启主控进入BootLoader
*/
void enterCtrBootLoader(void);
/*
** 下载通讯板/电池包文件策略:
**		1.接收下载通讯小板命令
**		2.等待执行回复命令,间隔800ms之后控制上层任务切换
*/
void ctrBoard_TaskSwitch(void);
/*
** set 恢复上层协议之APP应用层协议标志
*/
void set_CtrBoardTaskRecoverFlag(void);
/*
** 恢复上层协议之APP应用层协议 
**		控制策略:
**				1.文件下载完成之后,设置标志
**				2.等待回复(延时)800ms之后设置任务切换
*/
void ctrBoard_TaskRecover(void);
/*
** 整柜控制信息之下载文件
*/
void cabCtrInfo_EventId_UpgradeFileCmd(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 整柜控制信息之下载文件
*/
void cabCtrInfo_EventId_GetFileName(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 整柜控制信息之获取指定电池包固件信息
*/
void cabCtrInfo_EventId_GetBatFireInfo(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 查询所有充电器的温度
*/
void cabCtrInfo_EventId_AllchgTemp(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 查询BMS保护状态/故障状态
*/
void cabCtrInfo_EventId_BmsPFState(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 获取WIFI名称和密码
*/
void cabCtrInfo_EventId_GetWifiNamePw(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 查询SOC阈值
*/
void cabCtrInfo_EventId_ChkSocLimit(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 查询通讯板运行版本号/电池固件版本号
*/
void cabCtrInfo_EventId_ChkRunComVer(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*-------------------------------------------三相电压校准值--------------------------------------------------------------------------*/
/*
** 查询低压侧AD采样值
*/
void cabCtrInfo_EventId_ChkPhaseLvVoltAD(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 查询高压侧采样值
*/
void cabCtrInfo_EventId_ChkPhaseHvVoltAD(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 设置三相电压校准系数
*/
void cabCtrInfo_EventId_SetPhaseAjust(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*
** 获取实时电压
*/
void cabCtrInfo_EventId_GetRuntimeVolt(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx);
/*-----------------------------------------------------------------------------------------------------------------------------------*/
#endif

