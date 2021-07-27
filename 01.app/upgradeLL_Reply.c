#include "includes.h"

static int32 upgradeLLReply_TaskStk[512] = {0};

/*
** 使用指针调用其他文件变量
*/
UpperLayerPara* ulp_ULLR = null; 

/*
** 升级之参数结构体变量
*/
UpgradeLLParam upgradeLLParam;
/*
** 升级失败参数控制结构体变量
*/
UpgrLLPara upgrLLPara;

/*
** 升级之参数结构体变量初始化
*/
bool upgrFlag;
void UpgradeLLParam_Init(void){
	memset((uint8*)&upgradeLLParam.proCtr.step,0x00,sizeof(UpgradeLLParam));
	ulp_ULLR = getUpperLayerPara();
	upgrFlag = false;
}

/*
** 清除升级计数次数
*/
void clear_UpgrLLParamCnt(void){
	upgrLLPara.proCtrRep.cnt = 0;
}

/*
** 升级次数自加
*/
void add_UpgrCnt(void){
	upgrLLPara.proCtrRep.cnt++;
}

/*
** 升级文件版本号之多包
*/
void upgrFileMultiPackect_1(uint8* len, uint8 *item){
	uint8 txLen = 0;	
	ComBup comBupTemp = getCurComBup();

	/*
	** 帧标识 帧字节长度
	*/
	item[txLen] = 1;
	txLen += sizeof(uint8);
	item[txLen] = 6;
	txLen += sizeof(uint8);
	/*
	** 升级方式 文件类型 软件版本号 硬件版本号
	*/
	item[txLen] = comBupTemp.upgradeWay;
	txLen += sizeof(uint8);
	item[txLen] = comBupTemp.binFileType.flag;
	txLen += sizeof(uint8);
	memcpy((uint8*)&item[txLen],(uint8*)&comBupTemp.softVer,sizeof(uint16));
	txLen += sizeof(uint16);
	memcpy((uint8*)&item[txLen],(uint8*)&comBupTemp.detailedInfo.hardVer,sizeof(uint16));
	txLen += sizeof(uint16);

	/*
	** 更新数据长度
	*/
	*len = txLen;
}

void upgrFileMultiPackect_2(uint8* len, uint8 *item){
	uint8 txLen = 0;	
	ComBup comBupTemp = getCurComBup();

	/*
	** 帧标识 帧字节长度
	*/
	item[txLen] = 2;
	txLen += sizeof(uint8);
	item[txLen] = 6;
	txLen += sizeof(uint8);
	/*
	** 设备ID
	*/
	memcpy((uint8*)&item[txLen],(uint8*)&comBupTemp.detailedInfo.id[0],6);
	txLen += 6;

	/*
	** 更新数据长度
	*/
	*len = txLen;	
}

void upgrFileMultiPackect_3(uint8* len, uint8 *item){
	uint8 txLen = 0;	
	ComBup comBupTemp = getCurComBup();

	/*
	** 帧标识 帧字节长度
	*/
	item[txLen] = 3;
	txLen += sizeof(uint8);
	item[txLen] = 6;
	txLen += sizeof(uint8);
	/*
	** 设备ID
	*/
	memcpy((uint8*)&item[txLen],(uint8*)&comBupTemp.detailedInfo.id[6],6);
	txLen += 6;

	/*
	** 更新数据长度
	*/
	*len = txLen;	
}

void upgrFileMultiPackect_4(uint8* len, uint8 *item){
	uint8 txLen = 0;	
	ComBup comBupTemp = getCurComBup();

	/*
	** 帧标识 帧字节长度
	*/
	item[txLen] = 4;
	txLen += sizeof(uint8);
	item[txLen] = 4;
	txLen += sizeof(uint8);
	/*
	** 设备ID
	*/
	memcpy((uint8*)&item[txLen],(uint8*)&comBupTemp.detailedInfo.id[12],6);
	txLen += 6;

	/*
	** 更新数据长度
	*/
	*len = txLen;	
}

