#ifndef __SYSPARAM_H_
#define __SYSPARAM_H_

#include "lcdData_DW.h"

/*
** 开辟空间限制
**		1.电池固件包个数空间开辟和充电器固件包个数空间开辟
*/
#define UpgrBatMemoryNum							(uint8)44
/****************************系统升级地址分配Backup和EEPROM**********************************/
#define UpgradeKey_Head								(uint8)0xA5

/*backup区域参数地址分配*/
#define ByteReserveOffset								(0x04)/*预留之增加秘钥处理*/

#define UpgradeKey_Addr								0x0000
#define UpgradeKey_Len_Addr							sizeof(UpgradeKey)+0x03/*head+crc16*/

#define UpgradeVer_Addr								ByteReserveOffset + UpgradeKey_Addr + UpgradeKey_Len_Addr
#define UpgradeVer_Len_Addr							sizeof(uint32) + 0x03

#define UpgradeFileNameLen_Addr					ByteReserveOffset+UpgradeVer_Addr+UpgradeVer_Len_Addr
#define UpgradeFileNameLen_Len_Addr				sizeof(uint16)+0x03

#define UpgradeFileName_Addr							ByteReserveOffset + UpgradeFileNameLen_Addr + UpgradeFileNameLen_Len_Addr
#define UpgradeFileName_Len_Addr					128 + 0x03

#define UpgradeOffset_Addr							0x00B4/*偏移地址:180*//*0x100*/

/****************************************************************************************************************************************/
/*
** 设置保存注册信息参数地址
*/
 #define Register_LogoutInfo_Addr					UpgradeOffset_Addr
 #define Register_LogoutInfo_Len						(sizeof(uint8) + sizeof(uint16))

 #define LocalRemoteDomain_Addr						Register_LogoutInfo_Addr + Register_LogoutInfo_Len
#define LocalRemoteDomain_Len						(128 + sizeof(uint16))
#define SysDeviceID_Addr								LocalRemoteDomain_Addr + LocalRemoteDomain_Len
#define SysDeviceId_Len									sizeof(DeviceId) + sizeof(uint16)

#define WifiNamePara_Addr								(SysDeviceID_Addr + SysDeviceId_Len)
#define WifiNamePara_Len								(32+sizeof(uint16))

#define WifiPwPara_Addr								(WifiNamePara_Addr + WifiNamePara_Len)
#define WifiPwPara_Len									(32+sizeof(uint16))

/*
** 20120324--鉴于第一个电池固件包允许读取次数和早期发货版本电池固件包次数地址和存储字节长度重叠,导致电池包个数误判定为第一个电池固件限制次数
** 解决方案:新增2个偏移地址在上述wifi密码地址基础上
** 本次修改EEPROM地址鉴于修改整个存储逻辑--采用映射所引发逻辑--现场评估:目前美团第一个电池固件暂未使用,不会有任何影响
*/
#define Android_Effective_Addr 						(WifiPwPara_Addr + WifiPwPara_Len + sizeof(uint16))

/*
** 电池相应固件允许升级次数
*/
#define BatFireLimitNum_Addr(loc)							(Android_Effective_Addr + loc*BatFireLimitNum_Len)
#define BatFireLimitNum_Len							(sizeof(int16) + sizeof(uint16))
/*
** 电池固件实际升级次数
*/
#define BatRealityUpgrNum_Addr(loc)							((Android_Effective_Addr + UpgrBatMemoryNum*BatFireLimitNum_Len) + loc * BatRealityUpgrNum_Len)
#define BatRealityUpgrNum_Len						(sizeof(uint16) + sizeof(uint16))

/*
** 电池包参数(硬件版本号,电池ID)个数
*/
#define UpgrBatFileNumAddr							((Android_Effective_Addr + UpgrBatMemoryNum*BatFireLimitNum_Len) + UpgrBatMemoryNum * BatRealityUpgrNum_Len)
#define UpgrBatFileNumLen								sizeof(uint16) + sizeof(uint16)/*CRC校验*/

/*预留充电器固件包个数-->兼容12A充电器/电池固件包位置信息*/
#define UpgrChargeNumAddr							(UpgrBatFileNumAddr+UpgrBatFileNumLen)
#define UpgrChargeNumLen								sizeof(uint16) + sizeof(uint16)/*CRC校验*/

/*-------------------------------------------使用上位机矫正三相电压----------------------------------------------------------------*/
#define AdjusrPhaseVoltAddr							(UpgrChargeNumAddr+UpgrChargeNumLen)
#define AdjusrPhaseVoltLen								(((sizeof(bool)+sizeof(float)+sizeof(float))*3)+sizeof(uint16))
/*---------------------------------------------------------------------------------------------------------------------------------*/

