#include "includes.h"

static int32 upperLayerParse_TaskStk[1024] = {0};

/*
** 使用Extern调用其他文件变量
*/
extern UpperLayerReplyPara ulReplyPara;
extern SysParam sysParam;

/*
** 上层协议之结构体变量
*/
UpperLayerPara upperLayerPara;


/*
** 上层协议之结构体变量指针提供接口调用
*/
UpperLayerPara* getUpperLayerPara(void){
	return(&upperLayerPara);
} 

/*
** 更新控制板软件版本号
*/
void update_CtrSfotVer(uint16 ver){
	/*uint16 temp = 0;*/
	uint8 len = 0;
	uint8 charVer[4] = {0};/*软件版本号*/
	#if CtrHardware == CtrHardware_Andriod
		uint8 charVerHard[4] = {"0200"};/*硬件版本号*/
	#else
		uint8 charVerHard[4] = {"0100"};/*硬件版本号*/
	#endif
	
	
	/*temp = bcdToUInt((const uint8 *)&ver, sizeof(uint16));*/
	len = _itoa(ver/*temp*/, (char*)&charVer[0]);
	memset((uint8*)&upperLayerPara.sysSet.ctrSoftVer[0],'0',4);
	memcpy((uint8*)&upperLayerPara.sysSet.ctrSoftVer[4-len],(uint8*)&charVer[0],len);
	memcpy((uint8*)&upperLayerPara.sysSet.ctrHareVer[0],(uint8*)&charVerHard[0],4);
}

/*
** 初始化上层协议变量
*/
static void init_UpperLayerPara(void){
	char temp[10] = {"麦迪瑞"};
	char tempp[6] = {"CDR-U1"};
	uint16 ctrVer = get_CtrSoftVer();
	
	memset((uint8*)&upperLayerPara.sysSet.fireId[0],0x00,sizeof(UpperLayerPara));
	/*
	** 更新柜子设备ID参数
	*/
	memcpy((uint8*)&upperLayerPara.sysSet.id[0],(uint8*)&sysParam.deviceId.show[0],16);
	/*
	** 厂商标识 柜子型号
	*/
	memcpy((uint8*)&upperLayerPara.sysSet.fireId[0],(uint8*)&temp[0],10);
	memcpy((uint8*)&upperLayerPara.sysSet.ctrModel[0],(uint8*)&tempp[0],6);
	/*
	** 更新控制板软件版本号
	*/
	update_CtrSfotVer(ctrVer);
}

/*
** update 上层相关标志
*/
static void update_UpperFieldRef(void){
	DeviceId deviceIdTemp = {0};

	/*
	** 更新过压过流,欠压检测
	*/
	PhaseVCAbnCloseAllCharger();
	
	/*
	** 更新相关系统参数
	** 		1.设备地址
	*/
	deviceIdTemp = get_SysDeviceId();
	memcpy((uint8*)&upperLayerPara.sysSet.id[0],(uint8*)&deviceIdTemp.show[0],sizeof(DeviceId));
}

/*
**校验数据帧数据长度数据是否合法
*/
bool ChkFrameDataLenIsOk(uint16 frameLen){
	bool ret = false;

	if(frameLen>=FrameLenIsLegal_Min && frameLen<=FrameLenIsLegal_Max){
		ret = true;
		
	}
	return ret;
}

/*
**校验设备地址
*/
bool ChkDeviceAddrIsOk(uint8 addr){
	bool ret = true;

	return ret;
}

/*
** 注册信息
*/
void UL_RegisterInfo(uint16 rxlen,uint8*rx){
	Serve s = {0};
	#if CtrHardware == CtrHardware_Andriod
		SmCmd cmd = {true,CMD_ID_Register_Tx,0,0};
	#endif
	
	switch(rx[0]){/*注册是否成功*/
		case 0x01:/*注册成功*/
				s.registerFlag = true;
				set_RegisterState(1);/*已注册*/
			break;
		case 0x02:/*注册失败*/
				s.registerFlag = false;
			break;
	}	
	#if CtrHardware == CtrHardware_Andriod
	switch(rx[1]){/*注册信息之查询与回复*/
		case 0x02:/*安卓主动查询*/
				setCmd(cmd);		
			break;
	}
	#endif
	/*
	** 更新注册信息标志
	*/
	ulReplyPara.registerLogic.flag = false;
	/*
	** set服务端参数
	*/
	setServePara(s);
}

