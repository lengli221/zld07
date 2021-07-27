#include "includes.h"

static int32 lowerLayerReply_TaskStk[512] = {0};

/*
** 调用其他文件参数变量--使用函数指针
*/
UpperLayerPara* llR_ULP = null;
SysParam* llR_SysPara = null;

/*
** 下层协议参数定义之调用相关文件使用extern 
*/
extern LowerLayerParam llParam;

/*
** 下层协议之参数初始化
*/
static void LowerLayerReplyParam_Init(void){
	llR_ULP = getUpperLayerPara();
	llR_SysPara = get_SysParaPtr();
	/*
	** clear 通讯板/电池包软件版本号
	*/	
	clear_AppVer();
}

/*
** state Chk Not download File
*/
bool stateChkNotDownloadFile(uint8 deviceAddr){
	bool ret = false;

	if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0/*分控升级未处于中*/
		&& (llR_ULP->stateInfoChange.sysLogic.batFileDownload&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0/*电池文件下载未处于中*/
		){
		ret = true;
	}

	return ret;
}

/*
** 下层协议之回复之查询电池信息
*/
void LLReply_ChkBatStateInfo(uint8 cmd,uint8* len,uint8* item){
	uint8 txlen = 0;
	uint8 socLinit = get_SocLimit();
	uint16 chgOverTemp = get_ChgOverTempLimit();
	uint16 chgTime = get_ChgTimeLimit();
	uint16 sysModulePara = 0;

	/*
	** 数据项
	** 1.系统故障--涉及通讯板停止充电处理逻辑
	** 2.soc阈值
	** 3.充电器过温阈值
	** 4.充电时间阈值 0xD38F -- 1101 0011 1000 1111
	*/
	sysModulePara = llR_ULP->stateInfoChange.sysModuleStateInfo.flag & SysModuleStateInfo_MaskBit_Upgr;
	memcpy((uint8*)&item[txlen],(uint8*)&sysModulePara,sizeof(uint16));
	txlen += sizeof(uint16);
	
	item[txlen] = socLinit;
	txlen += sizeof(uint8);
	memcpy((uint8*)&item[txlen],(uint8*)&chgOverTemp,sizeof(uint16));
	txlen += sizeof(uint16);
	memcpy((uint8*)&item[txlen],(uint8*)&chgTime,sizeof(uint16));
	txlen += sizeof(uint16);
	/*
	** 解除相应标志位
	*/
	item[txlen] = cmd;
	txlen += sizeof(uint8);

	/*
	** 数据项长度
	*/
	*len = txlen;
}


/*
** 下层协议之轮询查询电池信息
*/
void LLReplay_PollingChkBatStateInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 0;
	uint8 tx[8] = {0};

	if(stateChkNotDownloadFile(deviceAddr) == true){		
		LLReply_ChkBatStateInfo(0x00,(uint8 *)&len, (uint8 *)&tx[0]);
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, LL_FunId_BatDoorState);
	}
	
	llParam.comBoardChk[deviceAddr].cnt++;
	if((TickOut((uint32 *)&llParam.comBoardChk[deviceAddr].itick, 60000) == true) &&llParam.comBoardChk[deviceAddr].cnt >= LLParse_FrameCntMax){
		llParam.comBoardChk[deviceAddr].cnt = LLParse_FrameCntMax;
		if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0/*分控升级未处于中*/
			/*stateChkNotDownloadFile(deviceAddr) == true*/
			){/*20210226--屏蔽原因:充电柜信息-->0x03--交叉查询,在升级和下载固件的同时也会查询状态--说明:由于分控在boot区未回复FunId = 0x03字段*/
				llParam.comBoardChk[deviceAddr].comAbn = true;/*通讯板丢失*/
				/*20210226--分控失联强制结束升级状态和下载文件状态*/
				/*优化李工语音播报:20210227*/
				llR_ULP->stateInfoChange.sysLogic.comUpgr &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);
				llR_ULP->stateInfoChange.sysLogic.comUpgrIsOk &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);
				llR_ULP->stateInfoChange.sysLogic.batFileDownload &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);	
				llR_ULP->stateInfoChange.sysLogic.batFileDownloadIsOk &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);	
				/*20210227--充电器升级和电池升级清除标志*/
				llR_ULP->stateInfoChange.sysLogic.batUpgr &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);	
				llR_ULP->stateInfoChange.sysLogic.batUpgrIsOk &= ~(DoorNumDefine)((DoorNumDefine)1<<deviceAddr);					
		}else{/*分控升级中不判断失联*/
			TickOut((uint32 *)&llParam.comBoardChk[deviceAddr].itick, 0);
			llParam.comBoardChk[deviceAddr].cnt = 0;
			llParam.comBoardChk[deviceAddr].comAbn = false;
		}
	}
	
	deviceAddr++;
	if(deviceAddr == SysCtr_AllDoorNum){
		deviceAddr = 0;
	}
}

