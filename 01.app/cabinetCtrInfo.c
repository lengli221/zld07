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
			__disable_irq();/*复位之前关闭所有中断*/
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
			** 执行升级灯闪烁逻辑--开启 -- 注明:美团升级闪烁灯硬件未接
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
extern bool oneWaringResetSetFlag;
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
	DoorNumDefine batUpgringFlag = 0;/*罗平君提出需求*/

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
				newIncrease = true;
				ver = 0;
				tx[txLen] = 0x02;/*已新增*/
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
	** 1.预留1个队列,防止指针溢出
	** 2.判定电池包安卓至主控,同主控到分控之间未冲突 -- 新增兼容:下载充电器固件包不允许安卓和分控冲突,针对同一类型文件
	** 3.新增:20210303--升级主控前需先判断是否存在主控到分控下载固件 -- (升级主控无需升级等待分控升级队列(20210303取消备注))
	**		3.1--20210525--新增:存在电池升级主控拒绝升级--罗平君提出
	** 4.分控此时此刻未升级
	*/
	batUpgringFlag = (upperLayerPara.stateInfoChange.sysLogic.batUpgr & ((DoorNumDefine)0x0000FFFFFFFFFFFF));
	if(Size_SeqQueue((SeqQueue *)&comQueue) <= (SeqQueue_DepthLimit - 1)){
		if(((memcmp((uint8*)&curComBupTemp.detailedInfo.hardVer,(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo)) != 0)
			&& (upgrType == 0x03))
			|| ((upgrType == 0x01) && (curComBupTemp.binFileType.flag == 0) && (batUpgringFlag == 0))
			|| ((upgrType == 0x02) && (curComBupTemp.binFileType.flag != ComBoradType))
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
						/*注明:增加修改非智能充电器硬件版本为200*/
						|| ((upgrType == 0x01 || upgrType == 0x02)&&(detailedInfoTemp.hardVer != (uint16)200/*300*/))
						#endif
						/*新增:一级报警禁止升级*/
						|| oneWaring_ForbidUpgrDownFile() == true	
						|| oneWaringResetSetFlag == true/*20210302一级告警已发生,下电恢复之后才能重新下开始检测是否接受文件*/
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
** 整柜控制信息之获取指定电池包固件信息 -- 0x0090 -->策略使用起始标号的方式查询电池固件包的固件信息
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
** 查询通讯板运行版本号/电池固件版本号 -- 事件:0x00F0 -- 分控实际运行分控软件版本号,电池固件软件版本号,充电器软件版本号 -- 20210301 -- 现阶段:安卓李工目前仅处理所有分控实际运行的版本号
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

/*
** 配置相应固件包升级次数 -- 事件ID:0x0100
*/
void cabCtrInfo_EventId_CfgFireUpgrNum(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;
	UpgrFilePara upgr = {0};
	uint8 cmdExceState = 0x02;/*命令执行失败*/
	int16 loc = -1;
	int16 upgrNum = -1;

	/*固件包类型*/
	fireType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*硬件版本,ID*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	len += 16;
	memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo));
	txLen += sizeof(DetailedInfo);
	
	/*升级次数*/
	memcpy((uint8*)&upgrNum,(uint8*)&rx[len],sizeof(uint16));

	switch(fireType){
		case 0x03:/*电池固件包*/
			loc = get_BatLocation(upgr);
			break;
		case 0x04:/*充电器固件包*/

			break;
	}
	
	if(loc != -1){/*固件查找成功*/
		cmdExceState = 0x01;
		set_FireUpgrNum(fireType, upgrNum, loc);
		/*清实际升级次数*/
		set_FireRealityRunNum(fireType, 0, loc);
	}

	/*命令执行状态*/
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 回复清除相应固件包 -- 事件ID:0x0110
*/
void cabCtrInfo_EventId_ClearFire(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;
	UpgrFilePara upgr = {0};
	UpgrFilePara upgrTemp = {0};
	int16 loc = -1;
	uint8 cmdExceState = 1;/*执行成功*/
	
	/*固件包类型*/
	fireType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*硬件版本,ID*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	len += 16;
	memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo));
	txLen += sizeof(DetailedInfo);	

	switch(fireType){
		case 0x03:/*电池固件包*/
			loc = get_BatLocation(upgr);
			if(loc != -1){
				upgrTemp = get_UpgrBatFilePara(loc);
				if(upgrTemp.upgrFileType.softVer != 0){
					/*清主控固件包参数*/
					memset((uint8*)&upgrTemp.upgrFileType.softVer,0,sizeof(UpgrFilePara)-sizeof(uint8)-sizeof(uint8)-sizeof(DetailedInfo));
					/*更新固件存储数据*/
					set_AssignUpgrBinFilePara(upgrTemp, loc);
					/*设置分控清除固件包标志*/
					set_ClearFire(fireType,true,loc);
				}else{
					cmdExceState = 2;/*对应固件已清除*/
				}
			}else{
				cmdExceState = 4;/*固件信息不存在*/
			}
			break;
		case 0x04:/*充电器固件包*/
			break;
	}
	/*命令执行状态*/
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 对应仓固件升级失败原因上报 -- 事件ID:0x0120
*/
void cabCtrInfo_EventId_UpgrFailReson(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;
	uint8 addr = 0;
	int8 cmd = -1;
	static UpgrFail upgrFailTemp[SysCtr_AllDoorNum] = {0};
	UpgrFail* upgrFailTempp = NULL;
	uint8 cmdExceState = 1;/*未接收到升级原因上报操作*/
	uint8 i = 0;

	/*固件类型*/
	fireType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*设备地址*/
	addr = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*命令*/
	cmd = rx[len];
	switch(cmd){
		case 0x01:/*查询升级失败原因*/
			switch(fireType){
				case 0x03:/*电池固件包*/
					if(upgrFailTemp[addr].bat.reson == 0){
						upgrFailTempp = (UpgrFail*)get_UpgrFailPara(0x03,addr);
						if(upgrFailTempp == NULL){/*调试新增:安卓查询失败原因在分控上报失败原因之前,调用任务延时(sleep(200ms)),重新读取*/
							for(i=0;i<40;i++){/*此处任务延时最大时间为:200ms=5*40*/
								Sleep(5);
								upgrFailTempp = (UpgrFail*)get_UpgrFailPara(0x03,addr);
								if(upgrFailTempp != NULL){
									break;
								}
							}
						}
						if(upgrFailTempp != NULL){
							upgrFailTemp[addr] = *(UpgrFail*)upgrFailTempp;
							cmdExceState = 2;/*执行升级原因上报*/
						}
					}else{/*上报过升级失败,但未确认清除升级失败原因参数*/
						cmdExceState = 2;/*执行升级原因上报*/
					}
					break;
				case 0x04:/*充电器固件包*/
				
					break;
			}
			break;
		case 0x02:/*清除对应失败原因*/
			switch(fireType){
				case 0x03:/*电池固件包*/
					memset((uint8*)&upgrFailTemp[addr],0,sizeof(UpgrFail));
					cmdExceState = 4;/*清除成功*/
					break;
				case 0x04:/*充电器固件包*/				
					break;
			}			
			break;
	}
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	/*填充升级失败原因字段参数*/
	if(cmdExceState == 0x02){
		switch(fireType){
			case 0x03:/*电池固件*/
				memcpy((uint8*)&tx[txLen],(uint8*)&upgrFailTemp[addr].bat.reson,sizeof(UpgrFail));
				break;
			case 0x04:/*充电器固件*/

				break;
		}
		txLen += sizeof(UpgrFail);
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 固件测试模式 -- 事件ID:0x0130
*/
void cabCtrInfo_EventId_FireEnterTestModel(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;	
	UpgrFilePara upgr = {0};
	uint8 cmdExceState = 4;/*固件不存在*/
	int16 loc = -1;

	/*固件类型*/
	fireType = rx[len];
	tx[txLen] = fireType;
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*硬件版本,ID*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	len += 16;
	memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo));
	txLen += sizeof(DetailedInfo);	

	switch(fireType){
		case 0x03:/*电池固件包*/
			loc = get_BatLocation(upgr);
			break;
		case 0x04:/*充电器固件包*/
			break;
	}
	if(loc != -1){
		cmdExceState = 1;/*执行成功*/
		/*设置固件进入测试模式*/
		set_FireEnterTestModel(fireType, true,loc);
		/*重新进入测试模式,清除上次测试模式,升级次数*/
		set_TestModelFireUpgrNum(fireType, 0, loc);/*清成功次数*/
		set_TestModelFireUpgrNumFail(fireType,  0, loc);/*清失败次数*/
		/*测试模式--根据李工需要只要存在任何一种固件进入测试模式,则置位*/
		upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.testModelFlag = true;
	}
	
	/*执行状态*/
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** 在限制固件升级次数的情况下,查询实际升级的次数 -- 事件ID:0x0140 -- Pre OTA 统计升级次数
*/
void cabCtrInfo_EventId_ChkRealityUpgrNum(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;
	UpgrFilePara upgr = {0};
	int16 loc = -1;
	uint8 cmdExceState = 4;/*固件信息不存在*/
	uint16 realityUpgrNum = 0;/*实际升级次数*/

	/*固件包类型*/
	fireType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*硬件版本,ID*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	len += 16;
	memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo));
	txLen += sizeof(DetailedInfo);	

	switch(fireType){
		case 0x03:/*电池包固件*/
			loc = get_BatLocation(upgr);
			if(loc != -1){/*固件查找到*/
				if(get_FireUpgrNum(0x03, loc) == (int16)-1){/*固件包允许无限次数升级*/
					cmdExceState = 1;/*固件包无限制升级*/
				}else{
					cmdExceState = 2;/*固件存在升级次数限制*/
				}
			}
			break;
		case 0x04:/*充电器固件*/
			
			break;
	}
	/*是否存在固件限制次数*/
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	if(cmdExceState == 2){/*固件存在升级次数限制*/
		realityUpgrNum = get_FireRealityRunNum(fireType, loc);
		memcpy((uint8*)&tx[txLen],(uint8*)&realityUpgrNum,sizeof(uint16));
		txLen += sizeof(uint16);
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 测试模式下,电池或充电器升级次数 -- 事件ID:0x0150 -- 统计压测-->升级成功次数和失败次数
*/
void cabCtrInfo_EventId_TestModelFireUpgrNum(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 fireType = 0;
	UpgrFilePara upgr = {0};
	int16 loc = -1;
	uint8 cmdExceState = 2;/*硬件版本/固件ID不合法*/
	uint16 testModelUpgrNum = 0;/*成功次数*/
	uint16 testModelUpgrNumFail = 0;/*失败次数*/

	/*固件包类型*/
	fireType = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(uint8);
	txLen += sizeof(uint8);
	/*硬件版本,ID*/
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&rx[len],sizeof(uint16));
	len += sizeof(uint16);
	memcpy((uint8*)&upgr.upgrFileType.detailedInfo.id[0],(uint8*)&rx[len],16);
	len += 16;
	memcpy((uint8*)&tx[txLen],(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo));
	txLen += sizeof(DetailedInfo);	

	switch(fireType){
		case 0x03:/*电池包固件*/
			loc = get_BatLocation(upgr);
			if(loc != -1){
				cmdExceState = 1;
				testModelUpgrNum = get_TestModelFireUpgrNum(0x03, loc);
				testModelUpgrNumFail = get_TestModelFireUpgrNumFail(0x03, loc);
			}
			break;
		case 0x04:/*chongdian*/

			break;
	}
	/*命令执行状态*/
	tx[txLen] = cmdExceState;
	txLen += sizeof(uint8);

	if(cmdExceState == 1){
		memcpy((uint8*)&tx[txLen],(uint8*)&testModelUpgrNum,sizeof(uint16));
		txLen += sizeof(uint16);
		/*失败次数*/
		memcpy((uint8*)&tx[txLen],(uint8*)&testModelUpgrNumFail,sizeof(uint16));
		txLen += sizeof(uint16);		
	}

	/*
	** 数据项长度字段填充+数据项长度返回
	*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}

/*
** Charger OC Limit Cfg -- 事件:0x0160
*/
void cabCtrInfo_EventId_ChargerOCLimitCfg(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/

	tx[txLen] = 0x01;/*配置成功*/
	txLen += sizeof(uint8);
	/*充电器过流阈值*/
	tx[txLen] = rx[0];
	txLen += sizeof(uint8);
	/*set Charger OC Limit*/
	set_ChargerOCLimit(rx[0]);

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

/*---------------------------------------------------------清除固件------------------------------------------------------------------*/
/*
** 一键清除--分控/电池/充电器
*/
void cabCtrInfo_EventId_OneKeyClearFire(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint32 clearFireField = 0;/*清除固件*/
	UpgrFilePara upgrTemp = {0};
	uint8 i = 0;

	/*固件字段*/
	memcpy((uint8*)&clearFireField,(uint8*)&rx[len],sizeof(uint32));
	len += sizeof(uint32);

	/*清除固件*/
	if(clearFireField == 0xFFFFFFFF){
		/*清除分控固件*/
		upgrTemp.upgrFileType.board = ComBoradType;
		set_UpgrBinFilePara(upgrTemp);
		/*清除电池固件*/
		init_BatPara();/*初始化电池固件*/
		/*20210330--鉴于平台关于ProOTA关闭功能存在操作困难,故增加上位机清除*/
		for(i=0;i<get_batFireSize();i++){
			set_FireUpgrNum(0x03, -1, i);
			/*清实际升级次数*/
			set_FireRealityRunNum(0x03, 0, i);				
		}
	}

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*----------------------------美团充电柜-第三方交互方案--后端接口(外部输出)-1.05-----------------------------------------------------*/
/*
** 设置过温保护阈值
*/
void cabCtrInfo_EventId_SetBatOTempLimit(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	int8 temp = 60;/*初始阈值*/
	uint8 cmdIsLegal = 0x02;/*阈值不合法*/
	
	/*过温阈值*/
	temp = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(int8);
	txLen += sizeof(int8);

	/*检测过温阈值是否合法*/
	if(temp >= 50 && temp <= 70){
		cmdIsLegal = 0x01;/*合法*/
		set_BatOTempLimit(temp);
	}

	/*命令合法性回复*/
	tx[txLen] = cmdIsLegal;
	txLen += sizeof(uint8);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}

/*
** 设置南都低温保护阈值
*/
void cabCtrInfo_EventId_SetNanduLowPLimit(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	int8 temp = 15;/*初始阈值*/
	uint8 cmdIsLegal = 0x02;/*阈值不合法*/

	/*南都低温阈值*/
	temp = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(int8);
	txLen += sizeof(int8);

	/*检测过温阈值是否合法*/
	if(temp >= -20 && temp <= 20){
		cmdIsLegal = 0x01;/*合法*/
		set_NanduLowPLimit(temp);
	}

	/*命令合法性回复*/
	tx[txLen] = cmdIsLegal;
	txLen += sizeof(uint8);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;
}

/*
** 设置低温保护阈值
*/
void cabCtrInfo_EventId_SetBatLowPLimit(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	int8 temp = 5;/*初始阈值*/
	uint8 cmdIsLegal = 0x02;/*阈值不合法*/

	/*电池低温保护阈值*/
	temp = rx[len];
	tx[txLen] = rx[len];
	len += sizeof(int8);
	txLen += sizeof(int8);

	/*检测过温阈值是否合法*/
	if(temp >= -20 && temp <= 20){
		cmdIsLegal = 0x01;/*合法*/
		set_BatLowPLimit(temp);
	}

	/*命令合法性回复*/
	tx[txLen] = cmdIsLegal;
	txLen += sizeof(uint8);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;	
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------硬件接口切换-----------------------------------------------------------------------------*/
void cabCtrInfo_EventId_Set_Interface_Sw(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 len = 0;
	uint8 cmd = 0;/*命令*/
	uint8 recSeq[7] = {0};
	uint8 replyCmd = 0x02;/*序列不合法不执行接口切换*/
	uint8 _485_To_232Seq[7] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07};/*接口485转232*/
	uint8 _232_To_232Seq[7] = {0x07,0x06,0x05,0x04,0x03,0x02,0x01};/*接口232转485*/

	/*切换接口命令*/
	cmd = rx[len];
	len += sizeof(uint8);
	/*接收上位机序列*/
	memcpy((uint8*)&recSeq[0],(uint8*)&rx[len],7);
	if(cmd == 0x01){
		if(memcmp((uint8*)&_485_To_232Seq[0],(uint8*)&recSeq[0],7) == 0){
			update_Interface_SwFlag(0x01);/*485转232*/
			replyCmd = 0x01;/*执行接口切换*/
		}
		
		if(memcmp((uint8*)&_232_To_232Seq[0],(uint8*)&recSeq[0],7) == 0){
			update_Interface_SwFlag(0x02);/*232转485*/
			replyCmd = 0x01;/*执行接口切换*/
		}
	}else{
		replyCmd = 0x04;/*命令不合法*/
	}

	/*回复命令*/
	tx[txLen] = replyCmd;
	txLen += sizeof(uint8);
	/*回复序列*/
	memcpy((uint8*)&tx[txLen],(uint8*)&recSeq[0],7);
	txLen += 7;

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}
/*------------------------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------电池接入SOC----------------------------------------------------------------*/
void cabCtrInfo_EventId_ChkAllInsertSoc(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint8 i = 0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		memcpy((uint8*)&tx[txLen],(uint8*)&llParam.batDoor[i].batDoorSysPara.chgBefSoc,sizeof(uint8));
		txLen += sizeof(uint8);
	}

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------三相电流检测标志---------------------------------------------------*/
void cabCtrInfo_EventId_PhaseCurOpenFlag(uint8 itemDatalen,uint8* rx,
	uint8* replyItemDataLen,uint8* tx){
	uint8 txLen = 1;/*说明:tx[0]预留存储数据项长度(长度字段+数据项内容)*/
	uint32 setCurOpenChkFlag = 0;
	uint8 len = 0;
	uint8 phase = 0;

	/*电流开启检测标志*/
	memcpy((uint8*)&setCurOpenChkFlag,(uint8*)&rx[len],sizeof(uint32));
	len += sizeof(uint32);

	if(setCurOpenChkFlag == (uint32)0xEEEEEEEE){/*设置调试Debug模式*/
		set_Phase_VC_Debug_Mode();
		phase = rx[len];/*相*/
		switch(phase){
			case 0:/*A相*/
			case 1:/*B相*/
			case 2:/*C相*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = true;/*相电流总标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO |= (uint16)((uint16)1<<phase); 		
				break;	
			case 3:/*ABC三相*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = true;/*相电流总标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO = 0x07;
				break;
			case 4:/*恢复*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = false;
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO = 0;
				break;
			case 5:/*A相短路*/
			case 6:/*B相短路*/
			case 7:/*C相短路*/	
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = true;
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort |= (uint16)((uint16)1<<(phase - 5));
				break;
			case 8:/*ABC三相短路*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = true;
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort = 7;					
				break;
			case 9:/*短路恢复*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = false;
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort = 0;				
				break;
		}
	}

	if(setCurOpenChkFlag == (uint32)0xBBBBBBBB){/*恢复默认模式*/
		init_Phase_VC_Debug_Mode();
		upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = false;
		upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO = 0;
		upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = false;
		upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort = 0;			
	}

	/*copy data*/
	memcpy((uint8*)&tx[txLen],(uint8*)&setCurOpenChkFlag,sizeof(uint32));
	txLen += sizeof(uint32);

	/*数据项长度字段填充+数据项长度返回*/
	tx[0] =  txLen-sizeof(uint8);
	replyItemDataLen[0] = txLen;		
}
/*-----------------------------------------------------------------------------------------------------------------------------------*/