/*
** 注销信息
*/
void UL_LogoutInfo(uint16 rxlen,uint8*rx){
#if CtrHardware == CtrHardware_Dwin
	Serve s = {0};

	switch(rx[0]){/*注销信息*/
		case 0x01:/*注销成功*/
			s.registerFlag = false;
			set_RegisterState(0);/*未注册*/
			break;
		case 0x02:/*注销失败*/
			
			break;
	}
	/*
	** 更新注销信息标志
	*/
	ulReplyPara.logoutLogic.flag = false;
	
	setServePara(s);
#endif
}

/*
** 状态信息变更
*/
void UL_StateInfoChange(uint16 rxlen,uint8*rx){
	SmCmd cmd = {true,CMD_ID_StateInfoChange_Tx,0,0};
	uint8 len = 0;

	/*
	** 回复/查询
	*/
	switch(rx[len]){
		case 0x01:/*回复*/
			
			break;
		case 0x2:/*查询*/
			setCmd(cmd);
			break;
		case 0xFE:/*分控复位已确认标志*/
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.comRecoverFlag = false;
			break;
	}
	len += sizeof(uint8);
	/*
	** 网络是否正常
	*/
	switch(rx[len]){
		case 0x01:/*在线*/
			upperLayerPara.sysFire.state.bits.isOnline = true;
			break;
		case 0x02:/*离线*/
			upperLayerPara.sysFire.state.bits.isOnline = false;
			break;
	}
}

/*
** 状态信息
*/
void UL_StateInfo(uint16 rxlen,uint8*rx){
	SmCmd cmd = {true,CMD_ID_StateInfo_Tx,0,0};

	setCmd(cmd);
}

