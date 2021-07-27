#ifndef __LOWERLAYER_H_
#define __LOWERLAYER_H_

/*
** 系统控制参数之参数#define限制
** 1.帧重帧参数
** 
*/
#define LLParse_FrameCntMax							(uint8)0x0A//0x05	

/*
** 系统模块故障信息之屏蔽位参数
*/
#define SysModuleStateInfo_MaskBit_Upgr			(uint16)0xD38F

/*
** 下层协议之系统模块处理之功能函数ID声明
** 1.电池舱状态
*/
#define LL_FunId_BatDoorState						(uint8)0x01
#define LL_FunId_BatUpgrState						(uint8)0x02
#define LL_FunId_ChgSysInfo							(uint8)0x03
#define LL_FunId_BmsInfo								(uint8)0x80


/*
** 限制内存对齐
*/
#pragma pack(1)
typedef struct{
	uint8 fireCode;/*厂商代码--兼容不同客户端Bms协议*/
}Compatibility;

typedef struct{
	uint32 itick;/*用于超时*/
	uint8 cnt;
	bool comAbn;/*通讯丢失*/
}ComBoardChk;

typedef union{
	uint16 flag;
	struct{
		uint16 batOnline:1;/*电池在线*/
		uint16 batDoorChargerOT:1;/*电池仓充电器过温*/
		uint16 batOT:1;/*电池过温*/
		uint16 chargerErr:1;/*充电器故障(电池在线,持续1min无电流故障)*/
		uint16 batOnlineOpenAC:1;/*电池在线开启AC*/
		uint16 remoteCloseAC:1;/*远程断AC*/
		uint16 sysErrCloseAC:1;/*系统故障断充电器AC*/
		uint16 comIdRep:1;/*通讯板ID重复*/
		uint16 batTrans:1;/*电池反接--更改为电池故障*/
		uint16 bmsErr:1;/*BMS板烧毁,电池故障*/
		uint16 batUpgrIsFinsh:1;/*电池升级中*/
		uint16 batUpgrIsOk:1;/*电池是否升级成功*/
		uint16 comIsFault:1;/*分控故障*/
		uint16 chargingErr:1;/*充电异常*/
		uint16 batLowTemp:1;/*电池低温*/
		uint16 oneErrRecover:1;/*一级警报分控复位标志*/
	}bits;
}BatDoorStateInfo;


typedef struct{
	uint8 comHareVer[4];/*分控板硬件版本号-->String*/
	uint8 comSoftVer[4];/*分控板软件版本号-->String*/
	uint8 chgBefSoc;/*电池接入时SOC*/
	uint16 chgTime;/*充电时长*/
	uint16 insertTime;/*接入时长*/
	int16 chargerTemp;/*充电器温度*/
}BatDoorSysPara;

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
typedef union{
	uint16 flag;
	struct{
		uint16 res:16;
	}bits;
}PState;

typedef union{
	uint8 flag;
	struct{
		uint8 res:8;
	}bits;
}FaultState;

typedef union{
	uint8 flag;
	struct{
		uint8 res:8;
	}bits;
}MosState;

typedef struct{
	uint16 realVol;/*实时电压*/
	int16 realCur;/*实时电流*/
	uint16 surplusCap;/*剩余容量*/
	uint16 circNum;/*循环次数*/
	uint16 singleBatVol[16];/*单节电池电压*/
	uint8 soh;
	uint8 batID[16];/*电池ID*/
	uint8 soc;
	uint16 sop;/*充电sop 充电限流*/
	uint8 batWorkMode;/*电池工作模式*/
	uint8 batChgMode;/*电池充电模式*/
	uint8 workState;/*工作状态*/
	PState pState;/*保护状态*/
	FaultState faultState;/*故障状态*/
	MosState mosState;/*MOS状态*/
	int8 batMaxTemp;/*电池最高温度*/
	int8 batMinTemp;/*电池最低温度*/
	int8 mosMaxTemp;/*MOS最高温度*/
	int8 pcbTemp;/*PCB温度*/
	uint16 chgNum;/*充电次数*/
	uint16 disChgNum;/*过放次数*/
	uint16 shortNum;/*短路次数*/
	int8 hisMaxBatTemp;/*历史最大电池温度*/
	uint8 bootVer;/*BOOT版本号*/
	uint8 firmCode;/*厂商代码*/
	uint16 hardVer;/*硬件版本*/
	uint16 softVer;/*软件版本*/
	uint16 designCap;/*设计容量*/
	uint8 batType;/*电池型号*/
}BmsInfoMeiTuan;
#endif

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
typedef struct{
	uint16 batTemp;/*电池温度*/
	uint16 batTotalVol;/*电池总电压*/
	uint16 realCur;/*实时电流*/
	uint8 relaCapPer;/*相对容量百分比*/
	uint16 surplusCap;/*剩余容量*/
	uint16 fullCap;/*满充容量*/
	uint16 circNum;/*循环次数*/
	uint16 singleBatVol[13];/*单节电池电压*/
	uint8 soh;
	uint8 ver[3];/*版本号*/
	uint8 batID[20];/*电池ID*/
}BmsInfoPineCone;
#endif

typedef struct{
	BatDoorStateInfo batDoorStateInfo;
	BatDoorSysPara batDoorSysPara;
	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
		BmsInfoMeiTuan bmsInfoMeiTuan;
	#endif
	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
		BmsInfoPineCone bmsInfoPineCone;
	#endif
}BatDoor;

typedef struct{
	Compatibility compatibility[SysCtr_AllDoorNum];
	ComBoardChk comBoardChk[SysCtr_AllDoorNum];
	BatDoor batDoor[SysCtr_AllDoorNum];
}LowerLayerParam;
#pragma pack()

/*
** 下层协议之系统模块处理列表结构体
*/
typedef struct{
	uint8 funId;/*功能函数ID*/
	void (*handle)(uint8,uint8*,uint8);/*1.数据长度 2.数据项 3.设备地址*/
}LowerLayerFunTable;


/*
** 下层协议之参数接口提供调用
*/
LowerLayerParam* getLowerLayerParaPtr(void);
/*
**提供系统软件创建下层协议解析接口函数
*/
void LowerLayerParse_Init(void);
/*
**提供系统软件创建下层协议查询接口函数
*/
void LowerLayerReply_Init(void);
#endif

