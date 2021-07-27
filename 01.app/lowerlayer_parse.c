#include "includes.h"

static int32 lowerLayerParse_TaskStk[512] = {0};

/*
** 使用指针方式调用其他文件变量
*/
UpperLayerPara* llp_ULP = null;

/*
** 下层协议参数变量定义
*/
LowerLayerParam llParam;

/*
** 下层协议之参数接口提供调用
*/
LowerLayerParam* getLowerLayerParaPtr(void){
	return(&llParam);
}

/*
** 下层协议之参数初始化
*/
static void LowerLayerParam_Init(void){
	memset((uint8*)&llParam.compatibility[0].fireCode,0x00,sizeof(LowerLayerParam));
	llp_ULP = getUpperLayerPara();
}

/*
** 下层协议之电池舱状态信息字段之实时电压数据更新
*/
#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
void FillBatDoorStateInfo_MeiTuan(uint8 fireCode,uint8* item,uint8 addr){
	uint8 len = 0;
	
	switch(fireCode){	
		case 0x01:/*博力威*/
		case 0x02:/*欣旺达*/
		case 0x03:/*飞毛腿*/
		case 0x04:/*南都*/
		case 0x05:/*新日动力*/
		case 0x06:/*星恒*/
		case 0x07:/*ATL*/
		case 0x08:/*CATL*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);

			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);

			llParam.batDoor[addr].bmsInfoMeiTuan.soc = item[len];
			len += sizeof(uint8);
			break;
		default:
			
			break;
	}
}
#endif

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
void FillBatDoorStateInfo_PineCone(uint8 fireCode,uint8* item,uint8 addr){
	uint8 len = 0;

	switch(fireCode){
		case 0x81:/*星恒*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batTotalVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);

			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);

			llParam.batDoor[addr].bmsInfoPineCone.soh = item[len];
			len += sizeof(uint8);
			break;
		default:

			break;
	}
}
#endif

/*
** 判定异常仓依据(0b0000 0011 1100 1110)
*/
#define AbnDoorNumBits						(uint16)0x03CE

/*
** 根据状态信息更新舱门数(空仓数,充电仓数,满仓数,异常挂起数,总仓数)
*/
static void updateDoorNum(void){
	DoorTypeNum doorTypeTemp = {0};
	uint8 i = 0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*
		** 状态位的故障位优先级最高
		*/
		if((llParam.batDoor[i].batDoorStateInfo.flag & AbnDoorNumBits) != 0
			|| llParam.comBoardChk[i].comAbn == true){
			doorTypeTemp.abn++;
		}else{/*无故障*/
			if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == false){/*电池不在线*/
				doorTypeTemp.idle++;
			}else{/*电池在线*/
				if(llParam.batDoor[i].bmsInfoMeiTuan.soc >= get_SocLimit()){
					doorTypeTemp.full++;
				}				
			}
		}
	}

	doorTypeTemp.ing = SysCtr_AllDoorNum - (doorTypeTemp.abn + doorTypeTemp.idle + doorTypeTemp.full);
	doorTypeTemp.total = SysCtr_AllDoorNum;

	/*
	** 开关拉下,更新检测仓门统计信息方案
	*/
	if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){
		memset((uint8*)&doorTypeTemp.idle,0,sizeof(DoorTypeNum));
		doorTypeTemp.abn = SysCtr_AllDoorNum;
		doorTypeTemp.total = SysCtr_AllDoorNum;
	}

	/*
	** 更新上层协议舱门数参数
	*/
	llp_ULP->stateInfoChange.doorTypeNum = doorTypeTemp;
}