/*
** 分控板(通讯板升级bin文件参数)
*/
#define UpgrBinFilePara_CommAddr					(AdjusrPhaseVoltAddr + AdjusrPhaseVoltLen)
/*
** 电池包
*/
#define UpgrBinFilePara_BatAddr(loc)							((UpgrBinFilePara_CommAddr + UpgrBinFilePara_Len) + loc*UpgrBinFilePara_Len)
/*
** bin文件相关参数长度--20210220:修改bin文件存储字节数-->由结构体: (sizeof(UpgrFilePara) + sizeof(uint16)) -- 65Byte 转换  (sizeof(ModifyEepromLogic) + sizeof(uint16)) -- 16 Byte
*/
#define UpgrBinFilePara_Len							(sizeof(ModifyEepromLogic) + sizeof(uint16))/*CRC16校验*/

/*
** 应用层地址分配说明:
---------------------------------------------------------------------------------------------------------------------------------------------
** 采用映射方案:
**		修改参数:	UpgrBinFilePara_Len * (UpgrBatMemoryNum(电池固件包) + 1) (通讯板) + 控制板(0xB4) + 
** 											regLog(3) + batSize(4) + 216(域名,设备ID,Wifi名称) 
**											+ UpgrBatMemoryNum * 4 * 2 + chargeSize(4)(预留)
											+ AdjusrPhaseVoltLen--(29Byte) = 1508 = 0x05E4
		-- 20210324--》新增:偏移地址--》+ sizeof(uint16) = 150A = 0x05E6	
*/
#define App_InitBaseAddr										0x05E6

#define TSave_Addr												App_InitBaseAddr
#define TSave_Len													(sizeof(Tsave)+ sizeof(uint16))/*CRC16校验*/
#define SocLimit_Addr												TSave_Addr + TSave_Len
#define SocLimit_Len												(sizeof(uint8) + sizeof(uint16))
#define ChgTimeLimit_Addr										SocLimit_Addr + SocLimit_Len
#define ChgTimeLimit_Len										(sizeof(uint16) + sizeof(uint16))
#define ChgOverTempLimit_Addr								ChgTimeLimit_Addr + ChgTimeLimit_Len
#define ChgOverTempLimit_Len									(sizeof(uint16) + sizeof(uint16))
#define ChgDoorOTLimit_Addr									ChgOverTempLimit_Addr + ChgOverTempLimit_Len
#define ChgDoorOTLimit_Len									(sizeof(uint16) + sizeof(uint16))
/*
** 充电器过流阈值
*/
#define ChargerOCLimit_Addr									(ChgDoorOTLimit_Addr + ChgDoorOTLimit_Len)	
#define ChargerOCLimit_Len										(sizeof(uint8)+sizeof(16))
/*----------------充电柜交互协议V1.05--------------*/
#define BatOTempLimit_Addr									(ChargerOCLimit_Addr + ChargerOCLimit_Len)
#define BatOTempLimit_Len										(sizeof(int8) + sizeof(uint16))
#define NanduLowPLimit_Addr									(BatOTempLimit_Addr + BatOTempLimit_Len)
#define NanduLowPLimit_Len									(sizeof(int8) + sizeof(uint16))
#define BatLowPLimit_Addr										(NanduLowPLimit_Addr + NanduLowPLimit_Len)
#define BatLowPLimit_Len										(sizeof(int8) + sizeof(uint16))
/*-------------------------------------------------*/


/****声明:目前关于升级地址内容仅存储至Backup*********************/
typedef struct{
	uint8 key[6];
}UpgradeKey;

/*
** 系统参数之本地保存参数设置之结构体声明
*/
#pragma pack(1)
typedef struct{
	uint16 ctr;/*控制板运行版本号*/
	uint16 comFile;/*外部Flash中保存的通讯板文件软件版本号*/
	uint16 comApp[SysCtr_AllDoorNum];/*通讯板运行App版本号--分控版本号,电池版本*/
	uint16 upgrFailNumAcc[SysCtr_AllDoorNum];/*统计升级失败累计次数--用于记录分控针对固件升级失败的次数统计*/
}NewSoftVer;

typedef struct{
	uint16 base;/*系统温度基准值*/
	uint16 diff;/*系统回差温度*/
}Tsave;

/*
** 域名
*/
typedef struct{
	uint8 local[64];
	uint8 remote[64];
}SysDomain;

typedef struct{
	bool registerFlag;/*服务端注册是否成功*/
}Serve;

typedef struct{
	uint8 name[32];
	uint8 pw[32];
}WifiSet;