/*
** 控制信息之事件接口函数表格设计
*/
static const CabinetCtrInfo cabinetCtrInfoFunTable[] = {
	{CabCtrInfo_EventId_Contactor,cabCtrInfo_EventId_ContactorFun},
	{CabCtrInfo_EventId_UpdateSoc,cabCtrInfo_EventId_UpdateSocPara},
	{CabCtrInfo_EventId_ChgTime,cabCtrInfo_EventId_ChgTime},
	{CabCtrInfo_EventId_ChgOverTemp,cabCtrInfo_EventId_ChgOverTemp},
	{CabCtrInfo_EventId_ChgDoorOT,cabCtrInfo_EventId_ChgDoorOT},
	{CabCtrInfo_EventId_SysTempInfo,cabCtrInfo_EventId_SysTempInfo},
	{CabCtrInfo_EventId_UpgrCmd,cabCtrInfo_EventId_UpgradeFileCmd},
	{CabCtrInfo_EventId_GetFileName,cabCtrInfo_EventId_GetFileName},
	{CabCtrInfo_EventId_GetBatFireInfo,cabCtrInfo_EventId_GetBatFireInfo},
	{CabCtrInfo_EventId_GetchgerTemp,cabCtrInfo_EventId_AllchgTemp},
	{CabCtrInfo_EventId_GetWifiNamePw,cabCtrInfo_EventId_GetWifiNamePw},
	{CabCtrInfo_EventId_BmsPF,cabCtrInfo_EventId_BmsPFState},
	{CabCtrInfo_EventId_ChkSocLimit,cabCtrInfo_EventId_ChkSocLimit},
	{CabCtrInfo_EventId_ChkRunComVer,cabCtrInfo_EventId_ChkRunComVer},
	/*-------------------------------------------三相电压校准值--------------------------------------------------------------------------*/	
	{CabCtrInfo_EventId_ChkLvAd,cabCtrInfo_EventId_ChkPhaseLvVoltAD},
	{CabCtrInfo_EventId_ChkHvAd,cabCtrInfo_EventId_ChkPhaseHvVoltAD},
	{CabCtrInfo_EventId_SetPAdjust,cabCtrInfo_EventId_SetPhaseAjust},
	{CabCtrInfo_EventId_GetRuntimeV,cabCtrInfo_EventId_GetRuntimeVolt},
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	{CabCtrInfo_EventId_CfgFireUpgrN,cabCtrInfo_EventId_CfgFireUpgrNum},
	{CabCtrInfo_EventId_ClearFire,cabCtrInfo_EventId_ClearFire},
	{CabCtrInfo_EventId_UpgrFailR,cabCtrInfo_EventId_UpgrFailReson},
	{CabCtrInfo_EventId_FireEnterT,cabCtrInfo_EventId_FireEnterTestModel},
	{CabCtrInfo_EventId_ChkRealityN,cabCtrInfo_EventId_ChkRealityUpgrNum},
	{CabCtrInfo_EventId_TestMUpgrNum,cabCtrInfo_EventId_TestModelFireUpgrNum},
	{CabCtrInfo_EventId_ChargerOCL,cabCtrInfo_EventId_ChargerOCLimitCfg},
	/*---------------------------------------------------------清除固件------------------------------------------------------------------*/
	{CabCtrInfo_EventId_OneClearAllFire,cabCtrInfo_EventId_OneKeyClearFire},
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	/*----------------------------美团充电柜-第三方交互方案--后端接口(外部输出)-1.05-----------------------------------------------------*/
	{CabCtrInfo_EventId_BatOTempLimit,cabCtrInfo_EventId_SetBatOTempLimit},
	{cabCtrInfo_EventId_NanduLowPLimit,cabCtrInfo_EventId_SetNanduLowPLimit},
	{CabCtrInfo_EventId_BatLowPLimit,cabCtrInfo_EventId_SetBatLowPLimit},
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	/*-------------------------------------------硬件接口切换-----------------------------------------------------------------------------*/
	{CabCtrInfo_EventId_SetInterfaceSw,cabCtrInfo_EventId_Set_Interface_Sw},
	/*------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------电池接入SOC----------------------------------------------------------------*/
	{CabCtrInfo_EventId_ChkAllInsSoc,cabCtrInfo_EventId_ChkAllInsertSoc},
	/*-----------------------------------------------------------------------------------------------------------------------------------*/
	{CabCtrInfo_PhaseVCDebugMode,cabCtrInfo_EventId_PhaseCurOpenFlag}
};
static uint8 cabinetCtrInfoFunTableNum = sizeof(cabinetCtrInfoFunTable)/sizeof(CabinetCtrInfo);


/*
** 控制信息
*/
void UL_CtrInfo(uint16 rxlen,uint8*rx){
	SmCmd cmd = {true,CMD_ID_Ctr_Tx,0,0};
	uint16 itemId = 0;
	uint8 i = 0;
	bool itemIdLegality = false;
	uint8 itemDataLen = 0;
	uint8 offsetAddr = 0;
	uint8 txEventDataLen = 0;	

	/*
	** 清除事件回复项 -- 是否增加事件回复项连续帧处理?-->暂未处理
	*/
	memset((uint8*)&upperLayerPara.cabinetCtrInfoReply.dataItemlen,0x00,sizeof(CabinetCtrInfoReply));

	for(;rxlen >= sizeof(uint16);){
		/*
		** 事件ID
		*/
		itemIdLegality = false;
		memcpy((uint8*)&itemId,(uint8*)&rx[offsetAddr],sizeof(uint16));
		for(i=0;i<cabinetCtrInfoFunTableNum;i++){
			if(itemId == cabinetCtrInfoFunTable[i].itemId){
				itemIdLegality = true;
				/*填充接受事件*/
				memcpy((uint8*)&upperLayerPara.cabinetCtrInfoReply.tx[upperLayerPara.cabinetCtrInfoReply.dataItemlen],
							(uint8*)&itemId,sizeof(uint16));
				offsetAddr += sizeof(uint16);
				rxlen -= sizeof(uint16);
				upperLayerPara.cabinetCtrInfoReply.dataItemlen += sizeof(uint16);
				itemDataLen = rx[offsetAddr];
				offsetAddr += sizeof(uint8);
				rxlen -= sizeof(uint8);
				cabinetCtrInfoFunTable[i].handle(itemDataLen,(uint8*)&rx[offsetAddr]
					,(uint8*)&txEventDataLen,
					(uint8*)&upperLayerPara.cabinetCtrInfoReply.tx[upperLayerPara.cabinetCtrInfoReply.dataItemlen]);
				offsetAddr += itemDataLen;
				rxlen -= itemDataLen;
				upperLayerPara.cabinetCtrInfoReply.dataItemlen += txEventDataLen;
				break;
			}
		}
		/*
		**事件ID不合法
		*/
		if(itemIdLegality == false){
			break;
		}
		
	}

	setCmd(cmd);
}

