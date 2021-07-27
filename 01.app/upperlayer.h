#ifndef __UPPERLAYER_H_
#define __UPPERLAYER_H_

#include "sysCtrLogic.h"

/*
** 帧头帧尾
*/
#define UpperLayerFrame_Head						(uint8)0x68
#define UpperLayerFrame_End							(uint8)0x16

/*
** 帧长度合法性限制--整柜控制信息包含所有事件为最长下发帧--参见协议版_v0.03
*/
#define FrameLenIsLegal_Min							(uint16)0x0009
#define FrameLenIsLegal_Max							(uint16)0x0039

/*
** 帧ID
** 1.解析上层协议帧
** 2.回复上层协议帧
*/
#define CMD_ID_Register_Rx							(uint8)0x01/*注册信息*/
#define CMD_ID_Logout_Rx								(uint8)0x02/*注销信息*/
#define CMD_ID_StateInfoChange_Rx				(uint8)0x03/*状态信息变更*/
#define CMD_ID_StateInfo_Rx							(uint8)0x04/*状态信息*/
#define CMD_ID_Ctr_Rx									(uint8)0x05/*控制信息*/
#define CMD_ID_BmsInfo_Rx							(uint8)0x06/*BMS信息*/
#define CMD_ID_Domain_Rx							(uint8)0x07/*域名信息*/
#define CMD_ID_BatMainInfo_Rx						(uint8)0x08/*电池关键信息*/

#define CMD_ID_Register_Tx							(uint8)0x81
#define CMD_ID_Logout_Tx								(uint8)0x82
#define CMD_ID_StateInfoChange_Tx				(uint8)0x83
#define CMD_ID_StateInfo_Tx							(uint8)0x84
#define CMD_ID_Ctr_Tx									(uint8)0x85
#define CMD_ID_BmsInfo_Tx							(uint8)0x86
#define CMD_ID_Domain_Tx								(uint8)0x87
#define CMD_ID_BatMainInfo_Tx						(uint8)0x88


/*
** 控制信息之事件控制
*/
enum{
	CabCtrInfo_EventId_Contactor 				= 0x0010,/*接触器控制命令*/
	CabCtrInfo_EventId_UpdateSoc 				= 0x0020,/*更新SOC参数值*/
	CabCtrInfo_EventId_ChgTime					= 0x0030,/*充电时间阈值*/
	CabCtrInfo_EventId_ChgOverTemp			= 0x0040,/*充电器过温阈值*/
	CabCtrInfo_EventId_ChgDoorOT				=0x0050,/*充电器仓过温阈值*/
	CabCtrInfo_EventId_SysTempInfo				= 0x0060,/*系统温度信息*/
	CabCtrInfo_EventId_UpgrCmd					= 0x0070,/*远程升级命令*/
	CabCtrInfo_EventId_GetFileName 				= 0x0080,/*文件名*/
	CabCtrInfo_EventId_GetBatFireInfo			= 0x0090,/*获取指定电池包固件信息*/	
	CabCtrInfo_EventId_GetchgerTemp 			= 0x00B0,/*充电器温度*/
	CabCtrInfo_EventId_GetWifiNamePw			= 0x00C0,/*获取wifi名称和密码*/
	CabCtrInfo_EventId_BmsPF						= 0x00D0,/*BMS保护状态/故障状态*/
	CabCtrInfo_EventId_ChkSocLimit				= 0x00E0,/*查询SOC阈值*/
	CabCtrInfo_EventId_ChkRunComVer 			= 0x00F0,/*查询运行版本(通讯板/电池固件)*/
	/*-------------------------------------------三相电压校准值--------------------------------------------------------------------------*/
	CabCtrInfo_EventId_ChkLvAd					= 0x2120,/*查询低压侧AD值*/
	CabCtrInfo_EventId_ChkHvAd					= 0x2220,/*查询高压侧AD值*/
	CabCtrInfo_EventId_SetPAdjust				= 0x2420,/*设置校正系数*/	
	CabCtrInfo_EventId_GetRuntimeV				= 0x2320,/*获取实时电压*/
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	CabCtrInfo_EventId_CfgFireUpgrN			= 0x0100,/*配置相应固件包升级次数*/
	CabCtrInfo_EventId_ClearFire					= 0x0110,/*清除相应固件包*/
	CabCtrInfo_EventId_UpgrFailR					= 0x0120,/*对应仓固件升级失败的原因*/
	CabCtrInfo_EventId_FireEnterT				= 0x0130,/*固件进入测试模式*/
	CabCtrInfo_EventId_ChkRealityN				= 0x0140,/*查询实际固件升级次数*/
	CabCtrInfo_EventId_TestMUpgrNum			= 0x0150,/*测试模式下查询固件升级次数*/
	CabCtrInfo_EventId_ChargerOCL				= 0x0160,/*充电器过温阈值*/
	/*---------------------------------------------------------清除固件------------------------------------------------------------------*/
	CabCtrInfo_EventId_OneClearAllFire 			= 0x3030,/*清除分控/电池/充电器固件--清除所有固件*/
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	/*----------------------------美团充电柜-第三方交互方案--后端接口(外部输出)-1.05-----------------------------------------------------*/
	CabCtrInfo_EventId_BatOTempLimit			= 0x0170,/*设置过温阈值*/
	cabCtrInfo_EventId_NanduLowPLimit			= 0x0180,/*设置南都低温阈值*/
	CabCtrInfo_EventId_BatLowPLimit			= 0x0190,/*设置电池低温阈值*/
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	/*-------------------------------------------硬件接口切换-----------------------------------------------------------------------------*/
	CabCtrInfo_EventId_SetInterfaceSw			= 0x2345,/*硬件接口切换*/
	/*------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------电池接入SOC----------------------------------------------------------------*/
	CabCtrInfo_EventId_ChkAllInsSoc				= 0x01A0,
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	CabCtrInfo_PhaseVCDebugMode				= 0x3131
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
};
	