typedef struct{
	int16 bat[UpgrBatMemoryNum];/*平台限制次数--电池*/
	uint16 exceBat[UpgrBatMemoryNum];/*实际升级次数--电池*/
}FireUpgrNumLimit;/*固件升级次数*/

typedef struct{
	bool bat[UpgrBatMemoryNum];/*true:清除对应固件--之电池*/
}ClearFire;/*清除对应固件标志*/

typedef struct{
	bool bat[UpgrBatMemoryNum];/*true:控制分控进入测试模式--之电池*/
	uint16 batUpgrNum[UpgrBatMemoryNum];/*测试模式下电池固件升级次数--20210224修改为:成功次数*/
	uint16 batUpgrNumFail[UpgrBatMemoryNum];/*测试模式下电池固件升级次数--20210224修改为:失败次数*/
}FireTestModel;/*固件测试模式*/

typedef struct{
	NewSoftVer newSoftVer;/*软件版本号控制*/
	uint8 socLimit;/*soc阈值*/
	uint16 chgTimeOut;/*充电时间超时*/
	uint16 chgOverTemp;/*充电器过温*/
	uint16 chgDoorOTemp;/*充电仓过温*/
	Tsave tsave;/*系统温度参数--基准温度/回差*/
	uint16 batFireSize;/*电池包固件个数*/
	SysDomain sysDomain;/*系统域名:本地/远程*/
	DeviceId  deviceId;/*设备ID*/
	Serve serve;/*服务端参数*/
	WifiSet wifiSet;/*Wifi 参数设置*/
	FireUpgrNumLimit fireUpgrNumLimit;/*固件升级次数*/
	ClearFire clearFire;/*清除固件*/
	FireTestModel fireTestModel;/*固件进入测试模式*/
	uint8 chargerOCLimit;/*充电器过流阈值*/
	/*----------------充电柜交互协议V1.05--------------*/
	int8 batOTempLimit;/*电池过温阈值*/
	int8 nanduLowPLimit;/*南都低温保护阈值*/
	int8 batLowPLimit;/*电池低温保护*/
	/*-------------------------------------------------*/
}SysParam;
#pragma pack()


