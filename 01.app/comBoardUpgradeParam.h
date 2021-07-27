#ifndef __COMBOARDUPGRADEPARAM_H_
#define __COMBOARDUPGRADEPARAM_H_

#pragma pack(1)
typedef union{
	uint8 flag;
	struct{
		uint8 comBoard:1;/*通讯板*/
		uint8 batBms:1;/*电池BMS*/
		uint8 res:6;
	}bits;
}BinFileType;

typedef struct{
	uint8 upgradeWay;/*升级方式*/
	BinFileType binFileType;/*bin文件类型*/
	uint16 softVer;/*软件版本号*/
	DetailedInfo detailedInfo;/*电池详细信息*/
	uint32 flashAddr;/*flash地址*/
	BinFileItemPara binFileItemPara;/*bin文件内容参数,字节长度,数据校验*/
}ComBup;
#pragma pack()

/*
** 通讯板升级参数初始化
*/
void ComBoardUpgrPara_Init(void);
/*
** 清除当前通讯升级任务参数
*/
void Clear_CurComBupPara(void);
/*
** get 当前升级任务相关参数
*/
ComBup getCurComBup(void);
/*
** 更新通讯小板队列参数
*/
void UpdataComBoardQueuePara(ComBup com);
/*
** 通讯板任务更新+任务相关参数更新
*/
void ComBoard_TaskSwitch(void);
#endif