/*
** 升级之下层协议之查询所有通讯小板运行区域--广播方式升级
*/
static void UgradeLL_BCComRunAreaLogic(void){
	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*广播升级地址*/
	uint8 len = 1;
	uint8 tx[8] = {0x01,0};

	if(upgradeLLParam.proCtr.step == UpgradeLL_ProcChkRunArea 
		&& upgradeLLParam.runArea.circCnt <= sizeof(uint16)*UpgradeLL_FrameMax){
		/*设计200Ms查询一次,基于通讯小板复位等待时间限制*/
		if(TickOut((uint32 *)&upgradeLLParam.runArea.itick, 195) == true){
			TickOut((uint32 *)&upgradeLLParam.runArea.itick, 0);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_RunArea);
			/*
			** 升级之填充通讯小板升级方式
			*/
			upgrFileMultiPackect_1((uint8 *)&len, (uint8 *)&tx[0]);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_Ver);	
			upgrFileMultiPackect_2((uint8 *)&len, (uint8 *)&tx[0]);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_Ver);	
			upgrFileMultiPackect_3((uint8 *)&len, (uint8 *)&tx[0]);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_Ver);	
			upgrFileMultiPackect_4((uint8 *)&len, (uint8 *)&tx[0]);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_Ver);	
			
			upgradeLLParam.runArea.circCnt++;
		}
	}else{
		if(upgradeLLParam.proCtr.step == UpgradeLL_ProcChkRunArea){/*可升级通讯小板状态已更新完成,开始升级处理*/
			upgradeLLParam.proCtr.step = UpgradeLL_Proc_FileByteCrc;
			/*
			** 更新升级命令之开始数据传输
			*/
			upgradeLLParam.fileByteItemCrc.cmd = 0x01;/*开始数据传输*/
			/*优化逻辑:防止下发F1之后,暂未完全接收并更新标志位--之前版本在F0中置位允许升级标志导致异常--修改时间:20210222*/
			TickOut((uint32 *)&upgradeLLParam.fileByteItemCrc.itick, 0);
		}
	}
}

/*
** 升级之填充文件字节内容+文件内容校验
*/
void UpgradeLL_FillFileByteItemCrc(uint8* len, uint8 *item){
	uint8 txlen = 0;
	BinFileItemPara binFileItemPara = getCurComBup().binFileItemPara;

	/*
	** 数据项 升级流程命令 文件字节数 文件内容校验
	*/
	item[txlen] = upgradeLLParam.fileByteItemCrc.cmd;
	txlen += sizeof(uint8);
	memcpy((uint8*)&item[txlen],(uint8*)&binFileItemPara.len,sizeof(uint32));
	txlen += sizeof(uint32);
	memcpy((uint8*)&item[txlen],(uint8*)&binFileItemPara.crc16,sizeof(uint32));
	txlen += sizeof(uint16);
	/*
	** 数据项长度
	*/
	*len = txlen;	
}

/*
** 更新上层协议中升级字段处理
** @param: 
***		filed-->更新字段 
**		lab  --> 0x01:"升级中"字段 0x02:"升级完成"字段
*/
void update_Upperfiled(DoorNumDefine filed,uint8 lab){
	ComBup comBupTemp = getCurComBup();
	
	if(comBupTemp.binFileType.flag == ComBoradType){/*通讯板*/
		switch(lab){
			case 0x01:/*升级中*/
				ulp_ULLR->stateInfoChange.sysLogic.comUpgr = filed;
				ulp_ULLR->stateInfoChange.sysLogic.comUpgrIsOk = 0;
				break;
			case 0x02:/*升级完成*/
				ulp_ULLR->stateInfoChange.sysLogic.comUpgr = 0;
				ulp_ULLR->stateInfoChange.sysLogic.comUpgrIsOk = filed;				
				break;
		}				
	}else{/*电池固件*/
		/*优化处理*/
		if(comBupTemp.binFileType.flag == BatBoardType){
			switch(lab){
				case 0x01:/*下载中*/
					ulp_ULLR->stateInfoChange.sysLogic.batFileDownload = filed;
					ulp_ULLR->stateInfoChange.sysLogic.batFileDownloadIsOk = 0;				
					break;
				case 0x02:/*下载完成*/
					ulp_ULLR->stateInfoChange.sysLogic.batFileDownload = 0;
					ulp_ULLR->stateInfoChange.sysLogic.batFileDownloadIsOk = filed;					
					break;
			}
		}
	}	

	/*
	** 更新同一固件升级次数参数更新
	*/
	if(lab == 0x01){/*下载中*/
		ulp_ULLR->stateInfoChange.sameFireRepCnt = upgrLLPara.proCtrRep.cnt;
	}
}