/*
** 判定通讯板是否失联,ID是否重复,变更上层字段信息
*/
void updateComIsAbn(void){
	uint8 i = 0;
	bool ret = false;
	static uint32 closeACtTick = 0;/*断AC之后重新计算分控失联时间标志--上报标志*/

	if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){/*AC断开*/
		TickOut((uint32 *)&closeACtTick, 0);
	}

	for(i = 0;i < SysCtr_AllDoorNum;i++){
		if(llParam.comBoardChk[i].comAbn == true){/*通讯板失联*/
			/*硬件开关拉下屏蔽通讯板失联检测*/
			if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == false){
				if(TickOut((uint32 *)&closeACtTick, 60000) == true){
					llp_ULP->stateInfoChange.sysLogic.comIsOnline |= (DoorNumDefine)((DoorNumDefine)1<<i);
				}	
			}else{
				llp_ULP->stateInfoChange.sysLogic.comIsOnline &= (DoorNumDefine)~((DoorNumDefine)1<<i);
			}
		}else{
			llp_ULP->stateInfoChange.sysLogic.comIsOnline &= (DoorNumDefine)~((DoorNumDefine)1<<i);
		}

		if(llParam.batDoor[i].batDoorStateInfo.bits.comIdRep == true){
			ret = true;
		}
	}

	/*
	** 更新系统状态位
	*/
	if(llp_ULP->stateInfoChange.sysLogic.comIsOnline != 0){
		llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comErr = true;
	}else{
		llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comErr = false;
	}	

	llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comIdRep = ret;
}

