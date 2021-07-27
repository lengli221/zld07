#ifndef __UPGRADEUL_H_
#define __UPGRADEUL_H_

#include "ctrGetUpgrFilePara.h"

/*
** 文件名最大数据长度
*/
#define FileName_MaxLen										30/*根据美团电池包升级文件变更文件名最大长度*//*128*/
/*
** 远程升级之数据帧最大长度
*/
#define FrameDataItem_MaxLen								1024	
/*
** 远程升级之数据帧帧头
*/
#define Frame_Head												(uint8)0x68
/*
** 远程升级之固件地址
*/
#define Firmware_HandwareAddr								(uint8)0xCC
/*
** 应答标志Ack
*/
#define UpgradeFrame_Ack										(uint8)0x06				

/*
** 远程升级帧应答标志FLAG
*/
enum{
	ChkFrameStartFlag = 0x02,//兼容"结束数据传输"流程控制--0x03
	ChkFrameIngFlag = 0x01,
	ChkFrameStopFlag = 0x03,

	FileNameFormatIilegal = 0xF5,//文件名格式不合法(包含非bin文件)
	WriteAppErr = 0xF6,//写APP区异常
	EraseAppErr = 0xF7,//擦除APP区异常
	EraseBackupErr = 0xF8,//擦除备份区异常
	FrameIdSpan = 0xF9,//数据帧跨越
	FrameIdRepeat = 0xFA,//数据帧重复
	WriteFlashErr = 0xFB,//写备份区异常
	FileItemChkErr = 0xFC,//文件内容校验错误
	FrameTimout = 0xFE,//帧超时,取消升级模式
};

/*
*1.字节对齐方案处理
*#pragma是否限制条件
*2.结构体"字段"的起始地址所占内存空间大小
*/
#pragma pack(1)
/*
** 数据帧结构
*/
typedef struct{
	uint8 flag;
	uint8 addr;
	uint16 frameId;
	uint16 dataFrameLen;
	uint16 crc16;
}UpgradeProFrame;

/*
** 文件名数据内容
*/
typedef struct{
	uint8 flag;
	uint32 word;
	uint16 fileCrc16;
	uint8 fileName[FileName_MaxLen];
}FileNameItem;

/*
** 帧回复数据项内容
*/
typedef struct{
	uint8 ack;
	uint8 flag;
}UpgradeReply;
#pragma pack()

/*
** 远程升级之系统参数
** 1.进程控制
** 2.固件地址
** 3.通讯小板升级Flag
*/
typedef struct{
	uint32 itick;/*定时器*/
	bool comBinFileRecOk;/*通讯小板bin文件接收完成*/
}ComBinParam;

typedef struct{
	uint8 step;/*进程控制*/
	uint16 frameId;/*当前帧ID*/
	uint16 frameLastId;/*已执行上一帧帧ID*/
	BinFileItemPara binFilePara;/*bin文件参数*/
}ProcessCtr;

typedef struct{
	uint8 addr;
	uint32 interFlashAddr;/*记录Flash地址*/
}Firmware;;

typedef struct{
	ProcessCtr processCtr;
	Firmware firmware;
}UpgradeSysPara;

/*
** 远程升级之功能执行函数表格结构体
*/
typedef struct{
	uint8 step;
	void (*handle)(void*,uint16);
}UpgradePro;

/*
** 提供系统软件之远程升级接口函数之上层协议
*/
void UgradeULParse_Init(void);
/*
** 远程升级之下载bin文件之灯闪烁频率--500ms
** param:flag:true--开启 false--关闭
*/
void UpgradeUL_CtrLedLogic(bool flag);
#endif

