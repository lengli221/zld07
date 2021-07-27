#ifndef __CTRGETUPGRFILEPARA_H_
#define __CTRGETUPGRFILEPARA_H_

#include "upgradeUL.h"

/*
** 文件类型
*/
#define ComBoradType										0x01/*通讯板*/
#define BatBoardType											0x02/*电池板*/

/*
** 控制板下载文件之参数结构体
*/
#pragma pack(1)
typedef struct{
	uint16 hardVer;/*硬件版本*/
	uint8 id[16];/*电池ID*/
}DetailedInfo;

typedef struct{
	uint8 cmdType;/*正常升级/强制升级*/
	uint8 board;/*升级类型:通讯板/电池包*/
	DetailedInfo detailedInfo;/*升级详细信息*/
	uint16 softVer;/*软件版本*/
	uint32 flashAddr;/*flash存储地址*/
	uint8 fileNameLen;/*文件名长度*/
	uint8 fileName[FileName_MaxLen];/*固件命名(美团初版24Byte):软件设计暂定30Byte,预留6Byte*/
	BinFileItemPara binFileItemPara;/*bin文件参数*/
}UpgrFileType;/*65 byte*/

typedef struct{
	UpgrFileType upgrFileType;
}UpgrFilePara;
#pragma pack()

/*
** 控制板下载文件之参数变量初始化
*/
void init_UpgrFilePara(void);
/*
** set控制板下载文件之参数变量 
*/
void setUpgrFilePara(UpgrFilePara u);
/*
** get控制板下载文件之参数变量 
*/
UpgrFilePara getUpgrFilePara(void);

#endif

