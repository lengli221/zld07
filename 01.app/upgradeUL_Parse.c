#include "includes.h"

static int32 upgradeULParse_TaskStk[1024] = {0};

extern UpperLayerPara upperLayerPara;

/*
** 远程升级之系统参数
*/
UpgradeSysPara upgradeSysPara;
UpgradeReply upgradeReply;
/*定义升级接口--默认485接口*/
uint8 upgrInterface_Label = BSP_ComUpperLayer;

/*
** 远程升级之系统参数初始化
*/
static void UpgradePro_ParaInit(void){
	memset((uint8*)&upgradeSysPara.processCtr.step,0x00,sizeof(UpgradeSysPara));
	/*
	** 更新流程参数
	*/
	upgradeSysPara.processCtr.step = ChkFrameStartFlag;
	upgradeSysPara.processCtr.binFilePara.len = 0;
	upgradeSysPara.processCtr.binFilePara.crc16 = 0;
	/*
	** 远程升级
	** 1.固件地址之参数初始化
	** 2.初始化Flash初始化内部地址
	*/
	upgradeSysPara.firmware.addr = Firmware_HandwareAddr;
	upgradeSysPara.firmware.interFlashAddr = getUpgrFlashAddr(getUpgrFilePara());
	/*
	** 更新回复参数数据
	*/
	upgradeReply.ack = UpgradeFrame_Ack;
	/*更新上层协议*/
	upgrInterface_Label = get_Interface().label;
}

/*
** 校验数据帧数据项的大小,防止内存数据复制导致硬中断
*/
static bool ChkFrameDataItemLenIsOk(uint16 len){
	uint16 baseLen = 0;
	bool ret = false;

	switch(upgradeSysPara.processCtr.step){
		case ChkFrameStartFlag:
			baseLen = sizeof(FileNameItem);	
			break;
		case ChkFrameIngFlag:
			baseLen = FrameDataItem_MaxLen;
			break;
	}

	if(len <= baseLen){
		ret = true;
	}

	return ret;
}

/*
** 校验地址合法性
*/
static bool ChkFrameAddrIsOk(uint8 addr){
	bool ret = true;

	if(addr != upgradeSysPara.firmware.addr){
		ret = false;
	}

	return ret;
}

/*
** 校验帧ID合法性
*/
static bool ChkFrameIdIsOk(uint16 frameId){
	bool ret = true;

	switch(upgradeSysPara.processCtr.step){
		case ChkFrameStartFlag:
			if(frameId != 0){
				ret = false;
			}
			break;
		case ChkFrameIngFlag:
			if(((uint8)(frameId - upgradeSysPara.processCtr.frameLastId )) != 1){
				ret = false;
			}
			break;
	}
	
	return ret;
}

/*
** 更新通讯板升级队列
*/
void update_ComUpgrQueue(UpgrFilePara upgr){
	/*
	** 通讯板队列参数
	*/
	ComBup com = {0};

	com.upgradeWay = upgr.upgrFileType.cmdType;
	com.binFileType.flag = upgr.upgrFileType.board;
	com.detailedInfo = upgr.upgrFileType.detailedInfo;/*电池详细信息*/
	com.softVer = upgr.upgrFileType.softVer;
	com.binFileItemPara.len = upgr.upgrFileType.binFileItemPara.len;
	com.binFileItemPara.crc16 = upgr.upgrFileType.binFileItemPara.crc16;
	com.flashAddr = upgr.upgrFileType.flashAddr;
	UpdataComBoardQueuePara(com);	
}

/*
** 远程升级之回复帧处理
*/
static void Upgrade_ReplyFrame(void){
	uint8 tx[sizeof(UpgradeProFrame)+sizeof(UpgradeReply)] = {0};
	uint32 len = 0;
	uint16 crc16=0;
	UpgradeProFrame upgradeProFrame; 

	/*20210129--发生一级告警立即退出*/
	if(oneWaring_ForbidUpgrDownFile() == true){
		upgradeReply.flag = 0xFE;
	}
	
	upgradeProFrame.flag = Frame_Head;
	upgradeProFrame.addr = upgradeSysPara.firmware.addr;
	upgradeProFrame.frameId = upgradeSysPara.processCtr.frameId;
	upgradeProFrame.dataFrameLen = sizeof(UpgradeReply);
	memcpy((uint8*)&tx[len],(uint8*)&upgradeProFrame.flag,sizeof(UpgradeProFrame));
	len += (sizeof(UpgradeProFrame)-sizeof(uint16));

	memcpy((uint8*)&tx[len],(uint8*)&upgradeReply.ack,sizeof(UpgradeReply));
	len += sizeof(UpgradeReply);

	crc16 = CRC16((uint8*)&tx[0],(uint32)len);

	memcpy((uint8*)&tx[len],(uint8*)&crc16,sizeof(uint16));
	len += sizeof(uint16);
		
	BSP_SmSend(BSP_ComType_USART, upgrInterface_Label/*BSP_ComUpperLayer*/, (uint8 *)&tx[0], (uint16 *)&len);
}

