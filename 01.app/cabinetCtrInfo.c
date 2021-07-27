#include "includes.h"

extern UpperLayerPara upperLayerPara;

/*
** 控制信息之接触器控制命令
*/
void cabCtrInfo_EventId_ContactorFun(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/	
	uint16 tmp = 0;

	/*
	** 命令
	*/
	switch(rx[0]){
		case 0x01:/*合上*/
			/*
			** 故障信息 -- 急停,烟感,温控,过压>=270V不可恢复
			*/	
			if((upperLayerPara.stateInfoChange.sysModuleStateInfo.flag & SysModuleStateInfo_MaskBit_Upgr) != 0){
				tx[txLen] = 0x02;/*操作失败*/
				txLen+= sizeof(uint8);
				tmp = upperLayerPara.stateInfoChange.sysModuleStateInfo.flag & SysModuleStateInfo_MaskBit_Upgr;
				memcpy((uint8*)&tx[txLen] ,(uint8*)&tmp,sizeof(uint16));
				txLen += sizeof(uint16);
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.majorLoopClose = true;
			}else{
				tx[txLen] = 0x01;/*操作成功*/
				txLen+= sizeof(uint8);
				/*
				** 接触器硬件接口 
				*/
				firewareCtr(setFireACSlaveCtr_label, true);
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.majorLoopClose = false;
			}			
			break;
		case 0x02:/*断开*/
			tx[txLen] = 0x01;/*操作成功*/
			txLen += sizeof(uint8);
			/*
			** 接触器硬件接口 
			*/
			firewareCtr(setFireACSlaveCtr_label, true);			
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.majorLoopClose = true;
			break;
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] = txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 控制信息之更新SOC参数
*/
void cabCtrInfo_EventId_UpdateSocPara(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;

	/*
	** 数据项 下发参数值 回复更新是否成功
	*/
	tx[txLen] = rx[len];
	txLen += sizeof(uint8);
	
	if(rx[len] >=50 && rx[len] <= 100){/*SOC:70-100*/
		tx[txLen] = 0x01;/*参数更新成功*/
		/*
		** set SOC阈值
		*/
		set_SocLimit(rx[len]);
	}else{
		tx[txLen] = 0x02;/*参数更新失败*/
	}
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 充电时间阈值
*/
void cabCtrInfo_EventId_ChgTime(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint16 min = 0;

	/*
	** 数据项--充电时间阈值,回复更新是否成功
	*/
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint16));
	memcpy((uint8*)&min,(uint8*)&rx[len],sizeof(uint16));
	txLen += sizeof(uint16);

	if(min >= 10){/*阈值暂定至少大于10min*/
		tx[txLen] = 0x01;/*参数更新成功*/
		/*
		** set 充电时间阈值
		*/
		set_ChgTimeLimit(min);
	}else{
		tx[txLen] = 0x02;/*参数更新失败*/
	}
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 充电器过温阈值
*/
void cabCtrInfo_EventId_ChgOverTemp(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint16 temp = 0;

	/*
	** 数据项--充电时间阈值,回复更新是否成功
	*/
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint16));
	memcpy((uint8*)&temp,(uint8*)&rx[len],sizeof(uint16));
	txLen += sizeof(uint16);

	if(temp >= 400 && temp <= 1000){/*阈值暂定40摄氏度至100摄氏度*/
		tx[txLen] = 0x01;/*参数更新成功*/
		/*
		** set 充电器过温阈值
		*/
		set_ChgOverTempLimit(temp);
	}else{
		tx[txLen] = 0x02;/*参数更新失败*/
	}
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 充电器仓过温阈值
*/
void cabCtrInfo_EventId_ChgDoorOT(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint16 temp = 0;

	/*
	** 数据项--充电时间阈值,回复更新是否成功
	*/
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint16));
	memcpy((uint8*)&temp,(uint8*)&rx[len],sizeof(uint16));
	txLen += sizeof(uint16);

	if(temp >= 400 && temp <= 1000){/*阈值暂定40摄氏度至100摄氏度*/
		tx[txLen] = 0x01;/*参数更新成功*/
		/*
		** set 充电器仓过温阈值
		*/
		set_ChgDoorOTLimit(temp);
	}else{
		tx[txLen] = 0x02;/*参数更新失败*/
	}
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 整柜控制信息之系统温度相关信息
*/
void cabCtrInfo_EventId_SysTempInfo(uint8 itemDatalen,uint8* rx,
	uint8*replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len=0;
	Tsave ts = get_Tsave();

	/*
	** 命令
	*/
	tx[txLen] = rx[len];
	txLen += sizeof(uint8);
	switch(rx[len]){
		case 0x01:/*查询充电仓环温*/
			memcpy((uint8*)&tx[txLen],(uint8*)&upperLayerPara.sysFire.chgDoorTemp,sizeof(uint16));
			txLen += sizeof(uint16);
			break;
		case 0x02:/*查询基准温度和温度回差值*/
			memcpy((uint8*)&tx[txLen],(uint8*)&ts.base,sizeof(uint16));
			txLen += sizeof(uint16);
			memcpy((uint8*)&tx[txLen],(uint8*)&ts.diff,sizeof(uint16));
			txLen += sizeof(uint16);
			break;
		case 0x03:
			len += sizeof(uint8);
			memcpy((uint8*)&ts.base,(uint8*)&rx[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&ts.diff,(uint8*)&rx[len],sizeof(uint16));
			len += sizeof(uint16);
			if((ts.base >= 250 && ts.base <= 500)
				&&(ts.diff >= 50 && ts.diff <= 100)){
				tx[txLen] = 0x01;/*设置成功*/
				set_Tsave(ts);
			}else{
				tx[txLen] = 0x02;/*设置失败*/
				ts = get_Tsave();
			}
			txLen += sizeof(uint8);
			memcpy((uint8*)&tx[txLen],(uint8*)&ts.base,sizeof(Tsave));
			txLen += sizeof(Tsave);			
			break;
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 控制板之进入Bootloader区域标志
*/
void enterCtrBootFlag(void){
	upperLayerPara.upgrCtrReset.flag = true;
	TickOut((uint32 *)&upperLayerPara.upgrCtrReset.itick, 0);
}

/*
** 控制板之下载通讯板/电池包文件
*/
void enterComBatFileDownload(void){
	upperLayerPara.upgrComBatTaskEn.flag = true;
	TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itick, 0);
	TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itickout, 0);
}

/*
** 控制板之更新下载通讯/电池包文件
*/
void downloadComBatFileUpdatePara(uint8 cmdType,uint8 upgrType,
				DetailedInfo detailedInfo){
	UpgrFilePara upgrFileParaTmep = {0};

	/*
	** 升级命令 -- 增加偏移值0x01:解决上层协议和下层协议不一致导致
	** 		变更需求:--因配合分控板已在bootloader发货版本已存在版本限制,故在次数处理成强制升级
	*/
	upgrFileParaTmep.upgrFileType.cmdType = 0x02/*cmdType + 0x01*/;/*针对分控处理成强制升级*/

	/*
	** 升级类型
	*/
	switch(upgrType){
		case 0x02:/*通讯板*/
			upgrFileParaTmep.upgrFileType.board = ComBoradType;
			break;
		case 0x03:/*电池包*/
			upgrFileParaTmep.upgrFileType.board = BatBoardType;
			break;
	}
	/*
	** 升级电池包详细信息
	*/
	upgrFileParaTmep.upgrFileType.detailedInfo = detailedInfo;

	/*
	** 更新参数
	** 		Note:set控制板下载文件之参数变量 
	*/
	setUpgrFilePara(upgrFileParaTmep);	
}

/*
** 控制板执行BootLoader策略:
**		1.接收远程升级指令
**		2.等待执行回复,800ms之后重启主控进入BootLoader
*/
void enterCtrBootLoader(void){
	if(upperLayerPara.upgrCtrReset.flag == true){
		if(TickOut((uint32 *)&upperLayerPara.upgrCtrReset.itick, 800) == true){
			NVIC_SystemReset();
		}
	}
}

/*
** 下载通讯板/电池包文件策略:
**		1.接收下载通讯小板命令
**		2.等待执行回复命令,间隔800ms之后控制上层任务切换
*/
void ctrBoard_TaskSwitch(void){
	static bool resumeFlag = true;
	static bool resFlag = true;

	/*
	** 下载文件超时处理
	*/
	if(upperLayerPara.upgrComBatTaskEn.flag == true){
		if(TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itickout, 5000) == true){
			TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itickout, 0);
			upperLayerPara.upgrComBatTaskEn.flag = false;
		}
	}

	if(upperLayerPara.upgrComBatTaskEn.flag == true){/*执行远程升级任务*/
		if(TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itick, 800) == true){
			TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itick, 0);
			if(resumeFlag == true){/*挂起App接口函数任务*/
				resumeFlag = false;
				OSTaskSuspend(UpperLayerParseTask_Prio);
				OSTaskSuspend(UpperLayerReplyTask_Prio);
			}
			
			if(resFlag == false){/*恢复远程升级接口函数*/
				resFlag = true;
				OSTaskResume(UgradeULParseTask_Prio);
				/*
				** 恢复任务之前清超时定时器
				*/
				TickOut((uint32 *)&upperLayerPara.upgrComBatTaskEn.itickout, 0);
			}
			/*
			** 执行升级灯闪烁逻辑--开启
			*/
			UpgradeUL_CtrLedLogic(true);
		}
	}else{/*执行App接口函数任务*/
		if(resumeFlag == false){/*恢复App接口任务*/
			resumeFlag = true;
			OSTaskResume(UpperLayerParseTask_Prio);
			OSTaskResume(UpperLayerReplyTask_Prio);
		}

		if(resFlag == true){/*挂起远程升级接口函数*/
			resFlag = false;
			OSTaskSuspend(UgradeULParseTask_Prio);
		}
		/*
		** 执行升级灯闪烁逻辑--关闭
		*/
		UpgradeUL_CtrLedLogic(false);		
	}	
}