/*
** 上层协议之结构体声明
*/
#pragma pack(1)
typedef struct{
	uint8 fireId[10];/*厂家标识(柜子型号)-->String*/
	uint8 id[16];/*整柜ID-->String*/
	uint8 ctrHareVer[4];/*控制板硬件版本号->String*/
	uint8 ctrSoftVer[4];/*控制板软件版本号->String*/
	uint8 ctrModel[6];/*主控型号*/
}SysSet;/*系统设置参数*/

typedef union{
	uint16 flag;
	struct{
		uint16 isOnline:1;/*网络*/
		uint16 interCom:1;/*内部通讯*/
		uint16 smoke:1;/*烟雾*/
		uint16 fan:1;/*风扇*/
		uint16 res:12;
	}bits;
}State;

typedef struct{
	State state;/*系统状态*/
	int16 chgDoorTemp;/*充电器仓环温*/
}SysFire;/*系统固件参数*/

typedef struct{
	DoorNumDefine batOnline;/*电池是否在线 置1:在线*/
	DoorNumDefine chgOverTemp;/*充电器过温 置1:充电器过温*/
	DoorNumDefine batComErr;/*电池通讯故障*/
	DoorNumDefine chargingErr;/*充电异常*/
	DoorNumDefine batLowTemp;/*电池低温*/
	DoorNumDefine batIsErr;/*电池是否故障 置1:故障 -- 暂时仅针对电池过温*/
	DoorNumDefine batFault;/*电池:置1故障*/
	DoorNumDefine comFault;/*分控:置1故障*/
	DoorNumDefine batChgOTime;/*电池充电时间过长*/
	DoorNumDefine comIsOnline;/*分控板是否在线 置1:离线*/
	DoorNumDefine comUpgr;/*分控板升级 置1:升级中*/
	DoorNumDefine comUpgrIsOk;/*分控板升级是否成功  置1:升级成功*/
	DoorNumDefine batFileDownload;/*电池文件下载中*/
	DoorNumDefine batFileDownloadIsOk;/*电池文件下载完成 置1:下载完成*/
	DoorNumDefine batUpgr;/*电池升级 置1:升级中*/
	DoorNumDefine batUpgrIsOk;/*电池升级是否成功 置1:升级成功*/
}SysLogic;

typedef union{
	uint16 flag;
	struct{
		uint16 emer:1;/*急停按钮 置1:故障*/
		uint16 smoke:1;/*烟感 置1:故障*/
		uint16 phaseOverVFault:1;/*相电压过压>270V,需下电才能恢复*/
		uint16 phaseOverVWaring:1;/*相电压过压可恢复*/
		uint16 ledOpen:1;/*声光报警 置1:开启*/
		uint16 fanOpen:1;/*风扇 置1:开启*/
		uint16 majorLoopClose:1;/*主回路 置1:断开--修改成:辅回路*/
		uint16 phaseLV:1;/*相电压欠压,可恢复*/
		uint16 phaseOC:1;/*相电流过流*/
		uint16 phaseCShort:1;/*相电流短路*/
		uint16 comIdRep:1;/*分控板ID重复 置1:异常*/
		uint16 comErr:1;/*分控板通讯 置1:异常*/
		uint16 chgDoorOverTemp:1;/*充电仓过温 置1:过温*/
		uint16 comRecoverFlag:1;/*分控一级报警之后复位标志*/
		uint16 anti:1;/*防雷*/
		uint16 ctrUpgr:1;/*主控升级中*/
	}bits;
}SysModuleStateInfo;

typedef union{
	uint16 flag;
	struct{
		uint16 hardSwitchClose:1;/*硬件开关断开*/
		uint16 abcPhaseCurO:3;/*ABC三相电流*/
		uint16 abcPhaseCurShort:3;/*三相电流短路*/
		uint16 testModelFlag:1;/*测试模式标志--整柜存在测试模式*/
		uint16 res_1:8;/*保留位*/
	}bits;
}SysModuleStateInfo_2;