/*
** 远程升级之校验数据帧连续帧不合法性回复
*/
static void ChkFrameDataIsIllegal(uint16 frameId){
	upgradeReply.flag = ((uint8)(frameId - upgradeSysPara.processCtr.frameLastId)) == 0?FrameIdRepeat:FrameIdSpan;
	Upgrade_ReplyFrame();
}

/*
** 远程升级之控制函数
*/
static void Upgrade_StartEndCancelFun(void*rx,uint16 dataItemLen){
	FileNameItem fileNameItem = {0};
	uint8 flag = 0;
	uint16 sectorNum = 0;
	uint16 i = 0;
	uint8 tempData[FrameDataItem_MaxLen] = {0};
	uint16 readFlashNum = 0;
	uint32 flashInitAddr = Upgrade_FlashAddrInit;
	/*
	** 升级基础地址
	*/
	uint32 flashBaseAddr = Upgrade_FlashAddrInit;
	uint16 uchInit = 0xFFFF;
	uint8 indexInit = 0;
	uint16 binFileCrc16 = 0;
	/*
	** 控制板下载文件参数变量
	*/
	UpgrFilePara upgr = getUpgrFilePara();
	/*
	** 更新flash初始/基础地址地址
	*/
	flashInitAddr = getUpgrFlashAddr(upgr);
	flashBaseAddr = flashInitAddr;
	
	if(dataItemLen <= sizeof(FileNameItem)){/*流程控制帧符合要求,copy数据*/
		memcpy((uint8*)&fileNameItem.flag,(uint8*)rx,dataItemLen);
	}

	switch(fileNameItem.flag){
		case ChkFrameStartFlag:
			flag = ChkFrameStartFlag;
			sectorNum = (uint16)(fileNameItem.word/sFLASH_SPI_SECTORSIZE);
			sectorNum += (uint16)(fileNameItem.word%sFLASH_SPI_SECTORSIZE) !=0?1:0;
			for(i=0;i<sectorNum;i++){
				sFLASH_EraseSector((uint32)(flashBaseAddr + i*sFLASH_SPI_SECTORSIZE));
			}
			/*
			** 接收数据帧异常之后,重新开始数据帧接收
			*/
			UpgradePro_ParaInit();
			/*
			** 接收bin文件数据帧
			*/
			upgradeSysPara.processCtr.step = ChkFrameIngFlag;
			upgradeSysPara.processCtr.binFilePara.crc16 = fileNameItem.fileCrc16;
			break;
		case ChkFrameStopFlag:
			flag = ChkFrameStopFlag;
			/*
			** 计算bin文件校验
			*/
			readFlashNum = (uint16)(upgradeSysPara.processCtr.binFilePara.len/FrameDataItem_MaxLen);
			for(i=0;i<readFlashNum;i++){
				sFLASH_ReadBuffer((uint8 *)&tempData[0], flashInitAddr, FrameDataItem_MaxLen);
				binFileCrc16 = CRC16_Upgrade((uint8 *)&tempData[0], FrameDataItem_MaxLen, uchInit, (uint8 *)&indexInit);
				uchInit = binFileCrc16;
				memset((uint8*)&tempData[0],0x00,FrameDataItem_MaxLen);
				flashInitAddr += FrameDataItem_MaxLen;
			}
			
			if((upgradeSysPara.processCtr.binFilePara.len - readFlashNum*FrameDataItem_MaxLen) != 0){
				sFLASH_ReadBuffer((uint8 *)&tempData[0], flashInitAddr, 
					upgradeSysPara.processCtr.binFilePara.len - readFlashNum*FrameDataItem_MaxLen);
				binFileCrc16 = CRC16_Upgrade((uint8 *)&tempData[0], 
					upgradeSysPara.processCtr.binFilePara.len - readFlashNum*FrameDataItem_MaxLen, uchInit, (uint8 *)&indexInit);
			}

			/*
			** 校验合法
			*/
			if(binFileCrc16 == upgradeSysPara.processCtr.binFilePara.crc16 && binFileCrc16 == fileNameItem.fileCrc16
				&& upgradeSysPara.processCtr.binFilePara.len == fileNameItem.word){
				/*文件名格式异常(包含非bin文件)*/
				if(FileNameVerOpt((uint8 *)&fileNameItem.fileName[0], dataItemLen-sizeof(uint8)-sizeof(uint32)-sizeof(uint16))){
					/*
					** set升级bin文件参数
					** 策略:根据电池包详细信息(硬件版本/ID)存储指定地址位置
					** 		参数更新:
					**				1.文件名长度
					**				2.文件名
					**				3.文件流长度
					** 				4.文件校验内容
					**				5.更新flash存储地址
					*/
					upgr = getUpgrFilePara();
					upgr.upgrFileType.fileNameLen = dataItemLen-sizeof(uint8)-sizeof(uint32)-sizeof(uint16);
					memcpy((uint8*)&upgr.upgrFileType.fileName[0],(uint8*)&fileNameItem.fileName[0],
						 dataItemLen-sizeof(uint8)-sizeof(uint32)-sizeof(uint16));
					upgr.upgrFileType.binFileItemPara.len = upgradeSysPara.processCtr.binFilePara.len;
					upgr.upgrFileType.binFileItemPara.crc16 = upgradeSysPara.processCtr.binFilePara.crc16;
					upgr.upgrFileType.flashAddr = flashBaseAddr;
					set_UpgrBinFilePara(upgr);
					/*
					** 更新控制板软件/通讯板软件版本号
					*/
					updateSoftVerPara();

					if(oneWaring_ForbidUpgrDownFile() == false){
						/*
						** 更新通讯板升级队列
						*/
						update_ComUpgrQueue(upgr);
					}
					
					/*
					** 控制板之参数下载完成之参数已保存
					*/
					memset((uint8*)&upgr.upgrFileType.cmdType,0x00,sizeof(UpgrFilePara));
					setUpgrFilePara(upgr);
					/*
					** set 恢复上层协议之APP应用层协议标志
					*/
					set_CtrBoardTaskRecoverFlag();
				}else{
					flag = FileNameFormatIilegal;
				}
			}else{/*文件内容校验错误*/
				flag = FileItemChkErr;
			}
			break;
	}

	/*
	** 更新回复标志
	*/
	upgradeReply.flag = flag;
}