/*
** set 恢复上层协议之APP应用层协议标志
*/
void set_CtrBoardTaskRecoverFlag(void){
	upperLayerPara.upgrComBatTaskDis.flag = true;
	TickOut((uint32 *)&upperLayerPara.upgrComBatTaskDis.itick, 0);
}

/*
** 恢复上层协议之APP应用层协议 
**		控制策略:
**				1.文件下载完成之后,设置标志
**				2.等待回复(延时)800ms之后设置任务切换
*/
void ctrBoard_TaskRecover(void){
	if(upperLayerPara.upgrComBatTaskDis.flag == true){
		if(TickOut((uint32 *)&upperLayerPara.upgrComBatTaskDis.itick, 800) == true){
			TickOut((uint32 *)&upperLayerPara.upgrComBatTaskDis.itick, 0);
			upperLayerPara.upgrComBatTaskEn.flag = false;
			/*
			** 清恢复任务标志
			*/
			upperLayerPara.upgrComBatTaskDis.flag = false;
		}
	}
}

/*
** 整柜控制信息之下载文件
*/
extern SeqQueue comQueue;/*检测升级队列参数,升级目标个数*/
void cabCtrInfo_EventId_UpgradeFileCmd(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	UpgrFilePara upgr = {0};
	DetailedInfo detailedInfoTemp = {0};
	uint16 ver = 0;
	uint16 getVer = 0;
	int16 loc = 0;
	uint8 upgrType = 0;
	uint8 cmdWay = 0;
	ComBup curComBupTemp = getCurComBup();
	bool newIncrease = false;

	/*
	** 升级类型
	*/
	upgrType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*
	** 获取硬件版本号,ID
	*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	txLen += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],16);
	len += 16;
	txLen += 16;
	/*
	** 是否需要新增区域--控制板,通讯板无需,电池包固件根据分析
	*/
	tx[txLen] = 0x01;/*无需*/
	switch(upgrType){
		case 0x01:/*控制板*/
			/*ver = (uint16)get_CtrVer();*/
			ver = get_CtrSoftVer();
			/*
			** 更新主控硬件版本信息
			*/
			detailedInfoTemp = upgr.upgrFileType.detailedInfo;
			break;
		case 0x02:/*通讯板*/
			/*
			** 更新分控硬件版本
			*/
			detailedInfoTemp = upgr.upgrFileType.detailedInfo;
			upgr = get_UpgrComFilePara();
			ver = upgr.upgrFileType.softVer;/*软件版本号*/
			break;
		case 0x03:/*电池包*/
			loc = get_BatLocation(upgr);
			if(loc == -1){
// 				loc = get_batFireSize()/*get_SizeBatFile()*/ + 1;
// 				set_SizeBatFile(loc);
// 				set_AssignUpgrBinFilePara(upgr, loc);
				ver = 0;
				tx[txLen] = 0x02;/*已新增*/
				newIncrease = true;
			}else{
				ver = get_UpgrBatFilePara(loc).upgrFileType.softVer;
			}
			/*
			** 更新电池包详细信息
			*/
			detailedInfoTemp = upgr.upgrFileType.detailedInfo; 
			break;
	}
	txLen += sizeof(uint8); 
	
	/*
	** 命令ID--升级方式
	*/
	cmdWay = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*
	** get 软件版本号
	*/
	memcpy((uint8*)&getVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	/*
	** 命令执行状态
	*/
	if(Size_SeqQueue((SeqQueue *)&comQueue) <= (SeqQueue_DepthLimit - 1)/*预留1个队列,防止指针溢出*/){
		if(((memcmp((uint8*)&curComBupTemp.detailedInfo.hardVer,(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo)) != 0)
			&& (upgrType == 0x03))/*判定电池包安卓至主控,同主控到分控之间未冲突*/
			|| (upgrType == 0x01)/*升级主控无需升级等待分控升级队列*/
			|| ((upgrType == 0x02) && (curComBupTemp.binFileType.flag != ComBoradType))/*分控此时此刻未升级*/
			){
			tx[txLen] = 0x01;/*允许升级*/
			switch(cmdWay){
				case 0x00:/*正常升级*/
				case 0x01:/*强制升级*/
					if((getVer == ver || newIncrease == true)
						/*
						** 注明:主控板,分控板增加硬件版本判断
						*/
						#if CtrHardware == CtrHardware_Dwin
						|| ((upgrType == 0x01 || upgrType == 0x02)&&(detailedInfoTemp.hardVer != (uint16)100))
						#elif  CtrHardware == CtrHardware_Andriod
						|| ((upgrType == 0x01 || upgrType == 0x02)&&(detailedInfoTemp.hardVer != (uint16)200))
						#endif
						/*新增:一级报警禁止升级*/
						|| oneWaring_ForbidUpgrDownFile() == true
						){
						tx[txLen] = 0x02;/*拒绝升级*/
						newIncrease = false;
					}else{
						if(upgrType == 0x01){/*控制板*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.ctrUpgr = true;
							enterCtrBootFlag();
						}else{/*通讯板/电池包*/
							enterComBatFileDownload();
						}
						downloadComBatFileUpdatePara(cmdWay, upgrType, detailedInfoTemp);
					}
					break;
// 				case 0x01:/*强制升级*/
// 					#if CtrHardware == CtrHardware_Dwin
// 					if(((upgrType == 0x01 || upgrType == 0x02)&&(detailedInfoTemp.hardVer != (uint16)100))
// 						|| (newIncrease == true)){
// 						tx[txLen] = 0x02;/*拒绝升级*/
// 						newIncrease = false;
// 					}else
// 					#elif CtrHardware == CtrHardware_Andriod
// 					if(((upgrType == 0x01 || upgrType == 0x02)&&(detailedInfoTemp.hardVer != (uint16)200))
// 						|| (newIncrease == true)){
// 						tx[txLen] = 0x02;/*拒绝升级*/
// 						newIncrease = false;
// 					}else
// 					#endif
// 					{
// 						if(upgrType == 0x01){/*控制板*/
// 							enterCtrBootFlag();
// 						}else{/*通讯板/电池包*/
// 							enterComBatFileDownload();
// 						}
// 						downloadComBatFileUpdatePara(cmdWay, upgrType, detailedInfoTemp);
// 					}
// 					break;
			}
		}else{
			tx[txLen] = 0x04;/*升级繁忙中*/
		}
	}else{
		tx[txLen] = 0x04;/*升级繁忙中*/
	}
	txLen += sizeof(uint8);

	/*
	** 版本号
	*/
	memcpy((uint8*)&tx[txLen],(uint8*)&ver,sizeof(uint16));
	txLen += sizeof(uint16);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;

	/*
	** 允许升级--填充升级秘钥
	*/
	if(upperLayerPara.upgrCtrReset.flag == true){
		set_UpgrCtrKey();
	}
}

