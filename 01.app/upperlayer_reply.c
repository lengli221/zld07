#include "includes.h"

static int32 upperLayerReply_TaskStk[512] = {0};

/*
** 上层协议之使用extern调用其他文件变量
*/
extern UpperLayerPara upperLayerPara;
/*
** 上层协议之使用指针调用其他文件变量
*/
LowerLayerParam* ul_LLParse = null;
SysParam* ulr_SysPara = null;

/*
** 上层协议之回复参数变量
*/
UpperLayerReplyPara ulReplyPara;

/*
** 上层协议之发送命令
*/
SmCmd smCmd;

/*
** 上层协议之回复参数变量接口函数之外部函数调用
*/
UpperLayerReplyPara* getULReplyPara(void){
	return(&ulReplyPara);
}

/*
** 上层协议之回复变量初始化
*/
static void init_ULReplyPara(void){
	memset((uint8*)&ulReplyPara.domain.flag,0x00,sizeof(UpperLayerReplyPara));
	ul_LLParse = getLowerLayerParaPtr();
	ulr_SysPara = get_SysParaPtr();
}

/*
** 提供解析文件调用接口函数
** 1.设置命令参数
** 2.查询命令参数
*/
void setCmd(SmCmd cmd){
	uint8 cnt=0;
	
	while(smCmd.id != 0 && cnt < 100){
		cnt++;
		Sleep(10);
	}
	memcpy((uint8*)&smCmd.refresh,(uint8*)&cmd.refresh,sizeof(SmCmd));
}

uint8 getCmd(void){
	return(smCmd.id);
} 

/*
** 上层协议之回复注册信息
*/
void ULReply_Register(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;
	uint32 timeStamp = 0;/*单位:ms*/
	
	/*
	** 设备ID,硬件版本号,软件版本号,主控型号,柜子型号,注册时间
	*/
	/*
	** 更新设备ID参数
	*/
	memcpy((uint8*)&upperLayerPara.sysSet.id[0],(uint8*)&ulr_SysPara->deviceId.show[0],16);
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.id[0],16);
	len += 16;
	/*控制板硬件版本号*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.ctrHareVer[0],4);
	len += 4;
	/*分控板硬件版本号*/
	memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[0].batDoorSysPara.comHareVer[0],4);
	len += 4;
	/*控制板软件版本号*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.ctrSoftVer[0],4);
	len += 4;
	/*分控板软件版本号*/
	memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[0].batDoorSysPara.comSoftVer[0],4);
	len += 4;

	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.ctrModel[0],6);
	len += 6;
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.fireId[0],10);
	len += 10;

	/*
	** 时间戳
	*/
	timeStamp = GetCurSecond()*1000;/*单位:ms*/
	memcpy((uint8*)&item[len],(uint8*)&timeStamp,sizeof(uint32));
	len += sizeof(uint32);

	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** 上层协议之回复注销信息