/*
** BMS信息
*/
void UL_BmsInfo(uint16 rxlen,uint8*rx){
	SmCmd cmd = {true,CMD_ID_BmsInfo_Tx,0,0};

	/*
	** 仓门地址
	*/
	cmd.doorAddr = rx[0];

	setCmd(cmd);
}

/*
** BMS主要信息
*/
void UL_BmsMainInfo(uint16 rxlen,uint8*rx){
	SmCmd cmd = {true,CMD_ID_BatMainInfo_Tx,0,0};

	setCmd(cmd);
}

/*
** 域名
*/
void UL_Domain(uint16 rxlen,uint8*rx){
	ulReplyPara.domain.flag = true;
}

/*
** 执行函数表格
*/
static const UpperLayerFunTable upperLayerFunTabel[] = {
	{CMD_ID_Register_Rx,UL_RegisterInfo},
	{CMD_ID_Logout_Rx,UL_LogoutInfo},
	{CMD_ID_StateInfoChange_Rx,UL_StateInfoChange},
	{CMD_ID_StateInfo_Rx,UL_StateInfo},
	{CMD_ID_Ctr_Rx,UL_CtrInfo},
	{CMD_ID_BmsInfo_Rx,UL_BmsInfo},
	{CMD_ID_Domain_Rx,UL_Domain},
	{CMD_ID_BatMainInfo_Rx,UL_BmsMainInfo},
};
static uint8 upperLayerFunTabelNum = sizeof(upperLayerFunTabel)/sizeof(UpperLayerFunTable);

/*
**上层协议帧执行函数处理分析
*/
void UpperLayerExecuteFun(MsgHead msgHead,uint8*msgItem){
	uint8 i=0;
	for(i=0;i<upperLayerFunTabelNum;i++){
		if(msgHead.id == upperLayerFunTabel[i].id){
			upperLayerFunTabel[i].handle(msgHead.dataLen,(uint8*)msgItem);
			break;
		}
	}
}