/*
** 整柜控制信息之获取文件名称
*/
void cabCtrInfo_EventId_GetFileName(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fileType = 0;
	uint16 ver = 0;
	UpgrFilePara upgr = {0};
	UpgrFilePara upgrTemp = {0};
	int8 loc = -1;
	uint8 fileNameLen = 0;

	/*
	** 文件类型
	*/
	fileType = rx[len];
	tx[txLen] = rx[len];
	txLen += sizeof(uint8);
	len += sizeof(uint8);

	switch(fileType){/*文件类型*/
		case 0x01:/*控制板*/
			/*ver = (uint16)get_CtrVer();*/
			ver = get_CtrSoftVer();
			break;
		case 0x02:/*通讯板*/
			upgr = get_UpgrComFilePara();
			ver = upgr.upgrFileType.softVer;
			break;
		case 0x03:/*电池包*/
			/*硬件版本号,ID*/
			memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
			len += 16;
			loc = get_BatLocation(upgr);
			if(loc != -1){
				upgrTemp = get_UpgrBatFilePara(loc);
				ver = upgrTemp.upgrFileType.softVer;
			}
			break;
	}

	/*
	** 分析软件是否为初始版本,初始版本无文件名
	*/
	if(ver != 0){
		tx[txLen] = 0x01;/*操作成功*/
		txLen += sizeof(uint8);
		switch(fileType){
			case 0x01:/*控制板*/
				get_CtrFileName((uint8 *)&tx[txLen], (uint8 *)&fileNameLen);
				break;
			case 0x02:/*通讯板*/
				upgr = get_UpgrComFilePara();
				memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.fileName[0],upgr.upgrFileType.fileNameLen);
				fileNameLen = upgr.upgrFileType.fileNameLen;
				break;
			case 0x03:/*电池包*/
				upgrTemp = get_UpgrBatFilePara(loc);
				memcpy((uint8*)&tx[txLen],(uint8*)&upgrTemp.upgrFileType.fileName[0],upgrTemp.upgrFileType.fileNameLen);
				fileNameLen = upgrTemp.upgrFileType.fileNameLen;
				break;
		}
		
	}else{
		tx[txLen] = 0x02;/*操作失败*/
		txLen += sizeof(uint8);
	}
	txLen += fileNameLen;

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 整柜控制信息之获取指定电池包固件信息
*/
void cabCtrInfo_EventId_GetBatFireInfo(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint16 loc = 0;
	UpgrFilePara upgr = {0};

	/*
	** 位置信息
	*/ 
	memcpy((uint8*)&loc,(uint8*)&rx[len],sizeof(uint8));
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint8));
	txLen += sizeof(uint16);
	len += sizeof(uint16);

	/*
	** 判断位置信息是否合法
	*/
	if(loc <= get_batFireSize()){/*合法*/
		tx[txLen] = 0x01;
		txLen += sizeof(uint8);
		/*
		** 硬件版本,ID,软件版本
		*/
		upgr = get_UpgrBatFilePara(loc);
		memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(uint16));
		txLen += sizeof(uint16);
		memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.id[0],16);
		txLen += 16;
		memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.softVer,sizeof(uint16));
		txLen += sizeof(uint16);
	}else{/*不合法*/
		tx[txLen] = 0x02;
		txLen += sizeof(uint8);
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 查询所有充电器的温度
*/
extern LowerLayerParam llParam;
void cabCtrInfo_EventId_AllchgTemp(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 i = 0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		memcpy((uint8*)&tx[txLen],(uint8*)&llParam.batDoor[i].batDoorSysPara.chargerTemp,sizeof(uint16));/*充电器温度*/	
		txLen += sizeof(uint16);
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 查询BMS保护状态/故障状态
*/
void cabCtrInfo_EventId_BmsPFState(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 i = 0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*
		** 电池保护状态
		*/
		memcpy((uint8*)&tx[txLen],(uint8*)&llParam.batDoor[i].bmsInfoMeiTuan.pState.flag,sizeof(uint16));
		txLen += sizeof(uint16);
		/*
		** 电池故障状态
		*/
		tx[txLen] = llParam.batDoor[i].bmsInfoMeiTuan.faultState.flag;
		txLen += sizeof(uint8);
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 获取WIFI名称和密码
*/
void cabCtrInfo_EventId_GetWifiNamePw(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 legalLen = 0;
	WifiSet wifiSetTemp = get_WifiNamePwSysSet();

	legalLen = get_WifiNamePwIsLegalLen((uint8 *)&wifiSetTemp.name[0]);
	tx[txLen] = legalLen;
	txLen += sizeof(uint8);
	memcpy((uint8*)&tx[txLen],(uint8*)&wifiSetTemp.name[0],legalLen);
	txLen += legalLen;

	legalLen = get_WifiNamePwIsLegalLen((uint8 *)&wifiSetTemp.pw[0]);
	tx[txLen] = legalLen;
	txLen += sizeof(uint8);
	memcpy((uint8*)&tx[txLen],(uint8*)&wifiSetTemp.pw[0],legalLen);
	txLen += legalLen;	

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 查询SOC阈值
*/
void cabCtrInfo_EventId_ChkSocLimit(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/

	/*
	** SOC阈值
	*/
	tx[txLen] = get_SocLimit();
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 查询通讯板运行版本号/电池固件版本号
*/
void cabCtrInfo_EventId_ChkRunComVer(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint16 loc = 0;

	/*
	** 查询位置信息
	*/
	memcpy((uint8*)&loc,(uint8*)&rx[len],sizeof(uint16));
	memcpy((uint8*)&tx[txLen],(uint8*)&rx[len],sizeof(uint16));
	txLen += sizeof(uint16);
	len += sizeof(uint16);	

	/*
	** 查询位置信息是否合法
	*/
	if(loc < (get_batFireSize() + 1)){/*合法*/
		tx[txLen] = 0x01;
		txLen += sizeof(uint8);			
		memcpy((uint8*)&tx[txLen],(uint8*)&upperLayerPara.runComVer[loc][0],(uint8)(SysCtr_AllDoorNum*sizeof(uint16)));
		txLen += (SysCtr_AllDoorNum*sizeof(uint16));
	}else{/*不合法*/
		tx[txLen] = 0x02;
		txLen += sizeof(uint8);		
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*-------------------------------------------三相电压校准值--------------------------------------------------------------------------*/
/*
** 查询低压侧AD采样值
*/
void cabCtrInfo_EventId_ChkPhaseLvVoltAD(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 ph = 0;
	uint16 lvAdValue = 0;

	/*相序*/
	ph = rx[len];
	/*读取低压侧AD值*/
	lvAdValue = get_LvAdValue(ph);
	/*拷贝数据*/
	memcpy((uint8*)&tx[txLen],(uint8*)&lvAdValue,sizeof(uint16));
	txLen += sizeof(uint16);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 查询高压侧采样值
*/
void cabCtrInfo_EventId_ChkPhaseHvVoltAD(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 ph = 0;
	uint16 hvAdValue = 0;

	/*相序*/
	ph = rx[len];
	/*读取高压侧AD值*/
	hvAdValue = get_HvAdValue(ph);
	/*拷贝数据*/
	memcpy((uint8*)&tx[txLen],(uint8*)&hvAdValue,sizeof(uint16));
	txLen += sizeof(uint16);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 设置三相电压校准系数
*/
void cabCtrInfo_EventId_SetPhaseAjust(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 ph = 0;
	float k = 0;
	float b = 0;

	/*相序*/
	ph = rx[len];
	len += sizeof(uint8);
	/*设置系数*/
	memcpy((uint8*)&k,(uint8*)&rx[len],sizeof(float));
	len += sizeof(float);
	memcpy((uint8*)&b,(uint8*)&rx[len],sizeof(float));
	len += sizeof(float);	
	set_PhaseVoltAdjust(ph, true, k,  b);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 获取实时电压
*/
void cabCtrInfo_EventId_GetRuntimeVolt(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 ph = 0;
	uint16 temp = 0;
	CalcPhaseVC pvc = get_CalcPhaseVC();

	/*相序*/
	ph = rx[len];
	len += sizeof(uint8);
	/*获取实时电压*/
	temp = (uint16)(pvc.abcPV[ph]);
	memcpy((uint8*)&tx[txLen],(uint8*)&temp,sizeof(uint16));
	txLen += sizeof(uint16);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}


/*-----------------------------------------------------------------------------------------------------------------------------------*/