/*
** 根据下层->电池舱状态信息->变更上层->整柜状态信息变更
*/
extern void LLReply_ChkBatStateInfo(uint8 cmd,uint8* len,uint8* item);
void stateInfoSwitch(uint8 addr){
	volatile uint8 temp = 0;
	uint8 tx[8] = {0};
	uint8 len = 0;
	
	/*
	** 电池状态信息:
	**			Bit0:置1--电池在线 Bit1:置1--电池舱充电器过温 Bit2:置1--电池过温 Bit3:置1--充电器故障(电池在线,持续1min无电流故障)
	**			Bit4:置1--电池在线开启AC Bit5:置1--远程断AC Bit6:置1--系统故障断充电器AC Bit7:BMS通讯板ID重复 
	**			Bit8:置1--电池故障 Bit9:置1--BMS板烧毁,电池故障 Bit10:置1--电池升级是否完成 Bit11:置1--电池升级是否成功
	*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == true){
		llp_ULP->stateInfoChange.sysLogic.batOnline |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batOnline &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
		/*电池不在线--清除所有电池相关信息*/
		memset((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realVol,0,sizeof(BmsInfoMeiTuan));
	}
	
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batDoorChargerOT == true){
		llp_ULP->stateInfoChange.sysLogic.chgOverTemp |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.chgOverTemp &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	if(llParam.batDoor[addr].batDoorStateInfo.bits.batOT == true){
		llp_ULP->stateInfoChange.sysLogic.batIsErr |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batIsErr &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	if(llParam.batDoor[addr].batDoorStateInfo.bits.chargerErr == true){
		llp_ULP->stateInfo.batOnline_1minNoCur |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfo.batOnline_1minNoCur &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	if(llParam.batDoor[addr].batDoorStateInfo.bits.batOnlineOpenAC == true){
		llp_ULP->stateInfo.batOnlineCtrAC |=(DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfo.batOnlineCtrAC &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	if(llParam.batDoor[addr].batDoorStateInfo.bits.sysErrCloseAC == true){
		llp_ULP->stateInfo.sysErrCtrAC |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfo.sysErrCtrAC &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	if(llParam.batDoor[addr].batDoorStateInfo.bits.comIdRep == true){
		llp_ULP->stateInfo.comIdRep |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfo.comIdRep &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*电池故障*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batTrans == true){
		llp_ULP->stateInfoChange.sysLogic.batFault |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batFault &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*电池通讯故障*/
	if( llParam.batDoor[addr].batDoorStateInfo.bits.bmsErr == true){
		llp_ULP->stateInfoChange.sysLogic.batComErr |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batComErr &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*分控故障*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.comIsFault == true){
		llp_ULP->stateInfoChange.sysLogic.comFault |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.comFault &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}	

	/*充电异常*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.chargingErr == true){
		llp_ULP->stateInfoChange.sysLogic.chargingErr |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.chargingErr &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*电池低温*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batLowTemp == true){
		llp_ULP->stateInfoChange.sysLogic.batLowTemp |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batLowTemp &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	
	if(llParam.batDoor[addr].batDoorStateInfo.bits.oneErrRecover == true){
		LLReply_ChkBatStateInfo(0xFE,(uint8 *)&len, (uint8 *)&tx[0]);
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], addr, LL_FunId_BatDoorState);		
		llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comRecoverFlag = true;
	}
}

/*
** 下层协议之电池舱状态解析
*/
void LLParse_BatDoorState(uint8 rxlen,uint8* item,uint8 addr){
	uint8 len = 0;
	
	/*
	** 更新数据--通讯小板一旦ID重复不再更新相应数据--判定通讯异常
	*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.comIdRep == false){/*通讯小板未重复*/
		/*
		** 数据项 电池舱状态信息 电池厂商代码 实时电压(美团)/电池总电压(快松果) 实时电流 SOC(美团)/SOH(快松果)
		*/
		/*
		** 电池状态信息:
		**			Bit0:置1--电池在线 Bit1:置1--电池舱充电器过温 Bit2:置1--电池过温 Bit3:置1--充电器故障(电池在线,持续1min无电流故障)
		**			Bit4:置1--电池在线开启AC Bit5:置1--远程断AC Bit6:置1--系统故障断充电器AC Bit7:BMS通讯板ID重复 
		**			Bit8:置1--电池反接 Bit9:置1--BMS板烧毁,电池故障 Bit10:置1--电池升级是否完成 Bit11:置1--电池升级是否成功
		*/
		
		memcpy((uint8*)&llParam.batDoor[addr].batDoorStateInfo.flag,(uint8*)&item[len],sizeof(uint16));
		len += sizeof(uint16);
		/*
		** 电池厂商代码 字段后(包含)所有数据仅在电池在线更新
		*/
		if(llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == true){
			llParam.compatibility[addr].fireCode = item[len];
			len += sizeof(uint8);

			#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
				FillBatDoorStateInfo_MeiTuan(llParam.compatibility[addr].fireCode,(uint8 *)&item[len], addr);
			#endif

			#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
				FillBatDoorStateInfo_PineCone(llParam.compatibility[addr].fireCode,(uint8 *)&item[len], addr);
			#endif
		}

		/*
		** 处理升级相关标志处理
		*/
		if(llp_ULP->stateInfoChange.sysLogic.batUpgr & (DoorNumDefine)((DoorNumDefine)0x01<<addr)){
			if(llParam.batDoor[addr].batDoorStateInfo.bits.batUpgrIsFinsh == true
				|| llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == false/*电池不在线*/){
				llp_ULP->stateInfoChange.sysLogic.batUpgr &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
				if(llParam.batDoor[addr].batDoorStateInfo.bits.batUpgrIsOk == true){
					llp_ULP->stateInfoChange.sysLogic.batUpgrIsOk |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
				}
			}
		}
		/*
		** 根据下层->电池舱状态信息->变更上层->整柜状态信息变更
		*/
		stateInfoSwitch(addr);
	}
}

/*
** 新增:20210115--张工测试需求 分控请求升级 -- 在充电器过温,电池过温(整柜中任意仓存在--拒绝升级)
*/
bool chk_IsExitBatChargerOT(void){
	bool flag = false;

	if(llp_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*充电器过温*/
		|| llp_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*电池过温*/){
		flag = true;
	}

	return flag;
}

/*
** 电池升级状态
*/
extern DoorNumDefine hmiUpgrIsFinshFlag;
void LLParse_UpgrState(uint8 rxlen,uint8* item,uint8 addr){
	static uint8 frame[SysCtr_AllDoorNum] = {0};
	uint8 len = 0;
	uint8 frameLabel = 0;
	uint8 txlen = 1;
	uint8 tx[8] = {0};
	
	/*
	** 帧标识 帧数据长度暂时未使用
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);
	
	switch(frameLabel){/*帧标识*/
		case 0x01:/*厂商代码,电池型号,电池原软件版本号,电池待升级软件版本号*/
			
			break;
		case 0x02:/*硬件版本号,电池ID*/
			
			break;
		case 0x03:
			
			break;
		case 0x04:
			
			break;
	}

	/*
	** 检测帧是否接受完整
	*/
	frame[addr] |= 0x01<<(frameLabel - 1);
	if(frame[addr] == 0x0F){
		frame[addr] = 0;
		/*整仓存在电池过温,充电器过温 -- 拒绝升级 0x02*/
		if(chk_IsExitBatChargerOT() == true){
			tx[0] = 0x02;/*拒绝升级*/
		}else{
			llp_ULP->stateInfoChange.sysLogic.batUpgr |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
			llp_ULP->stateInfoChange.sysLogic.batUpgrIsOk &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
			hmiUpgrIsFinshFlag &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
			tx[0] = 0x01;/*允许升级*/
		}
		/*
		** 回复分控板
		*/
		CAN_TransmitAnalysis(CAN_Port_1, txlen, (uint8 *)&tx[0], addr, LL_FunId_BatUpgrState);
	}
}

/*
** 充电仓系统信息
*/
void LLPrse_ChgSysInfo(uint8 rxlen,uint8* item,uint8 addr){
	uint8 len = 0;
	/*
	** 电池状态信息
	*/
	/*llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = item[len]&0x01;*/
	if((item[len]&0x01) == 0x01){/*电池在线*/
		llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = true;
	}
	len += sizeof(uint8);

	/*
	** 电池接入时SOC 电池充电时长 电池接入时长
	*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == true){
		llParam.batDoor[addr].batDoorSysPara.chgBefSoc = item[len];	
		len += sizeof(uint8);
		memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.chgTime,(uint8*)&item[len],sizeof(uint16));	
		len += sizeof(uint16);
		memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.insertTime,(uint8*)&item[len],sizeof(uint16));
		len += sizeof(uint16);
	}else{
		memset((uint8*)&llParam.batDoor[addr].batDoorSysPara.chgBefSoc,0x00,sizeof(uint8)+sizeof(uint16)+sizeof(uint16));
		len += sizeof(uint8)+sizeof(uint16)+sizeof(uint16);
	}

	/*
	** 更新电池插入时间过长标志
	*/
	if(llParam.batDoor[addr].batDoorSysPara.insertTime >= get_ChgTimeLimit()){
		llp_ULP->stateInfoChange.sysLogic.batChgOTime |= (DoorNumDefine)((DoorNumDefine)1<<addr); 
	}else{
		llp_ULP->stateInfoChange.sysLogic.batChgOTime &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}
	
	/*
	** 充电器温度
	*/
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.chargerTemp,(uint8*)&item[len],sizeof(uint16));
	len += sizeof(uint16);
}

/*
** 下层协议之BMS信息之填充BMS字段信息
*/
#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
void FillBatInfo_Data_MetTuan(uint8* item,uint8 frameLabel,uint8 addr){
	uint8 len = 0;
	uint8 dataLen  = 0;
	
	/*
	** 优化电池在线逻辑--接收电池上报信息即认可电池在线
	** 明确:是否存在查询时间差不一致导致在线切换异常,通讯是否处理好
	*/
	llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = true;
	
	switch(frameLabel){
		case 0x01:/*数据项:实时电压,实时电流,SOC,厂商代码*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			llParam.batDoor[addr].bmsInfoMeiTuan.soc = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoMeiTuan.firmCode = item[len];
			len += sizeof(uint8);		
			/*更新:厂商代码*/
			llParam.compatibility[addr].fireCode = llParam.batDoor[addr].bmsInfoMeiTuan.firmCode;
			break;
		case 0x02:/*数据项:保护状态 故障状态 SOH SOP*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.pState.flag,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			llParam.batDoor[addr].bmsInfoMeiTuan.faultState.flag = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoMeiTuan.soh = item[len];
			len += sizeof(uint8);	
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.sop,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			break;
		case 0x03:/*数据项:单节电池电压 16*2*/
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
			dataLen = frameLabel != (uint8)0x08?0x06:0x02;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.singleBatVol[(frameLabel-0x03)*(0x06/sizeof(uint16))],
				(uint8*)&item[len],dataLen);
			len += dataLen;
			break;
		case 0x09:/*数据项:电池ID 16*/
		case 0x0A:	
		case 0x0B:
			dataLen = frameLabel != (uint8)0x0B?0x06:0x04;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batID[(frameLabel-0x09)*0x06],
				(uint8*)&item[len],dataLen);
			len += dataLen;
			break;
		case 0x0C:/*数据项:充电次数,过放次数,短路次数*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.chgNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.disChgNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.shortNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);			
			break;
		case 0x0D:/*循环次数 剩余容量 设计容量*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.circNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.surplusCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.designCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);			
			break;
		case 0x0E:/*数据项:电池最高温度,电池最低温度,MOS最高温度,PCB温度,历史电池最高温度 工作状态*/
			llParam.batDoor[addr].bmsInfoMeiTuan.batMaxTemp = item[len];
			len += sizeof(uint8);		
			llParam.batDoor[addr].bmsInfoMeiTuan.batMinTemp = item[len];
			len += sizeof(uint8);	
			llParam.batDoor[addr].bmsInfoMeiTuan.mosMaxTemp = item[len];
			len += sizeof(uint8);		
			llParam.batDoor[addr].bmsInfoMeiTuan.pcbTemp = item[len];
			len += sizeof(uint8);		
			llParam.batDoor[addr].bmsInfoMeiTuan.hisMaxBatTemp = item[len];
			len += sizeof(uint8);	
			llParam.batDoor[addr].bmsInfoMeiTuan.workState = item[len];
			len += sizeof(uint8);
			break;
		case 0x0F:/*数据项:硬件版本,软件版本,BOOT版本号,电池型号*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.hardVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.softVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);	
			llParam.batDoor[addr].bmsInfoMeiTuan.bootVer = item[len];
			len += sizeof(uint8);		
			llParam.batDoor[addr].bmsInfoMeiTuan.batType = item[len];
			len += sizeof(uint8);				
			break;
		case 0x10:/*数据项:电池工作模式,电池充电模式,MOS状态*/
			llParam.batDoor[addr].bmsInfoMeiTuan.batWorkMode = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoMeiTuan.batChgMode = item[len];
			len += sizeof(uint8);	
			llParam.batDoor[addr].bmsInfoMeiTuan.mosState.flag = item[len];
			len += sizeof(uint8);				
			break;
		default:
			break;
	}
}


void FillBatInfo_MetTuan(uint8 fireCode,uint8* item,uint8 frameLabel,uint8 addr){
	switch(fireCode){
		case 0x01:/*博力威*/
		case 0x02:/*欣旺达*/
		case 0x03:/*飞毛腿*/
		case 0x04:/*南都*/
		case 0x05:/*新日动力*/
		case 0x06:/*星恒*/
		case 0x07:/*ATL*/
		case 0x08:/*CATL*/
			FillBatInfo_Data_MetTuan((uint8 *)&item[0], frameLabel, addr);
			break;
		default:
			/*
			** 优化处理,厂商标号为无,清空所有BMS电池信息
			*/
			memset((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realVol,0x00,sizeof(BmsInfoMeiTuan));
			break;
	}
}
#endif

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
void FillBatInfo_Data_PineCone(uint8* item,uint8 frameLabel,uint8 addr){
	uint8 len = 0;
	uint8 dataLen = 0;

	switch(frameLabel){
		case 0x01:/*数据项:电池总电压,实时电流,相对容量百分比,SOH*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batTotalVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);	
			llParam.batDoor[addr].bmsInfoPineCone.relaCapPer = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoPineCone.soh = item[len];
			len += sizeof(uint8);			
			break;
		case 0x02:/*数据项:剩余容量,满充容量,循环次数*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.surplusCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.fullCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.circNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);				
			break;
		case 0x03:/*数据项:单节电池电压 13*2*/
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			dataLen = frameLabel == 0x07?0x06:0x02;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.singleBatVol[(frameLabel-0x03)*(0x06/sizeof(uint16))],
				(uint8*)&item[len],dataLen);
			len += dataLen;
			break;
		case 0x08:/*数据项:ID条码 20*/
		case 0x09:
		case 0x0A:
		case 0x0B:	
			dataLen = frameLabel == 0x0B?0x06:0x02;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batID[(frameLabel-0x03)*0x06],
				(uint8*)&item[len],dataLen);
			len += dataLen;			
			break;
		case 0x0C:/*数据项:电池温度,版本号*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batTemp,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.ver[0],(uint8*)&item[len],3);
			len += 3;
			break;
		default:
			
			break;
	}
}

void FillBatInfo_PineCone(uint8 fireCode,uint8* item,uint8 frameLabel,uint8 addr){
	switch(fireCode){
		case 0x81:/*星恒*/
			FillBatInfo_Data_PineCone((uint8 *)&item[0], frameLabel, addr);
			break;
		default:
			
			break;
	}
}
#endif

/*
** 下层协议之BMS信息
*/
void LLParse_BatInfo(uint8 rxlen,uint8* item,uint8 addr){
	uint8 frameLabel = 0;
	uint8 len = 0;
	
	/*
	** 数据项 帧标识 数据填充项长度 数据项内容
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);/*注明:数据填充项长度暂时未使用*/

	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
		FillBatInfo_MetTuan(llParam.compatibility[addr].fireCode, (uint8 *)&item[len], frameLabel, addr);
	#endif

	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
		FillBatInfo_PineCone(llParam.compatibility[addr].fireCode, (uint8 *)&item[len], frameLabel, addr);
	#endif
}

/*
** 更新通讯板软件版本号
*/
void update_ComSfotVer(uint16 ver,uint8 addr){
	uint8 len = 0;
	uint8 charVer[4] = {0};/*软件版本号*/
	#if CtrHardware == CtrHardware_Andriod
		uint8 charVerHard[4] = {"0200"};/*硬件版本号*/
	#else
		uint8 charVerHard[4] = {"0100"};/*硬件版本号*/
	#endif
	
	len = _itoa(ver/*temp*/, (char*)&charVer[0]);
	memset((uint8*)&llParam.batDoor[addr].batDoorSysPara.comSoftVer[0],'0',4);
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.comSoftVer[4-len],(uint8*)&charVer[0],len);
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.comHareVer[0],(uint8*)&charVerHard[0],4);
}