/*
** fill Chg Sys Info
*/
void fill_ChgSysInfo(uint8* item,uint8* len){
	uint8 txlen = 0;
	
	/*整柜分控存在一级故障*/
	if(chk_IsExitBatChargerOT() == true){
		item[txlen] |= 0x01; 
	}
	txlen += sizeof(uint8);

	/*充电电流配置*/
	item[txlen] = get_ChargerOCLimit();
	txlen += sizeof(uint8);

	/*电池过温阈值*/
	item[txlen] = get_BatOTempLimit();
	txlen += sizeof(uint8);

	/*南都低温保护*/
	item[txlen] = get_NanduLowPLimit();
	txlen += sizeof(uint8);

	/*电池低温保护*/
	item[txlen] = get_BatLowPLimit();
	txlen += sizeof(uint8);

	/*数据项*/
	*len = txlen;
}

/*
** 下层协议之查询充电仓系统信息
*/
void LLReply_ChkChgSysInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 0;
	uint8 tx[8] = {0};

	//if(stateChkNotDownloadFile(deviceAddr) == true){
		/*fill Chg Sys Info*/		
		fill_ChgSysInfo((uint8 *)&tx[0], (uint8 *)&len);
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, LL_FunId_ChgSysInfo);
	//}
	deviceAddr++;
	if(deviceAddr == SysCtr_AllDoorNum){
		deviceAddr = 0;
	}
}

/*
** 查询电池信息
*/
void LLReply_ChkBmsInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 1;
	uint8 tx[8] = {0};
	uint8 i = 0;

	for(i=deviceAddr;i<SysCtr_AllDoorNum;i++){
		if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == true){
			deviceAddr = i;
			if(stateChkNotDownloadFile(deviceAddr) == true){
				CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, LL_FunId_BmsInfo);
			}
			break;
		}
	}

	deviceAddr++;
	if(deviceAddr == SysCtr_AllDoorNum){
		deviceAddr = 0;
	}
}

/*
** power On 检查文件版本信息是否一致性
** @param:
**			label->标号
** @return:
**			true->不一致-->进入升级流程
**			false->一致
*/
extern bool oneWaringResetSetFlag;
bool powerOn_ChkVer(uint8 label){
	uint8 i = 0;
	uint16 binFileVer = 0;
	UpgrFilePara upgrTemp = {0};
	bool ret = false;

	if(label == 0){
		binFileVer = llR_SysPara->newSoftVer.comFile;/*通讯板外部Flash存储软件版本号*/
	}else{
		upgrTemp = get_UpgrBatFilePara(label - 1);
		binFileVer = upgrTemp.upgrFileType.softVer;/*外部电池固件文件版本号*/
	}
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*新增:仅支持未处于一级报警情况下,升级分控,和下载温控文件*/
		if(oneWaring_ForbidUpgrDownFile() == false && llR_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == false/*无一级报警且开关合上*/
			&& oneWaringResetSetFlag == false/*20210302一级告警已发生,下电恢复之后才能重新下开始判定是否升级*/){
			if(llParam.comBoardChk[i].comAbn == false){/*通讯板在线*/
				if(binFileVer != llR_SysPara->newSoftVer.comApp[i]/*binFileVer > llR_SysPara->newSoftVer.comApp[i]*/&& binFileVer != 0
					&& llR_SysPara->newSoftVer.upgrFailNumAcc[i] != 0xFFFF/*相应分控板对应固件升级失败次数未达到3次以上*/){
					ret = true;
					break;
				}	
			}
		}
	}
	return ret;
}

/*
** update Com Bat Soft Version
*/
void update_ComBatSoftVer(uint8* len, uint8 *item,uint8 label){
	uint8 txlen = 0;
	/*
	** 查询通讯板App版本号,查询指定电池包版本信息
	*/
	item[txlen] = label;
	txlen += sizeof(uint8);
	
	/*清除对应固件*/
	item[txlen] = 0;
	if(label >= 1 && label < (get_batFireSize() + 1)){/*清除指定电池固件包*/
		if(get_ClearFire(0x03, label - 1) == true){
			item[txlen] = 0xFE;
		}
	}
	txlen += sizeof(uint8);

	/*对应固件是否进入测试模式*/
	item[txlen] = 0;
	if(label >= 1 && label < (get_batFireSize() + 1)){/*电池固件*/
		if(get_FireEnterTestModel(0x03, label - 1) == true){
			item[txlen] = 0xEE;
		}
	}
	txlen += sizeof(uint8);

	/*
	** 数据项长度
	*/
	*len = txlen;
}