/*
** 升级之下层协议之广播升级之文件字节数+文件内容校验
*/
static void UpgradeLL_BCFileByteItemCrc(void){
	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*广播升级地址*/
	uint8 len = 0;
	uint8 tx[8] = {0};
	static uint32 tickBase = 200;

	/*
	** 更新定时器基准时间
	*/
	if(upgradeLLParam.fileByteItemCrc.cmd == 0x01){/*开始数据传输*/
		tickBase = 1000;
	}

	if(upgradeLLParam.fileByteItemCrc.cmd == 0x02){/*结束数据传输*/
		tickBase = 1000;/*更新基准时间基于结束数据传输在于通讯小板需要擦除,写Flash需要时间,目前测算700MS*/
	}	

	if(upgradeLLParam.proCtr.step == UpgradeLL_Proc_FileByteCrc
		&& upgradeLLParam.fileByteItemCrc.circCnt < 3/*sizeof(uint16)*UpgradeLL_FrameMax*/
		&& upgradeLLParam.proCtr.upgradePermit != 0){/*允许升级通讯板个数>=1*/
		if(TickOut((uint32 *)&upgradeLLParam.fileByteItemCrc.itick, tickBase) == true){
			TickOut((uint32 *)&upgradeLLParam.fileByteItemCrc.itick, 0);
			UpgradeLL_FillFileByteItemCrc((uint8 *)&len, (uint8 *)&tx[0]);
			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_FileByteCrc);
			upgradeLLParam.fileByteItemCrc.circCnt++;
			/*
			** 更新上层协议中升级字段处理--修改:仅在升级upgradeLLParam.fileByteItemCrc.cmd == 0x01为开始升级数据
			*/
			if(upgradeLLParam.fileByteItemCrc.cmd == 0x01){
				update_Upperfiled(upgradeLLParam.proCtr.upgradePermit, 0x01);/*更新固件/下载文件中*/
			}
		}
	}else{
		if(upgradeLLParam.proCtr.step == UpgradeLL_Proc_FileByteCrc ){
			if(upgradeLLParam.fileByteItemCrc.cmd == 0x01/*开始传输数据*/
				&& upgradeLLParam.proCtr.binDataItemTransmit != 0){/*待传输数据地址存在*/
				upgradeLLParam.proCtr.step = UpgradeLL_Proc_DataItemTra;
				/*
				** 升级之更新升级文件内容参数
				*/
				Upgrade_UpdateFileItemPara(1);		
			}else{
				upgradeLLParam.proCtr.step  = UpgradeLL_Proc_Finsh;/*结束进程*/
			}
		}
	}
}

/*
** 升级之文件内容之数据帧完成发送标志位
*/
bool Upgrade_FileItemTraFinshFlag(void){
	bool ret = false;
	BinFileItemPara binFileItemPara = getCurComBup().binFileItemPara;
	uint32 binLen = binFileItemPara.len;
	uint8 frameNum = 0;

	/*
	** 兼容不同文件大小的单片机 
	*/
	if((binLen%sizeof(uint32)) != 0){
		frameNum = 1;
	}
	binLen = ((binLen/sizeof(uint32)) + frameNum)*sizeof(uint32);
	
	if(/*binFileItemPara.len*/binLen == (upgradeLLParam.fileItem.frameLabel- 0x01)*sizeof(uint32)){
		ret = true;
	}

	return ret;
}

/*
** 升级之更新升级文件内容参数
*/
void Upgrade_UpdateFileItemPara(uint16 frameLabel){
	ComBup comBupTemp = getCurComBup();
	
	/*
	** 帧标号
	*/
	upgradeLLParam.fileItem.frameLabel = frameLabel;
	/*
	** 帧数据内容字节
	*/
	upgradeLLParam.fileItem.dataItemLen = sizeof(uint32);
	/*
	** 文件内容 
	*/
	sFLASH_ReadBuffer((uint8 *)&upgradeLLParam.fileItem.data[0], comBupTemp.flashAddr + (frameLabel - 0x01)*sizeof(uint32), sizeof(uint32));
}

/*
** 升级之填充文件内容
*/
void UpgradeLL_FillFileItem(uint8* len, uint8 *item){
	uint8 txlen = 0;

	/*
	** 数据项 帧标识 帧数据内容字节数 文件内容
	*/
	memcpy((uint8*)&item[txlen],(uint8*)&upgradeLLParam.fileItem.frameLabel,sizeof(uint16));
	txlen += sizeof(uint16);
	item[txlen] = upgradeLLParam.fileItem.dataItemLen;
	txlen += sizeof(uint8);
	memcpy((uint8*)&item[txlen],(uint8*)&upgradeLLParam.fileItem.data[0],sizeof(uint32));
	txlen += sizeof(uint32);
	/*
	** 数据项长度
	*/
	*len = txlen;		
}