/*
** 更新软件版本 
*/
extern uint8 assignLabel;
extern uint16 assignSoftVer[SysCtr_AllDoorNum];
void LLParse_UpdateSoftVer(uint8 rxlen,uint8* item,uint8 addr){
	uint32 comAppRunVer = {0};
	uint16 verTemp = 0;
	uint8 len = 0;
	uint8 label = 0;

	/*
	** 标号
	*/
	label = item[len];
	len += sizeof(uint8);

	/*
	** 软件版本号
	*/
	memcpy((uint8*)&verTemp,(uint8*)&item[len],sizeof(uint16));
	len += sizeof(uint16);

	/*
	** 更新通讯板软件版本号
	*/
	if(label == 0x00){/*通讯板*/
		update_ComSfotVer(verTemp,addr);
	}

	/*
	** 指定标号升级查询指令
	*/
	if(assignLabel == label){
		assignSoftVer[addr] = verTemp;
	}
	

	/*
	** get 当前控制板保存的相应地址通讯板的软件
	*/
	comAppRunVer = getComRunAppVer(addr);
	
	if(comAppRunVer != verTemp){
		/*
		** 系统参数之更新通讯小板App运行版本
		*/
		setComRunAppVer(verTemp, addr);
	}

	/*
	**	增加上报接口通讯板运行软件版本和电池固件包软件版本
	*/
	llp_ULP->runComVer[label][addr] = verTemp;
}