/*
**解析数据帧多帧,粘帧,异常帧处理
*/
void ParseUpperLayerFrame(uint8* rxbuf,uint16* rxlen){
	/*
	**临时变量
	*/
	UpperLayerFrame ulFrameTmp = {0};
	/*
	**异常帧,粘帧处理变量
	*/
	uint8 *ptrbuf = rxbuf;	
	uint16 updataLen = *rxlen;
	uint16 frameLen = 0;
	uint16 msgLen=0;
	uint16 saveDataLen = *rxlen;

	for(;updataLen>=(uint16)(sizeof(ulFrameTmp.head)+sizeof(ulFrameTmp.msgFrameLen));){
		memcpy((uint8*)&frameLen,(uint8*)&ptrbuf[1] ,sizeof(uint16));
		if(ptrbuf[0] == UpperLayerFrame_Head&&ChkFrameDataLenIsOk(frameLen)){
			if(updataLen>=frameLen){
				if(CalcBcc((uint8 *)&ptrbuf[0], (uint16)frameLen-2) == ptrbuf[frameLen-2] 
					&& ptrbuf[frameLen-1] == UpperLayerFrame_End){
					memcpy((uint8*)&ulFrameTmp.head,(uint8*)&ptrbuf[0],sizeof(ulFrameTmp.head)+sizeof(ulFrameTmp.msgFrameLen)+sizeof(MsgHead));
					memcpy((uint8*)&msgLen,(uint8*)&ulFrameTmp.msgHead.dataLen,sizeof(uint16));
					ulFrameTmp.chkCode = ptrbuf[frameLen-2];
					if(ChkDeviceAddrIsOk((uint8)ulFrameTmp.msgHead.addr)){
						UpperLayerExecuteFun(ulFrameTmp.msgHead, 
							(uint8 *)(ptrbuf+sizeof(ulFrameTmp.head)+sizeof(ulFrameTmp.msgFrameLen)+sizeof(ulFrameTmp.msgHead)));
						memset((uint8*)&ulFrameTmp.head,0x00,sizeof(ulFrameTmp));
					}
					ptrbuf += frameLen;
					updataLen -= frameLen;
					frameLen = 0;					
				}
				else{
					ptrbuf++;
					updataLen--;
					frameLen = 0;
				}
			}
			else{
				break;
			}
		}
		else{
			ptrbuf++;
			updataLen--;
			frameLen = 0;
		}
	}
	memcpy((uint8*)&rxbuf[0],(uint8*)&ptrbuf[0],updataLen);
	*rxlen = updataLen;
	memset((uint8*)rxbuf+updataLen,0x00,saveDataLen-updataLen);
}

void SM_UpperLayerParse_Task(void* p_arg){
	/*修改:*/
	uint8 ucRxBuf[sizeof(uint16)*(1024+sizeof(UpgradeProFrame))/*1024*/];
	uint8 *rxptr;
	uint16 ucLen = 0;
	uint16 ucRecvLen = 0;
	/*更新上层协议硬件接口标号*/
	uint8 interface_Label = get_Interface().label;

	/*
	** 初始化上层协议变量
	*/
	init_UpperLayerPara();
	
	Sleep(4000);

	for(;;){
		ucRecvLen = 0;
		BSP_SmRecv(BSP_ComType_USART,interface_Label,ucRxBuf + ucLen,&ucRecvLen);
		ucLen += ucRecvLen;
		if(ucLen >( sizeof(ucRxBuf)))
		{
			ucLen = 0;
		}
		
		if(ucRecvLen == 0 && ucLen != 0)
		{
			rxptr = ucRxBuf;
			ucRecvLen = ucLen;
			ParseUpperLayerFrame((uint8 *)rxptr, (uint16 *)&ucRecvLen);
			ucLen = ucRecvLen;
		}	

		/*
		** CAN1 Tx Task
		*/
		CAN1_TxDataTask();

		/*
		** update 上层相关标志
		*/
		update_UpperFieldRef();

		/*
		** 控制板执行BootLoader策略:
		**		1.接收远程升级指令
		**		2.等待执行回复,800ms之后重启主控进入BootLoader
		*/
		enterCtrBootLoader();

		/*------------------------------------硬件接口实时检测-------------------------------------------------------*/
		/*
		** chk Interface Sw Logic
		*/
		 chk_Interface_SwLogic();		
		/*-----------------------------------------------------------------------------------------------------------*/
		/*
		** 20210518--检测是否存在升级原因未读取
		*/
		runtimeChk_UpgrResonFlag();
		
		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();

		Sleep(10);
	}
}

/*
** 提供系统软件创建上层协议解析接口函数
*/
void UpperLayerParse_Init(void){
	Thread_create(SM_UpperLayerParse_Task, 0,
		(int32 *)&upperLayerParse_TaskStk[1024-1], (int8)UpperLayerParseTask_Prio);
}