/*
** 远程升级之数据项内容函数
*/
static void Upgrade_DataItemFun(void*rx,uint16 dataItemLen){
	uint8 flag = ChkFrameIngFlag;

	/*
	** Error:#852:expression must be a pointer to complete object type
	*/
	/*
	** sFLASH_WriteData((uint8*)&rx[0],upgradeSysPara.firmware.interFlashAddr, dataItemLen);
	*/
	sFLASH_WriteData((uint8*)rx,upgradeSysPara.firmware.interFlashAddr, dataItemLen);
	/*
	** 更新Flash写地址 更新bin写入字节数
	*/
	upgradeSysPara.firmware.interFlashAddr += dataItemLen;
	upgradeSysPara.processCtr.binFilePara.len += dataItemLen;
	/*
	** 更新帧ID
	*/
	upgradeSysPara.processCtr.frameLastId = upgradeSysPara.processCtr.frameId;
	/*
	** 更新回复标志
	*/
	upgradeReply.flag = flag;	
}

/*
** 远程升级之功能执行函数表格
*/
static const UpgradePro upgradeProTabel[] = {
	{ChkFrameStartFlag,Upgrade_StartEndCancelFun},
	{ChkFrameIngFlag,Upgrade_DataItemFun}
};
uint8 upgradeProTabelNum = sizeof(upgradeProTabel)/sizeof(UpgradePro);

/*
** 远程升级之功能执行函数
*/
static void UpgradeProTabelExecuteFun(uint8*rx,uint16 dataItemLen){
	uint8 i=0;
	
	for(i=0;i<upgradeProTabelNum;i++){
		if(upgradeSysPara.processCtr.step == upgradeProTabel[i].step){
			upgradeProTabel[i].handle(rx,dataItemLen);
			Upgrade_ReplyFrame();
			break;
		}
	}
}

