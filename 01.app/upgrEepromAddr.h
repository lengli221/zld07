#ifndef __UPGREEPROMADDR_H_
#define __UPGREEPROMADDR_H_

typedef struct{
	uint32 ver;/*版本号*/
	uint32 fileLen;/*文件名长度*/
	uint32 fileItem;/*文件内容*/
}UpgrEepromAddr;


/*
** 获取升级参数存储EEPROM地址
*/
uint32 get_UpgrEepromAddr(UpgrFilePara upgr);
/*
** 获取电池升级(硬件版本号,电池ID)存储EEPROM地址
** 控制策略:
** 				用于电池初始化参数获取EEPROM地址
*/
uint32 get_UpgrBatEepromAddr(uint8 loc,UpgrFilePara upgr);
#endif