// /*
// ** 下层协议之系统模块列表
// */ 
// static const LowerLayerFunTable llFunTable[] = {
// 	{LL_FunId_BatDoorState,LLParse_BatDoorState},
// 	{LL_FunId_BatUpgrState,LLParse_UpgrState},
// 	{LL_FunId_ChgSysInfo,LLPrse_ChgSysInfo},
// 	{LL_FunId_BmsInfo,LLParse_BatInfo},
// 	{UpgradeLL_FunId_ChkAppRunVer,LLParse_UpdateSoftVer},
// };
// static uint8 llFunTableNum = sizeof(llFunTable)/sizeof(LowerLayerFunTable);

/*
** 下层协议之解析任务函数接口
*/
void SM_LowerLayerParse_Task(void* p_arg){
// 	CanRxMsg rxMsg = {0};
// 	uint8 funId = 0;
// 	uint8 i=0;
// 	uint8 deviceAddr = 0;

	/*
	** 下层协议之参数初始化
	*/
	LowerLayerParam_Init();

	Sleep(2000);
	
	for(;;){
// 		for(;CAN_RecAnalysis(CAN_Port_1, (CanRxMsg *)&rxMsg) == true;){
// 			funId = (uint8)((rxMsg.ExtId>>16)&0x000000FF);
// 			for(i=0;i<llFunTableNum;i++){
// 				if(funId == llFunTable[i].funId){
// 					deviceAddr = (uint8)(rxMsg.ExtId&0x000000FF);				
// 					llFunTable[i].handle(rxMsg.DLC,(uint8*)&rxMsg.Data[0],deviceAddr);
// 					/*
// 					** 判断通讯是否失联
// 					*/
// 					llParam.comBoardChk[deviceAddr].cnt = 0;
// 					llParam.comBoardChk[deviceAddr].comAbn = false;
// 					break;
// 				}
// 			}
// 			/*
// 			** 看门口喂狗
// 			*/
// 			watchdogUpdate();
// 		}

// 		for(;CAN_RecAnalysis(CAN_Port_2, (CanRxMsg *)&rxMsg) == true;){
// 			funId = (uint8)((rxMsg.ExtId>>16)&0x000000FF);
// 			for(i=0;i<llFunTableNum;i++){
// 				if(funId == llFunTable[i].funId){
// 					deviceAddr = (uint8)(rxMsg.ExtId&0x000000FF);				
// 					llFunTable[i].handle(rxMsg.DLC,(uint8*)&rxMsg.Data[0],deviceAddr);
// 					/*
// 					** 判断通讯是否失联
// 					*/
// 					llParam.comBoardChk[deviceAddr].cnt = 0;
// 					llParam.comBoardChk[deviceAddr].comAbn = false;
// 					break;
// 				}
// 			}
// 			/*
// 			** 看门口喂狗
// 			*/
// 			watchdogUpdate();
// 		}		

		/*
		** 下层协议之数据解析之任务函数处理
		*/
		lowerUpgrFunAnalyze();

		/*
		** 判定通讯板是否失联,变更上层字段信息
		*/
		updateComIsAbn();

		/*
		** 根据状态信息更新舱门数(空仓数,充电仓数,满仓数,异常挂起数,总仓数)
		*/
		updateDoorNum();		

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
**提供系统软件创建下层协议解析接口函数
*/
void LowerLayerParse_Init(void){
	Thread_create(SM_LowerLayerParse_Task, 0,
		(int32 *)&lowerLayerParse_TaskStk[512-1], (int8)LowerLayerParseTask_Prio);
}