/*
** 解析升级协议帧
*/
static void ParseUpgradeProFrame(uint8*rxbuf,uint16*rxlen){
	uint8* ptrbuf = rxbuf;
	uint16 updataLen = *rxlen;
	uint16 saveDataLen = *rxlen;
	uint16 dataItemLen = 0;
	uint16 crc16 = 0;
	uint16 frameId = 0;
	
	for(;updataLen>=(uint16)(sizeof(UpgradeProFrame));){
		/*说明:常数参数优化项,数据项长度*/
		memcpy((uint8*)&dataItemLen,(uint8*)&ptrbuf[4],sizeof(uint16));
		if(ptrbuf[0] == Frame_Head && ChkFrameDataItemLenIsOk(dataItemLen)){
			if(updataLen >= dataItemLen+sizeof(UpgradeProFrame)){
				memcpy((uint8*)&crc16,(uint8*)&ptrbuf[sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen],sizeof(uint16));
				if(crc16 == CRC16((uint8 *)&ptrbuf[0], sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen)){
					/*帧ID*/
					memcpy((uint8*)&frameId,(uint8*)&ptrbuf[2],sizeof(uint8)+sizeof(uint8));
					/*兼容"结束数据接收帧"*/
					if(frameId == 0){
						upgradeSysPara.processCtr.step = ChkFrameStartFlag;
					}	
					/*更新当前帧ID*/
					upgradeSysPara.processCtr.frameId = frameId;
					if(ChkFrameAddrIsOk(ptrbuf[sizeof(uint8)]) && ChkFrameIdIsOk(frameId)){
						/*帧执行函数*/
						UpgradeProTabelExecuteFun((uint8*)&ptrbuf[6],dataItemLen);
						/*
						** 清超时定时器
						*/
						TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itickout, 0);
					}else{
						/*地址合法,帧异常*/
						if(ChkFrameAddrIsOk(ptrbuf[sizeof(uint8)])){
							ChkFrameDataIsIllegal(frameId);
						}
					}
					ptrbuf += (dataItemLen+sizeof(UpgradeProFrame));
					updataLen -= (dataItemLen+sizeof(UpgradeProFrame));
					dataItemLen = 0;
					/*
					** 清除下载文件定时
					*/
					//UpgradeCom_ClearComTickOut();
				}else{
					ptrbuf++;
					updataLen--;
					dataItemLen = 0;
				}
			}else{
				break;
			}
		}else{
			ptrbuf++;
			updataLen--;
			dataItemLen = 0;
		}
	}
	memcpy((uint8*)&rxbuf[0],(uint8*)&ptrbuf[0],updataLen);
	*rxlen = updataLen;
	memset((uint8*)rxbuf+updataLen,0x00,saveDataLen-updataLen);
}

/*
** 远程升级之下载bin文件之灯闪烁频率--500ms
** param:flag:true--开启 false--关闭
*/
void UpgradeUL_CtrLedLogic(bool flag){
	static uint32 cnt = 0;
	static bool ledFlickFlag = false;
	
	if(flag == true){/*开启闪烁*/
		if(TickOut((uint32 *)&cnt, 500) == true){
			TickOut((uint32 *)&cnt, 0);
			ledFlickFlag = ledFlickFlag == false?true:false;
			firewareCtr(setFirewareUpgre_Label, ledFlickFlag);
		}			
	}else{/*关闭闪烁*/
		firewareCtr(setFirewareUpgre_Label, false);
	}
}

/*
** 提供系统软件之远程升级接口任务之上层协议
*/
void SM_UpgradeULParse_Task(void* p_arg){
	/*
	*数据帧,粘帧,断帧,异常帧处理的buf裕量
	*/
	uint8 ucRxBuf[sizeof(uint16)*(FrameDataItem_MaxLen+sizeof(UpgradeProFrame))];/*明确与任务stk之间的关系:参数设置512导致系统复位*/
	uint8 *rxptr;
	uint16 ucLen = 0;
	uint16 ucRecvLen = 0;	

	/*
	** 远程升级之系统参数初始化
	*/
	UpgradePro_ParaInit();

	Sleep(6000);
	
	for(;;){
		ucRecvLen = 0;
		BSP_SmRecv(BSP_ComType_USART,upgrInterface_Label/*BSP_ComUpperLayer*/,ucRxBuf + ucLen,&ucRecvLen);
		ucLen += ucRecvLen;
		if(ucLen >( sizeof(ucRxBuf))){
			ucLen = 0;
		}
		
		if(ucRecvLen == 0 && ucLen != 0){
			rxptr = ucRxBuf;
			ucRecvLen = ucLen;
			ParseUpgradeProFrame((uint8 *)rxptr, (uint16 *)&ucRecvLen);
			ucLen = ucRecvLen;
		}

		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
		
		Sleep(10);
	}
}

/*
** 提供系统软件之远程升级接口函数之上层协议
*/
void UgradeULParse_Init(void){
	Thread_create(SM_UpgradeULParse_Task, 0, 
		(int32 *)&upgradeULParse_TaskStk[1024-1], (int8)UgradeULParseTask_Prio);
}