/*
** 升级之下层协议之广播升级之文件内容
*/
static void UpgradeLL_BCFileItem(void){
	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*广播升级地址*/
	static uint8 len = 0;
	static uint8 tx[8] = {0};

	/*
	** 文件最新下发策略:
	** 					1.定时17ms每一帧下发
	**					2.不检测分控是否收到
	*/	
	if(upgradeLLParam.proCtr.step == UpgradeLL_Proc_DataItemTra
		&& TickOut((uint32 *)&upgradeLLParam.fileItemCtr.itick,10) == true){
		TickOut((uint32 *)&upgradeLLParam.fileItemCtr.itick, 0);
		upgradeLLParam.fileItemCtr.cnt  = 0;
		if(upgradeLLParam.fileItemCtr.cnt == 0){/*更新数据*/
			Upgrade_UpdateFileItemPara(upgradeLLParam.fileItem.frameLabel);
			UpgradeLL_FillFileItem((uint8 *)&len, (uint8 *)&tx[0]);
			/*
			** 更新帧标志之后,更接收标志位
			*/
			upgradeLLParam.proCtr.fileItemRecFlag = 0;
		}
		
		if(upgradeLLParam.fileItemCtr.cnt++ >= UpgradeLL_FrameMax){
			upgradeLLParam.fileItemCtr.cnt = 0;
			/*
			** 通讯小板未回复故更新可升级通讯小板标志位
			*/
			upgradeLLParam.proCtr.binDataItemTransmit = upgradeLLParam.proCtr.fileItemRecFlag;
			/*
			** 升级之文件内容之数据帧完成发送标志位
			*/			
			if(Upgrade_FileItemTraFinshFlag() == true){
				upgradeLLParam.proCtr.step  = UpgradeLL_Proc_FileByteCrc;
				upgradeLLParam.fileByteItemCrc.circCnt = 0;
				/*
				** 更新升级命令之结束数据传输
				*/
				upgradeLLParam.fileByteItemCrc.cmd = 0x02;/*结束数据传输*/
			}
		}
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_FileItem);
		upgradeLLParam.fileItem.frameLabel++;
		if(Upgrade_FileItemTraFinshFlag() == true){
			upgradeLLParam.proCtr.step  = UpgradeLL_Proc_FileByteCrc;
			upgradeLLParam.fileByteItemCrc.circCnt = 0;
			/*
			** 更新升级命令之结束数据传输
			*/
			upgradeLLParam.fileByteItemCrc.cmd = 0x02;/*结束数据传输*/
			/*清结束传输定时器--分控可能最后一包未接收成功--修改时间:20210222--上午11:01*/
			TickOut((uint32 *)&upgradeLLParam.fileByteItemCrc.itick, 0);
		}
	}
}

/*
** 查询App软件版本号
*/
extern SysParam sysParam;
extern void update_ComBatSoftVer(uint8* len, uint8 *item,uint8 label);
uint8 assignLabel;
uint16 assignSoftVer[SysCtr_AllDoorNum];
bool assign_ChkVer(uint8 label){
	uint8 i = 0;
	uint16 binFileVer = 0;
	UpgrFilePara upgrTemp = {0};
	bool ret = false;

	if(label == 0){
		binFileVer = sysParam.newSoftVer.comFile;/*通讯板外部Flash存储软件版本号*/
		/*
		** 通讯板软件版本号
		*/
		
	}else{
		upgrTemp = get_UpgrBatFilePara(label - 1);
		binFileVer = upgrTemp.upgrFileType.softVer;/*外部电池固件文件版本号*/
	}
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		if(binFileVer != assignSoftVer[i]/*binFileVer > assignSoftVer[i]*/ && binFileVer != 0){
			ret = true;
			break;
		}	
	}
	return ret;
}