typedef struct{
	uint8 idle;/*空仓数*/
	uint8 ing;/*充电仓数*/
	uint8 full;/*满仓数*/
	uint8 abn;/*异常挂起数*/
	uint8 total;/*总仓数*/
}DoorTypeNum;

typedef struct{
	SysLogic sysLogic;
	uint8 sameFireRepCnt;/*同一固件升级次数*/
	SysModuleStateInfo sysModuleStateInfo;
	SysModuleStateInfo_2 sysModuleStateInfo_2;
	DoorTypeNum doorTypeNum;
}StateInfoChange;

typedef struct{
	DoorNumDefine batOnlineCtrAC;/*置1:在线开启AC*/
	DoorNumDefine remoteCtrAC;/*置1:远程控制AC*/
	DoorNumDefine sysErrCtrAC;/*置1:系统故障断AC*/
	DoorNumDefine comIdRep;/*置1:ID重复*/
	DoorNumDefine batOnline_1minNoCur;/*置1:电池在线,持续1min无电流故障-->充电器故障*/
}StateInfo;

typedef struct{
	uint16 dataItemlen;/*数据项长度*/
	uint8 tx[128];/*数据项内容*/
}CabinetCtrInfoReply;

typedef struct{
	bool flag;
	uint32 itick;
}UpgrCtrReset;/*控制板确认允许升级控制板之后等待回复(延时)之后进入bootLoader区*/

typedef struct{
	bool flag;
	uint32 itick;
	uint32 itickout;/*下载文件超时定时器*/
}UpgrComBatTaskEn;/*控制板确认允许下载通讯板/电池包文件之后等待回复(延时)之后启用任务切换*/

typedef struct{
	SysSet sysSet;
	SysFire sysFire;
	StateInfoChange stateInfoChange;
	StateInfo stateInfo;
	CabinetCtrInfoReply cabinetCtrInfoReply;
	UpgrCtrReset upgrCtrReset;
	UpgrComBatTaskEn upgrComBatTaskEn;
	UpgrComBatTaskEn upgrComBatTaskDis;
	/*增加所有通讯板的运行版本号上报*/
	uint16 runComVer[45][SysCtr_AllDoorNum];/*note:runComVer[0]:通讯板自身运行版本,其余均为电池运行版本*/
}UpperLayerPara;

typedef struct{
	bool flag;/*域名信息*/
}Domain;

typedef struct{
	bool flag;/*是否注册*/
}RegisterLogic;

typedef struct{
	bool flag;/*是否注销*/
}LogoutLogic;

typedef struct{
	Domain domain;/*域名信息*/
	RegisterLogic registerLogic;/*注册流程*/
	LogoutLogic logoutLogic;/*注销流程*/
}UpperLayerReplyPara;

/*
** 操作进程系统结构体相关进程--上报上层协议定义结构体
*/
typedef struct{
	bool refresh;/*更新标志*/
	uint8 id;/*消息ID*/
	uint8 deviceAddr;/*设备地址--参见协议总线地址说明部分(暂时保留)*/
	uint8 doorAddr;/*仓门地址*/
}SmCmd;

/*
** 帧格式
*/
typedef struct{
	uint8 id;/*消息id*/
	uint16 dataLen;/*数据长度*/
	uint8 addr;/*从设备地址*/
}MsgHead;

typedef struct{
	uint8 head;/*帧起始符*/
	uint16 msgFrameLen;/*帧长度*/
	MsgHead msgHead;/*消息头*/
	uint8 chkCode;/*校验码*/
	uint8 end;/*帧结束符*/
}UpperLayerFrame;

/*
** 控制信息之接口函数结构体
*/
typedef struct{
	uint16 itemId;//事件ID
	void (*handle)(uint8,uint8*,uint8*,uint8*);/*事件项数据长度,事件项内容 回复数据项长度 回复数据项内容*/
}CabinetCtrInfo;

/*
** 功能函数之解析上层协议
*/
typedef struct{
	uint8 id;/*消息ID*/
	void (*handle)(uint16,uint8*);/*数据项长度,数据项*/
}UpperLayerFunTable;

/*
** 功能函数之回复上层协议
*/
typedef struct{
	uint8 id;/*消息ID*/
	void (*handle)(UpperLayerFrame*,uint8*,uint8);/*消息格式,数据项域,仓门地址*/
}UpperLayerReplyFunTable;
#pragma pack()

/*
** 上层协议之结构体变量指针提供结构调用
*/
UpperLayerPara* getUpperLayerPara(void);
/*
** 上层协议之回复参数变量接口函数之外部函数调用
*/
UpperLayerReplyPara* getULReplyPara(void);
/*
** 提供解析文件调用接口函数
** 1.设置命令参数
** 2.查询命令参数
*/
void setCmd(SmCmd cmd);
uint8 getCmd(void);
/*
** 提供系统软件创建上层协议解析接口函数
*/
void UpperLayerParse_Init(void);
/*
** 提供系统软件创建上层协议回复接口函数
*/
void UpperLayerReply_Init(void);
#endif