/*
** 使用指针提供外部文件访问变量
*/
SysParam* get_SysParaPtr(void);
/*
** get Ctr文件名
*/
void get_CtrFileName(uint8* rx,uint8* rxlen);
/*
** clear 通讯板/电池包软件版本号
*/
void clear_AppVer(void);
/*
** get 通讯板/电池包运行App域版本号
*/
uint16 getComRunAppVer(uint8 addr);
/*
** set 通讯板/电池包运行App域版本号
*/
void setComRunAppVer(uint16 ver,uint8 addr);
/*
** 统计升级次数已达累计不允许再次进入升级模式
*/
void set_statisAccUpgrNum(uint16 numFlag,uint8 addr);
/*
** get Ctr SoftVer
*/
uint16 get_CtrSoftVer(void);
/*
** 更新控制板软件/通讯板软件版本号
*/
void updateSoftVerPara(void);
/*
** set 升级秘钥参数
*/
void set_UpgrCtrKey(void);
/*
** get Ctr软件版本号
*/
uint32 get_CtrVer(void);
/*
** default 电池包信息
*/
DetailedInfo default_DetailedInfo(uint16 loc);
/*
** get电池包存储队列的大小(个数)
*/
uint16 get_SizeBatFile(void);
/*
** get 电池固件包个数
** 		策略:仅用于查询电池包个数
*/
uint16 get_batFireSize(void);
/*
** set电池包存储队列的大小(个数)
*/
void set_SizeBatFile(uint16 szie);
/*
** 初始化电池包(硬件版本号,ID)参数
*/
void init_BatPara(void);
/*
** set升级bin文件参数
** 策略:根据电池包详细信息(硬件版本/ID)存储指定地址位置
*/
void set_UpgrBinFilePara(UpgrFilePara upgr);
/*
** set Assign电池包bin文件参数
*/
void set_AssignUpgrBinFilePara(UpgrFilePara upgr,int16 loc);
/*
** get升级bin文件参数--电池包
** @para:
**		loc:指定位置--(0,max(get_SizeBatFile()))
*/
UpgrFilePara get_UpgrBatFilePara(uint16 loc);
/*
** get升级bin文件参数--通讯板
*/
UpgrFilePara get_UpgrComFilePara(void);
/*
** get电池包位置信息
** @return:
** 			-1:无电池包信息--是否添加
**			-2:参数丢失
** 策略:
**		1.用于升级判断是否存在电池包对应型号区域
**		2.以位置信息获取Eeprom地址信息
*/
int16 get_BatLocation(UpgrFilePara upgr);
/*
** set 系统温度值(基准+回差)
*/
void set_Tsave(Tsave ts);
/*
** get 系统温度值(基准+回差)
*/
Tsave get_Tsave(void);
/*
** set SOC阈值
*/
void set_SocLimit(uint8 soc);
/*
** get SOC阈值
*/
uint8 get_SocLimit(void);
/*
** set 充电时间阈值
*/
void set_ChgTimeLimit(uint16 time);
/*
** get 充电时间阈值
*/
uint16 get_ChgTimeLimit(void);
/*
** set 充电器过温阈值
*/
void set_ChgOverTempLimit(uint16 temp);
/*
** get 充电器过温阈值
*/
uint16 get_ChgOverTempLimit(void);
/*
** set 充电器仓过温阈值
*/
void set_ChgDoorOTLimit(uint16 temp);
/*
** get 充电器仓过温阈值
*/
uint16 get_ChgDoorOTLimit(void);
/*-----------------------------------充电器过流阈值V1.5-----------------------------------------------------*/
/*
** set Charger OC Limit
*/
void set_ChargerOCLimit(uint8 limit);
/*
** get Charger OC Limit
*/
uint8 get_ChargerOCLimit(void);
/*-------------------------------------------------------------------------------------------------------*/
/*
** set 域名
*/
void set_SysDomain(SysDomain s);
/*
** get 域名
*/
SysDomain get_SysDomain(void);
/*
** set Sys 设备ID
*/
void set_SysDeviceId(DeviceId id);
/*
** get Sys 设备ID
*/
DeviceId get_SysDeviceId(void);
/*
** get 服务端参数
*/
Serve get_ServePara(void);
/*
** set服务端参数
*/
void setServePara(Serve s);
/*
** get WIFI Name PW
*/
WifiSet get_WifiNamePwSysSet(void);
/*
** set Wifi Name  
*/
void set_WifiNameSysSet(uint8* space);
/*
** set Wifi Pw 
*/
void set_WifiPwSysSet(uint8* space);
/**********************美团新需求查阅邮件2020/12/22--管控固件等操作***********************************************************/
/*
** set Fire Upgr Num
*/
void set_FireUpgrNum(uint8 type,int16 num,uint16 loc);
/*
** get Fire Upgr Num
*/
int16 get_FireUpgrNum(uint8 type,uint16 loc);
/*
** set Fire Reality Run Num
*/
void set_FireRealityRunNum(uint8 type,uint16 num,uint16 loc);
/*
** get Fire Reality Run Num
*/
uint16 get_FireRealityRunNum(uint8 type,uint16 loc);
/*
** set Clear Fire
*/
void set_ClearFire(uint8 type,bool flag,uint16 loc);
/*
** get Clear FIre
*/
bool get_ClearFire(uint8 type,uint16 loc);
/*
** set Fire Enter Test model
*/
void set_FireEnterTestModel(uint8 type,bool flag,uint16 loc);
/*
** get Fire Enter Test model
*/
bool get_FireEnterTestModel(uint8 type,uint16 loc);
/*
** set Test Model Fire Upgr Num
*/
void set_TestModelFireUpgrNum(uint8 type,uint16 num,uint16 loc);
/*
** get Test Model Fire Upgr Num
*/
uint16 get_TestModelFireUpgrNum(uint8 type,uint16 loc);
/*
** set Test Model Fire Upgr Num -- 失败次数
*/
void set_TestModelFireUpgrNumFail(uint8 type,uint16 num,uint16 loc);
/*
** get Test Model Fire Upgr Num -- 失败次数
*/
uint16 get_TestModelFireUpgrNumFail(uint8 type,uint16 loc);
/*****************************************************************************************************************************/
/*----------------------------美团充电柜-第三方交互方案--后端接口(外部输出)-1.05---------------------------------------------*/
/*
** 设置电池过温阈值
*/
void set_BatOTempLimit(int8 temp);
/*
** 获取电池过温阈值
*/
int8 get_BatOTempLimit(void);
/*
** 设置南都电池低温保护
*/
void set_NanduLowPLimit(int8 temp);
/*
** 获取南都电池低温保护
*/
int8 get_NanduLowPLimit(void);
/*
** 设置电池低温保护
*/
void set_BatLowPLimit(int8 temp);
/*
** 获取电池低温保护
*/
int8 get_BatLowPLimit(void);
/*---------------------------------------------------------------------------------------------------------------------------*/
/*
** 系统参数初始化
*/
void init_SysPara(void);
#endif

