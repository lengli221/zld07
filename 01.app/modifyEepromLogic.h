#ifndef __MODIFY_EEPROMLOGIC_H_
#define __MODIFY_EEPROMLOGIC_H_

/*
** 修改EEPROM存储读取逻辑
*/
#pragma pack(1)
typedef struct{
	uint8 board;/*固件类型信息:0x01--分控固件,0x02--电池固件,0x03--充电器固件*/
	uint8 label;/*标号--位置信息*/
	uint16 softver;/*软件版本号*/
	uint32 flashAddr;/*flash存储地址*/
	BinFileItemPara binFileItemPara;
}ModifyEepromLogic;/*bin文件参数*/
#pragma pack()

/*
** 20210202-->转换结构体存储
*/
ModifyEepromLogic set_TypedefModify(UpgrFilePara* upgr);
/*
** 20210202-->转换结构体存储 -- 指定初始化参数
*/
ModifyEepromLogic set_TypedefModify_Assign(UpgrFilePara* upgr,uint8 lo);
/*
** 20210220 --> 结构体转换
*/
UpgrFilePara conv_TypedefModify(ModifyEepromLogic modifyEL);

#endif