*/
void ULReply_Logout(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;
	/*
	** 设备ID
	*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysSet.id[0],16);
	len += 16;

	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** 上层协议之状态信息变更
*/
void ULReply_StateInfoChange(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;
	
	/*
	** 数据项
	*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.stateInfoChange.sysLogic.batOnline,sizeof(StateInfoChange));
	len += sizeof(StateInfoChange);

	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** 上层协议之回复状态信息
*/
void ULReply_StateInfo(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;
	DoorEUResonNoRead resonTemp = {0};

	/*
	** 数据项 
	*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.stateInfo.batOnlineCtrAC,sizeof(StateInfo));
	len += sizeof(StateInfo);
	/*
	** 充电仓环温
	*/
	memcpy((uint8*)&item[len],(uint8*)&upperLayerPara.sysFire.chgDoorTemp,sizeof(uint16));
	len += sizeof(uint16);
	/*
	** 电池包固件数
	*/
	memcpy((uint8*)&item[len],(uint8*)&ulr_SysPara->batFireSize,sizeof(uint16));
	len += sizeof(uint16);
	/*
	** 充电原因标志位
	*/
	resonTemp = get_DoorEUResonNoRead();
	memcpy((uint8*)&item[len],(uint8*)&resonTemp.bat,sizeof(DoorEUResonNoRead));
	len += sizeof(DoorEUResonNoRead);
	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** 上层协议之回复控制信息
*/
void ULReply_CtrInfo(UpperLayerFrame * ulFreame,uint8* item,uint8 addr){
	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = upperLayerPara.cabinetCtrInfoReply.dataItemlen;
	/*
	** 数据项
	*/
	memcpy((uint8*)&item[0],(uint8*)&upperLayerPara.cabinetCtrInfoReply.tx[0],ulFreame->msgHead.dataLen);
}

/*
** BMS信息
*/
void ULReply_BmsInfo(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;

	/*
	** 仓门地址
	*/
	item[len] = addr;
	len += sizeof(uint8);
	if(addr >= SysCtr_AllDoorNum){/*仓门地址不合法*/
		item[len] = 0x02;
		len += sizeof(uint8);
	}else{/*仓门地址合法*/
		item[len] = 0x01;
		len += sizeof(uint8);
		/*
		** 电池是否在线
		*/
		if(ul_LLParse->batDoor[addr].batDoorStateInfo.bits.batOnline == false){/*电池离线*/
			item[len] = 0x02;
			len += sizeof(uint8);
		}else{/*电池在线*/
			item[len] = 0x01;
			len += sizeof(uint8);
			/*
			** 电池充电时长
 			*/
 			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].batDoorSysPara.chgTime,sizeof(uint16));
			len += sizeof(uint16);
			/*
			** 电池信息
			*/
			/*数据项:实时电压,实时电流,SOC,厂商代码*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.realVol,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.realCur,sizeof(uint16));
			len += sizeof(uint16);
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.soc;
			len += sizeof(uint8);
			 item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.firmCode;
			len += sizeof(uint8);	
			/*数据项:保护状态 故障状态 SOH SOP*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.pState.flag,sizeof(uint16));
			len += sizeof(uint16);
			 item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.faultState.flag;
			len += sizeof(uint8);		
			 item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.soh;
			len += sizeof(uint8);	
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.sop,sizeof(uint16));
			len += sizeof(uint16);	
			/*数据项:单节电池电压 16*2*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.singleBatVol[0],16*sizeof(uint16));
			len += 16*sizeof(uint16);
			/*数据项:电池ID 16*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batID[0],16);
			len += 16;
			/*数据项:充电次数,过放次数,短路次数*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.chgNum,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.disChgNum,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.shortNum,sizeof(uint16));
			len += sizeof(uint16);
			/*循环次数 剩余容量 设计容量*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.circNum,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.surplusCap,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.designCap,sizeof(uint16));
			len += sizeof(uint16);	
			/*数据项:电池最高温度,电池最低温度,MOS最高温度,PCB温度,历史电池最高温度 工作状态*/
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batMaxTemp;
			len += sizeof(uint8);		
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batMinTemp;
			len += sizeof(uint8);	
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.mosMaxTemp;
			len += sizeof(uint8);		
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.pcbTemp;
			len += sizeof(uint8);		
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.hisMaxBatTemp;
			len += sizeof(uint8);	
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.workState;
			len += sizeof(uint8);
			/*数据项:硬件版本,软件版本,BOOT版本号,电池型号*/	
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.hardVer,sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.softVer,sizeof(uint16));
			len += sizeof(uint16);
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.bootVer;
			len += sizeof(uint8);	
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batType;
			len += sizeof(uint8);	
			/*数据项:电池工作模式,电池充电模式,MOS状态*/
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batWorkMode;
			len += sizeof(uint8);		
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batChgMode;
			len += sizeof(uint8);	
			item[len] = ul_LLParse->batDoor[addr].bmsInfoMeiTuan.mosState.flag;
			len += sizeof(uint8);	
			/*-------------------美团:美团助力车-主控_电池通信协议1.15.pdf----------------------*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[addr].bmsInfoMeiTuan.batMTV115.materialChangeR,sizeof(BatMeiTuanV115));
			len += sizeof(BatMeiTuanV115);
			/*----------------------------------------------------------------------------------*/
		}
	}
	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** 域名
*/
void ULReply_Domain(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint16 len = 0;
	uint8 legalLen = 0;
	SysDomain sysDomain = get_SysDomain();

	/*
	** 数据项: ZLD域名长度 ZLD域名 美团域名长度 美团域名 -- 增加设备ID
	*/
	legalLen = getDomainIsLegalLen((uint8 *)&sysDomain.local[0]);
	item[len] = legalLen;
	len += sizeof(uint8);
	memcpy((uint8*)&item[len],(uint8*)&sysDomain.local[0],legalLen);
	len += legalLen;

	legalLen = getDomainIsLegalLen((uint8 *)&sysDomain.remote[0]);
	item[len] = legalLen;
	len += sizeof(uint8);
	memcpy((uint8*)&item[len],(uint8*)&sysDomain.remote[0],legalLen);
	len += legalLen;	

	/*
	** 增加设备ID
	*/
	memcpy((uint8*)&item[len],(uint8*)&ulr_SysPara->deviceId.show[0],16);
	len += 16;

	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
** Bms主要信息
*/
void ULReply_BmsMainInfo(UpperLayerFrame *ulFreame,uint8* item,uint8 addr){
	uint8 i = 0;
	uint16 len = 0;
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		if(ul_LLParse->batDoor[i].batDoorStateInfo.bits.batOnline == true){/*电池在线*/
			item[len] = ul_LLParse->batDoor[i].bmsInfoMeiTuan.soc;/*SOC值*/
			len += sizeof(uint8);
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[i].batDoorSysPara.insertTime,sizeof(uint16));/*插入时间*/
			len += sizeof(uint16);
			/*充电时间--20210127*/
			memcpy((uint8*)&item[len],(uint8*)&ul_LLParse->batDoor[i].batDoorSysPara.chgTime,sizeof(uint16));
			len += sizeof(uint16);
		}else{
			memset((uint8*)&item[len],0x00,sizeof(uint8) + sizeof(uint16) + sizeof(uint16));
			len += (sizeof(uint8) + sizeof(uint16) + sizeof(uint16));
		}
	}

	/*
	** 数据项长度
	*/
	ulFreame->msgHead.dataLen = len;
}