static void UpgradeLL_BCChkAppVer(void){
 	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*广播升级地址*/
 	uint8 len = 1;
 	uint8 tx[8] = {0};/*0x00--针对通讯板软件*/
	ComBup  comBupTemp = getCurComBup();
	UpgrFilePara upgr = {0};
	
	if(upgradeLLParam.proCtr.step  == UpgradeLL_Proc_Finsh){/*结束进程*/
		if(TickOut((uint32 *)&upgradeLLParam.appRunVer.itick, 100/*40*//*60*/) == true){
			TickOut((uint32 *)&upgradeLLParam.appRunVer.itick, 0);
			if(upgradeLLParam.appRunVer.cnt++ >= sizeof(uint16)*UpgradeLL_FrameMax){
				/*设置升级完成标志*/
				update_Upperfiled(upgradeLLParam.proCtr.binDataItemEndFlag, 0x02);				
				if(/*(upgradeLLParam.proCtr.upgradePermit == upgradeLLParam.proCtr.binDataItemEndFlag
					&& upgradeLLParam.proCtr.upgradePermit != 0)
					||*/ upgrLLPara.proCtrRep.cnt >= BroadCast_UgpradeMaxCnt
					|| assign_ChkVer(assignLabel) == false/*版本一致*/){
					upgradeLLParam.proCtr.step  = UpgradeLL_Proc_Over;
					/*
					** 清除当前通讯升级任务参数
					*/
					/*
					** 分控从bootloader退出需要时间
					*/
					if(comBupTemp.binFileType.flag == ComBoradType){/*延时5S*/
						Sleep(5000);
					}
					Clear_CurComBupPara();	
				}else{
					UpgradeLLParam_Init();
					add_UpgrCnt();
				}	

				/*在美团1.5版本每次下载完成一次之后延时6S*/
				Sleep(6000);
			}
			
			if(comBupTemp.binFileType.flag == ComBoradType){
				assignLabel = 0;
			}else{
				memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&comBupTemp.detailedInfo.hardVer,sizeof(uint16));
				memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&comBupTemp.detailedInfo.id[0],16);
				assignLabel = get_BatLocation(upgr);
				assignLabel += 1;
			}
			update_ComBatSoftVer((uint8 *)&len, (uint8 *)&tx[0], assignLabel);
 			CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_ChkAppRunVer);
			/*
			** 更新上层协议中升级字段处理
			*/
			//update_Upperfiled(upgradeLLParam.proCtr.binDataItemEndFlag, 0x02);/*更新固件/下载文件完成*/
		}
	}
}

/*
** 指定取消升级对应通讯升级操作
*/
void UpgradeLL_AssignCanelUpgrade(uint8 addr){
	uint8 txlen = 0;
	uint8 item[8] = {0};
	BinFileItemPara binFileItemPara = getCurComBup().binFileItemPara;

	/*
	** 数据项 升级流程命令 文件字节数 文件内容校验
	*/
	item[txlen] = 0xFD;/*取消升级*/
	txlen += sizeof(uint8);
	memcpy((uint8*)&item[txlen],(uint8*)&binFileItemPara.len,sizeof(uint32));
	txlen += sizeof(uint32);
	memcpy((uint8*)&item[txlen],(uint8*)&binFileItemPara.crc16,sizeof(uint32));
	txlen += sizeof(uint16);

	CAN_TransmitAnalysis(CAN_Port_1, txlen, (uint8 *)&item[0], addr, UpgradeLL_FunId_FileByteCrc);
}

/*
** 提供系统软件之远程升级接口任务之下层协议之回复函数
*/
void SM_UpgradeLLReply_Task(void* p_arg){
 	ComBup comCurBupTemp = {0}; 
	/*
	** 升级之参数结构体变量初始化
	*/
	UpgradeLLParam_Init();
	/*
	** 清除升级计数次数
	*/
	clear_UpgrLLParamCnt();

	/*
	** 系统复位延时6S,防止任务先启动,导致通讯板程序进入Bootloader区
	*/
	Sleep(6000);
	
	for(;;){
		comCurBupTemp = getCurComBup();
		if(comCurBupTemp.binFileType.flag != 0){
			/*
			** 升级之下层协议之查询所有通讯小板运行区域
			*/
			UgradeLL_BCComRunAreaLogic();	
			/*
			** 升级之下层协议之广播升级之文件字节数+文件内容校验
			*/
			UpgradeLL_BCFileByteItemCrc();		
			/*
			** 升级之下层协议之广播升级之文件内容
			*/
			UpgradeLL_BCFileItem();	
			/*
			** 查询App软件版本号
			*/
			UpgradeLL_BCChkAppVer();
		}
		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
		
		Sleep(5);
	}
}

/*
** 提供系统软件之远程升级接口函数之下层协议之回复函数
*/
void UpgradeLLReply_Init(void){
	Thread_create(SM_UpgradeLLReply_Task, 0,
		(int32 *)&upgradeLLReply_TaskStk[512-1], (int8)UgradeLLReplyTask_Prio);
}

