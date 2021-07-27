#ifndef __UPGRADELL_H_
#define __UPGRADELL_H_

/*
** 目标地址之广播升级
*/
#define DstAddr_BoardCast_Define							(uint8)0xBB

/*
** 广播升级最大次数
*/
#define BroadCast_UgpradeMaxCnt							(uint8)0x04/*应美团需求:重试3次定义为最终需要发送4次*/

/*
** 帧数最大数
*/
#define UpgradeLL_FrameMax									(uint8)0x05

/*
** 升级之功能#define声明
** 1.查询通讯小板boot loader--Fun ID:0xF0
** 2.升级文件版本号--Fun ID:0xF1--应用APP层处理
** 3.文件字节数+文件内容校验--Fun ID:0xF2
** 4.文件内容----Fun ID:0xF3
** 5.查询App软件版本号 -- Fun ID:0xF4(注明:控制板,通讯板App兼容)
*/
#define UpgradeLL_FunId_RunArea							(uint8)0xF0
#define UpgradeLL_FunId_Ver									(uint8)0xF1
#define UpgradeLL_FunId_FileByteCrc							(uint8)0xF2
#define UpgradeLL_FunId_FileItem							(uint8)0xF3
#define UpgradeLL_FunId_ChkAppRunVer					(uint8)0xF4

/*
** 进程控制参数限制
*/
enum{
	UpgradeLL_ProcChkRunArea = 0x00,/*查询运行区域*/
	UpgradeLL_Proc_FileByteCrc = 0x01,/*文件字节数+文件内容校验*/
	UpgradeLL_Proc_DataItemTra = 0x02,/*开始数据传输标志*/ 
	UpgradeLL_Proc_Finsh = 0x03,/*进程完成(备注:bin文件传输层完成)*/
	UpgradeLL_Proc_Over = 0xFF,/*通讯小板升级完成*/
};

/*
** 升级之参数结构体声明
** 1.进程控制
** 2.查询通讯小板运行区域
** 3.
*/
typedef struct{
	uint8 step;/*进程步骤*/
	DoorNumDefine onLine;/*在线标志*/
	DoorNumDefine areaFlag;/*置1运行在BootLoader 清0运行在App区域*/
	DoorNumDefine upgradePermit;/*置1允许升级 清0拒绝升级*///注明:暂时标志areaFlag/upgradePermit具有一致性 仅存在时间差200MS
	DoorNumDefine fileByteCrcRecFlag;/*置1表示文件字节数+文件内容校验接收回复帧*/
	DoorNumDefine binDataItemTransmit;/*置1表示开始数据传输*/
	DoorNumDefine binDataItemEndFlag;/*置1表示结束结束传输--20210226--用于设置安卓统计成功和失败次数*/
	DoorNumDefine fileItemRecFlag;/*置1--内容帧接收完成*/
}ProcCtr;

typedef struct{
	uint8 cnt;
}ProcCtrRep;

typedef struct{
	uint8 circCnt;/*循环计数*/
	uint32 itick;/*定时器*/
}RunArea;

typedef struct{
	uint8 circCnt;/*循环计数*/
	uint32 itick;/*定时器*/
	uint8 cmd;/*升级流程命令:0x01--开始传输数据 0x02--结束数据传输 0xFD:取消升级*/
}FileByteItemCrc;

typedef struct{
	uint16 frameLabel;/*帧标识*/
	uint8 dataItemLen;/*帧数据内容字节数*/
	uint8 data[sizeof(uint32)];/*文件内容*/
}FileItem;

typedef struct{
	uint8 cnt;/*帧重复计数*/
	uint32 itick;/*定时器*/
}FileItemCtr;

typedef struct{
	uint8 cnt;/*帧重复计数*/
	uint32 itick;/*定时器*/
}AppRunVer;

typedef struct{
	ProcCtr proCtr;
	RunArea runArea;
	FileByteItemCrc fileByteItemCrc;
	FileItem fileItem;
	FileItemCtr fileItemCtr;
	AppRunVer appRunVer;
}UpgradeLLParam;

typedef struct{
	ProcCtrRep proCtrRep;	
}UpgrLLPara;

/*
** 下层协议之系统模块处理列表结构体声明
*/
typedef struct{
	uint8 funId;/*功能函数ID*/
	void (*handle)(uint8,uint8*,uint8)/*1.数据长度 2.数据项 3.设备地址*/;
}UpgradeParseFunTable;

/*
** 升级之参数结构体变量初始化
*/
void UpgradeLLParam_Init(void);
/*
** 清除升级计数次数
*/
void clear_UpgrLLParamCnt(void);
/*
** 升级次数自加
*/
void add_UpgrCnt(void);
/*
** 升级之文件内容之数据帧完成发送标志位
*/
bool Upgrade_FileItemTraFinshFlag(void);
/*
** 升级之更新升级文件内容参数
*/
void Upgrade_UpdateFileItemPara(uint16 frameLabel);
/*
** 指定取消升级对应通讯升级操作
*/
void UpgradeLL_AssignCanelUpgrade(uint8 addr);
/*
** 提供系统软件之远程升级接口函数之下层协议之回复函数
*/
void UpgradeLLReply_Init(void);
/*
** 提供系统软件之远程升级接口函数之下层协议之解析函数
*/
void UpgradeLLParse_Init(void);

#endif