/*
**上层协议之回复接口函数表格
*/
static const UpperLayerReplyFunTable upperLayerReplyFunTable[] = {
	{CMD_ID_Register_Tx,ULReply_Register},
	{CMD_ID_Logout_Tx,ULReply_Logout},
	{CMD_ID_StateInfoChange_Tx,ULReply_StateInfoChange},
	{CMD_ID_StateInfo_Tx,ULReply_StateInfo},
	{CMD_ID_Ctr_Tx,ULReply_CtrInfo},
	{CMD_ID_BmsInfo_Tx,ULReply_BmsInfo},
	{CMD_ID_Domain_Tx,ULReply_Domain},
	{CMD_ID_BatMainInfo_Tx,ULReply_BmsMainInfo},
};
static uint8 upperLayerReplyFunTableNum = sizeof(upperLayerReplyFunTable)/sizeof(UpperLayerReplyFunTable);

/*
** 注册信息控制逻辑
*/
void ULReply_RegisterCtrLogic(void){
	SmCmd cmd = {true,CMD_ID_Register_Tx,0,0};
	static uint32 itick = 0;

	if(ulReplyPara.registerLogic.flag == true){
		if(TickOut((uint32 *)&itick, 5000) == true){
			TickOut((uint32 *)&itick, 0);
			setCmd(cmd);
		}
	}
}

/*
** 注销信息控制逻辑
*/
void ULReply_LogoutCtrLogic(void){
	SmCmd cmd = {true,CMD_ID_Logout_Tx,0,0};
	static uint32 itick = 0;

	if(ulReplyPara.logoutLogic.flag == true){
		if(TickOut((uint32 *)&itick, 5000) == true){
			TickOut((uint32 *)&itick, 0);
			setCmd(cmd);
		}
	}
}

/*
** 更新整柜状态信息
*/
static uint32 stateTick= 0; 
static uint32 dealHardErrTick = 0;/*处理硬件抖动,主动上报数据不断触发*/
void ULReply_UpdateStateInfoLogic(void){
	/*
	** static uint32 itick = OSTimeGet();
	** static uint32 dealHardErr = OSTimeGet();
	** 报错: function call is not allowed in a constant expression 
	** static uint32 itick = OSTime;
	** 报错: expression must have a constant value
	*/
	SmCmd cmd = {true,CMD_ID_StateInfoChange_Tx,0,0};
	static bool resetFlag = true;
	static StateInfoChange stateInfoChange = {0};

	if(resetFlag == true){
		resetFlag = false;
		stateTick = OSTimeGet();
		dealHardErrTick = OSTimeGet();
	}

	/*上电等待20S之后,等待状态稳定之后在变更状态信息*/
	if(TickOut((uint32 *)&stateTick, 20000) == true){
		if(memcmp((uint8*)&stateInfoChange.sysLogic.batOnline,
			(uint8*)&upperLayerPara.stateInfoChange.sysLogic.batOnline,sizeof(StateInfoChange)) != 0
			&& TickOut((uint32 *)&dealHardErrTick, 1000) == true){
			TickOut((uint32 *)&dealHardErrTick, 0);
			memcpy((uint8*)&stateInfoChange.sysLogic.batOnline,
				(uint8*)&upperLayerPara.stateInfoChange.sysLogic.batOnline,sizeof(StateInfoChange));
			setCmd(cmd);
		}
	}
}