/*
** 查询App软件版本号 -- FunID:0xF4 -- 应用App层兼容处理 -- 广播方式处理
** 控制策略:
**			仅针对上电判断通讯板/电池固件包app软件版本是否偏低
*/
extern void update_ComUpgrQueue(UpgrFilePara upgr);
void chkComAppVer_BC(void){
	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*广播升级地址*/
	uint8 len = 0;
	uint8 tx[8] = {0};
	static bool ret = false;/*上电标志*/
	static uint32 itick = 0;
	static uint8 cnt = 0;
	static uint8 label = 0;
	UpgrFilePara upgr = {0};
	
	if(ret == false){
		if(TickOut((uint32 *)&itick, 400) == true){
			TickOut((uint32 *)&itick, 0);
			if(cnt++ >= /*sizeof(uint16)**/UpgradeLL_FrameMax){/*修改策略:检测5次,每次400ms测试--一个固件时间2S*/
				cnt = 0;
				if(powerOn_ChkVer(label) == true){/*处理升级*/
					if(label == 0x00){/*通讯板*/
						upgr = get_UpgrComFilePara();
					}else{
						upgr = get_UpgrBatFilePara(label - 1);
					}
					/*
					** 容错处理
					** 		变更需求:--因配合分控板已在bootloader发货版本已存在版本限制,故在次数处理成强制升级
					*/
					upgr.upgrFileType.cmdType = 0x02/*0x01*/;/*原始设计:断电重启之后修改成"无论什么升级方式修改成-->正常升级"*/
					update_ComUpgrQueue(upgr);
				}
				label++;
				if(label >= get_batFireSize()/*get_SizeBatFile()*/ + 1){
					/*ret = true;*//*修改:在未升级情况下,实时查询版本号是否匹配*/
					label = 0;
				}
			}
			
			if(label < get_batFireSize()+ 1){
				update_ComBatSoftVer((uint8 *)&len, (uint8 *)&tx[0], label);
				CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, UpgradeLL_FunId_ChkAppRunVer);
			}
		}
	}	
}

/*
** 下层协议之回复任务函数接口
*/
void SM_LowerLayerReply_Task(void* p_arg){
	static uint32 itick = 0;
	static uint32 itick1 = 0;
	static uint32 itick2 = 0;
	ComBup comCurBupTemp = {0}; 
	/*
	** 下层协议之参数初始化
	*/
	LowerLayerReplyParam_Init();
	
	Sleep(2000);
	
	for(;;){
		if(TickOut((uint32 *)&itick, 10/*25*/) == true){/*邮箱发送时间2ms*/
			TickOut((uint32 *)&itick, 0);
			/*
			** 查询App软件版本号 -- FunID:0xF4 -- 应用App层兼容处理 -- 广播方式处理
			** 控制策略:
			**			仅针对上电判断通讯板app软件版本是否偏低
			*/			
			comCurBupTemp = getCurComBup();
			if(comCurBupTemp.binFileType.flag == 0){
				chkComAppVer_BC();
			}
			/*
			** 下层协议之轮询查询电池信息
			*/
			LLReplay_PollingChkBatStateInfo();
		}

		if(TickOut((uint32 *)&itick2, 50) == true){
			TickOut((uint32 *)&itick2, 0);
			/*查询--充电仓信息:ID-->0x03*/
			LLReply_ChkChgSysInfo();
		}

		if(TickOut((uint32 *)&itick1, 100/*200*/) == true){/*邮箱发送时间2ms*/
			TickOut((uint32 *)&itick1, 0);
			/*
			** 查询电池信息
			*/
			LLReply_ChkBmsInfo();			
		}
		/*
		** CAN1 Tx Task
		*/
		CAN1_TxDataTask();
		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();	
		
		Sleep(5);
	}
}

/*
**提供系统软件创建下层协议查询接口函数
*/
void LowerLayerReply_Init(void){
	Thread_create(SM_LowerLayerReply_Task, 0, 
		(int32 *)&lowerLayerReply_TaskStk[512-1], LowerLayerReplyTask_Prio);
}