/*
** 从设备主动上报域名信息
*/
void powerOnUpDomainInfo(void){
	SmCmd cmd = {true,CMD_ID_Domain_Tx,0,0};
	static uint32 itick = 0;
	static bool powerOnFlag = false;

	if(ulReplyPara.domain.flag == false){/*主动上报域名,直至服务器获取域名*/
		if(TickOut((uint32 *)&itick, 6000) == true
			|| powerOnFlag == false){
			TickOut((uint32 *)&itick, 0);
			setCmd(cmd);
			powerOnFlag = true;
		}
	}
}

/*
** 上层协议之封装回复帧接口函数
*/
void UL_PackectReplyFun(UpperLayerFrame *ulFreame,uint8* item,uint8*tx){
	uint8 len = 0;

	/*帧头*/
	tx[len] = UpperLayerFrame_Head;
	len += sizeof(uint8);

	/*帧数据长度计算逻辑*/
	ulFreame->msgFrameLen = sizeof(UpperLayerFrame) + ulFreame->msgHead.dataLen;

	/*帧长度*/
	memcpy((uint8*)&tx[len],(uint8*)&ulFreame->msgFrameLen,sizeof(uint16));
	len += sizeof(uint16);

	/*消息头*/
	memcpy((uint8*)&tx[len],(uint8*)&ulFreame->msgHead.id,sizeof(MsgHead));
	len += sizeof(MsgHead);

	/*消息内容项*/
	memcpy((uint8*)&tx[len],(uint8*)&item[0],ulFreame->msgHead.dataLen);
	len += ulFreame->msgHead.dataLen;

	/*校验*/
	tx[len] = CalcBcc((uint8 *)&tx[0], len);
	len += sizeof(uint8);

	/*帧尾*/
	tx[len] = UpperLayerFrame_End;
}

void SM_UpperLayerReply_Task(void* p_arg){
	uint8 txItem[256] = {0};
	uint8 txbuf[256] = {0};
	UpperLayerFrame ulFrameTmp = {0};
	uint8 i = 0;
	uint8 interface_Label = get_Interface().label;

	/*
	** 上层协议之回复变量初始化
	*/
	init_ULReplyPara();

	Sleep(4000);
	
	for(;;){		
		if(smCmd.refresh == true){
			for(i=0;i<upperLayerReplyFunTableNum;i++){
				if(smCmd.id == upperLayerReplyFunTable[i].id){
					ulFrameTmp.msgHead.id = upperLayerReplyFunTable[i].id;
					ulFrameTmp.msgHead.addr = smCmd.deviceAddr;/*设备地址参阅协议总线部分说明*/
					upperLayerReplyFunTable[i].handle((UpperLayerFrame*)&ulFrameTmp,
						(uint8*)&txItem[0],smCmd.doorAddr);
					UL_PackectReplyFun((UpperLayerFrame *)&ulFrameTmp, 
						(uint8 *)&txItem[0], (uint8 *)&txbuf[0]);
					BSP_SmSend(BSP_ComType_USART, interface_Label, 
						(void *)&txbuf[0], (uint16 *)&ulFrameTmp.msgFrameLen);
					memset((uint8*)&smCmd.refresh,0x00,sizeof(SmCmd));
					break;
				}
			}			
		}
		
		/*
		** 从设备主动上报域名信息
		*/
		#if CtrHardware == CtrHardware_Dwin
		powerOnUpDomainInfo();
		#endif

		#if CtrHardware == CtrHardware_Andriod
			ulReplyPara.domain.flag = true;/*安卓默认已获取域名,采用安卓屏,域名从安卓获取*/
		#endif

		if(ulReplyPara.domain.flag == true){/*主设备获取域名成功,再上传其他信息*/
			#if CtrHardware == CtrHardware_Dwin
			/*
			** 注册信息控制逻辑
			*/
			ULReply_RegisterCtrLogic();
			/*
			** 注销信息控制逻辑
			*/
			ULReply_LogoutCtrLogic();	
			#endif
			/*
			** 更新整柜状态信息
			*/
			ULReply_UpdateStateInfoLogic();
		}

		/*
		** CAN1 Tx Task
		*/
		CAN1_TxDataTask();

		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
			
		Sleep(10);
	}
}


/*
**提供系统软件创建上层协议回复接口函数
*/
void UpperLayerReply_Init(void){
	Thread_create(SM_UpperLayerReply_Task, 0, 
		(int32 *)&upperLayerReply_TaskStk[512-1], UpperLayerReplyTask_Prio);
}

