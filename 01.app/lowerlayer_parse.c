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
**		bit6:置1--系统故障断AC
**		异常现象:
**			出现一级报警已恢复,app闪退--分控全部进入需要重新上电标志,但是安卓没显示,分控上报系统故障断AC,故主控检测无空仓,充电仓,满仓
------------------------------------------------------------------------------------------------------------------------------------
**		电池舱状态 -- Fun ID:0x01				分控判定故障-->红灯闪烁
**		---------------------------------------------------------
**		bit0:电池在线
**		bit1:电池舱充电器过温					1
**		bit2:电池过温								1
**		bit3:充电器故障							1
**		bit4:电池在线开启AC							-- 分控统计无故障信息:充电状态下,充电中,满电置位
**		bit5:远程断AC
**		bit6:系统故障断充电器AC					1
**		bit7:BMS通讯ID重复
**		bit8:电池故障								1
**		bit9:电池通讯故障							1
**		bit10:电池是否升级完成
**		bit11:电池升级是否成功
**		bit12:分控故障							1
**		bit13:充电异常							1
**		bit14:电池低温							1
**		bit15:一级告警分控板复位之后置位		分控判定故障-->红灯闪烁
**																				---->0x734E
**																				---->0x007E
*/
#define AbnDoorNumBits						(uint16)0x734E/*修改时间:20210309*//*0x03CE*/

/*
** 根据状态信息更新舱门数(空仓数,充电仓数,满仓数,异常挂起数,总仓数)
*/
uint8 doorStateFlag[SysCtr_AllDoorNum] = {0};
static void updateDoorNum(void){
	DoorTypeNum doorTypeTemp = {0};
	uint8 i = 0;

// 	for(i=0;i<SysCtr_AllDoorNum;i++){
// 		/*
// 		** 状态位的故障位优先级最高 -- 调整故障仓判断条件 20210309
// 		**	注明:整柜一级报警发生未直接置位所有故障位的原因:分控接收系统故障位之后,保持灯和故障一致性,此时失联以失联条件判断
// 		*/
// 		if((llParam.batDoor[i].batDoorStateInfo.flag & AbnDoorNumBits) != 0 
// 			|| llParam.comBoardChk[i].comAbn == true
// 			|| (llParam.chargeInfo[i].state.flag & ChargerAbnDoorNumBit) != 0){
// 			doorTypeTemp.abn++;
// 		}else{/*无故障*/
// 			if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == false){/*电池不在线*/
// 				doorTypeTemp.idle++;
// 			}else{/*电池在线*/
// 				if(llParam.batDoor[i].bmsInfoMeiTuan.soc >= get_SocLimit()){
// 					doorTypeTemp.full++;
// 				}				
// 			}
// 		}
// 	}

// 	doorTypeTemp.ing = SysCtr_AllDoorNum - (doorTypeTemp.abn + doorTypeTemp.idle + doorTypeTemp.full);
// 	doorTypeTemp.total = SysCtr_AllDoorNum;

// 	/*修改:20210309--统计:满仓数量,充电充数量,空仓数量*/
// 	for(i=0;i<SysCtr_AllDoorNum;i++){
// 		if(llParam.comBoardChk[i].comAbn == false){/*分控在线--分控不在线即认为仓存在故障*/
// 			if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == true){/*电池在线*/
// 				if(llParam.batDoor[i].bmsInfoMeiTuan.soc >= get_SocLimit()){/*满电*/
// 					doorTypeTemp.full++;
// 				}else{/*充电中*/
// 					if(llParam.batDoor[i].batDoorStateInfo.bits.batOnlineOpenAC == true){/*判定充电中*/
// 						doorTypeTemp.ing++;	
// 					}
// 				}
// 			}else{/*只要电池不在线即判定为空仓--纵使充电存在故障,同样也判定为空仓*/
// 				doorTypeTemp.idle++;
// 			}
// 		}
// 	}
// 	doorTypeTemp.abn = SysCtr_AllDoorNum - (doorTypeTemp.idle+doorTypeTemp.ing+doorTypeTemp.full);
// 	doorTypeTemp.total = SysCtr_AllDoorNum;

// 	/*
// 	** 开关拉下,更新检测仓门统计信息方案
// 	*/
// 	if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){
// 		memset((uint8*)&doorTypeTemp.idle,0,sizeof(DoorTypeNum));
// 		doorTypeTemp.abn = SysCtr_AllDoorNum;
// 		doorTypeTemp.total = SysCtr_AllDoorNum;
// 	}

	/*修改:20210512
	* 策略:
	*	1.整柜存在一级告警-->将所有仓位置为空仓
	*	2.AC下电-->将所有仓位置为空仓
	*	3.分控不在线-->置为空仓
	*	4.分控检测充电器和电池以及自身检测故障状态-->置为空仓
	*	5.充电中/满仓根据分控判定
	*/
	if(cabinet_OneWaringFlagJudge() == true/*整柜存在一级告警-->空仓*/
		||llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true/*AC断开*/
		){
		memset((uint8*)&doorTypeTemp.idle,0,sizeof(DoorTypeNum));
		doorTypeTemp.idle = SysCtr_AllDoorNum;
		doorTypeTemp.total = SysCtr_AllDoorNum;
	}else{
		for(i=0;i<SysCtr_AllDoorNum;i++){
			/*分控不在线判定为故障*/
			if((llp_ULP->stateInfoChange.sysLogic.comIsOnline&((DoorNumDefine)(((DoorNumDefine)0x01)<<i))) != (DoorNumDefine)0){
				doorTypeTemp.idle++;
			}else{
				switch(doorStateFlag[i]){
					case 0:/*空仓*/
						doorTypeTemp.idle++;
						break;
					case 1:/*充电中*/
						doorTypeTemp.ing++;
						break;
					case 2:/*满仓*/
						doorTypeTemp.full++;
						break;
				}
			}
		}
	}

	/*更新上层协议舱门数参数*/
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
extern bool oneWaringResetSetFlag;
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
		/*20210302--一级告警恢复标志*/
		oneWaringResetSetFlag = false;
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
** 20210422--新增:电池一级告警==充电过流/充电高温
*/
bool chk_BatIsExitOneWaring(void){
	uint8 i = 0;
	bool ret = false;
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		if(llParam.batDoor[i].bmsInfoMeiTuan.pState.bits.batChgOT == true
			|| llParam.batDoor[i].bmsInfoMeiTuan.pState.bits.batChgOC == true){
			ret = true;/*整柜存在电池一级告警*/
			break;
		}		
	}
	return ret;
}


/*
** 新增:20210115--张工测试需求 分控请求升级 -- 在充电器过温,电池过温(整柜中任意仓存在--拒绝升级)
*/
bool chk_IsExitBatChargerOT(void){
	bool flag = false;

	if(llp_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*充电器过温*/
		|| llp_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*电池过温*/
		|| llp_ULP->stateInfoChange.sysLogic.batChgOTime != 0 /*电池充电时间过长*/
		|| chk_BatIsExitOneWaring() == true
		){
		flag = true;
	}

	return flag;
}

/*
** 电池升级状态
*/
extern DoorNumDefine hmiUpgrIsFinshFlag;
/*新增识别:正常模式和ProOTA模式时差问题20210426*/
DoorNumDefine batNomal_UpgrageIsTrue = 0;
void LLParse_UpgrState(uint8 rxlen,uint8* item,uint8 addr){
	static uint8 frame[SysCtr_AllDoorNum] = {0};
	uint8 len = 0;
	uint8 frameLabel = 0;
	uint8 txlen = 1;
	uint8 tx[8] = {0};
	static UpgrFail upgrFailTemp[SysCtr_AllDoorNum] = {0};
	UpgrFilePara upgrTemp = {0};
	int16 loc = -1;
	uint16 upgrExceNum = 0;
	static uint8 upgrModel = 0;
	DoorNumDefine upgrTempField = 0xE000000000000000;
	static uint16 upgrNumSyn[SysCtr_AllDoorNum] = {0};
	static uint16 upgrSynTemp[SysCtr_AllDoorNum] = {0};/*使用static 标识原因是-->包顺序不一致*/
	static bool isChkSynFlag[SysCtr_AllDoorNum] = {0};/*使用static 标识原因是-->包顺序不一致*/
		
	/*
	** 帧标识 帧数据长度暂时未使用
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);

	switch(frameLabel){/*帧标识*/
		case 0x01:/*硬件版本+固件ID(4字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.hardVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[0],(uint8*)&item[len],4);
			len +=  4;
			break;
		case 0x02:/*固件ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[4],(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x03:/*固件ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[10],(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x04:/*原软件版本号+电池ID(4字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.softVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[0],(uint8*)&item[len],4);
			len += 4;
			break;
		case 0x05:/*电池ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[4],(uint8*)&item[len],6);
			len += 6;				
			break;
		case 0x06:/*电池ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[10],(uint8*)&item[len],6);
			len += 6;					
			break;
		case 0x07:/*升级模式*/
			upgrModel = item[len];
			len += 1;
			if(rxlen == 0x05){/*协议V1.9新增:升级标识*/
				memcpy((uint8*)&upgrSynTemp[addr],(uint8*)&item[len],sizeof(uint16));
				len += sizeof(uint16);
				isChkSynFlag[addr] = true;/*检测同步标志*/
			}
			break;
	}


	/*
	** 检测帧是否接受完整
	*/
	frame[addr] |= 0x01<<(frameLabel - 1);
	if(frame[addr] == 0x7F){
		frame[addr] = 0;
		/*获取固件位置*/
		memcpy((uint8*)&upgrTemp.upgrFileType.detailedInfo.hardVer,(uint8*)&upgrFailTemp[addr].bat.hardVer,sizeof(DetailedInfo));
		/*清除变量upgrFailTemp[addr]数据--upgrFailTemp[addr]变量在处清除,原因:该变量仅在上一条语句上做标识*/
		memset((uint8*)&upgrFailTemp[addr].bat.hardVer,0,sizeof(UpgrFail));
		loc = get_BatLocation(upgrTemp);
		/*检测是否允许分控升级*/
		if(loc == -1 ){/*固件ID不合法*/
			tx[0] = 0x04;/*拒绝升级--因"电池过温","充电器过温"*/
		}else{
			/*新增:电池过温,充电器过温禁止升级*/
			if(chk_IsExitBatChargerOT() == true){
				tx[0] = 0x02;/*拒绝升级*/
			}else{
				switch(upgrModel){
					case 0x01:/*正常模式*/
						if(get_FireUpgrNum(0x03, loc) == (int16)-1){/*查询电池固件包为无限次升级逻辑-->新增:测试模式允许无限次升级但不统计升级次数*/
							tx[0] = 0x01;/*允许升级*/
							batNomal_UpgrageIsTrue |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
						}else{
							upgrExceNum = get_FireRealityRunNum(0x03, loc);
							if(get_FireUpgrNum(0x03, loc) <= upgrExceNum){
								tx[0] = 0x02;/*拒绝升级*/
							}else{
								tx[0] = 0x01;/*允许升级*/
								if(upgrNumSyn[addr] != upgrSynTemp[addr] /*同步标志下-->检测是否为不同升级需求*/
									|| isChkSynFlag[addr] == false /*V1.8协议之间无同步逻辑*/
									){
									upgrNumSyn[addr] = upgrSynTemp[addr];
									upgrExceNum++;
									if(upgrExceNum == 0xFFFF){
										upgrExceNum = 1;
									}	
									set_FireRealityRunNum(0x03, upgrExceNum, loc);
								}
							}
						}						
						break;
					case 0x02:/*测试模式*/
						tx[0] = 0x01;/*允许升级*/
						break;
				}			

				/*允许升级模式下置位下述参数才有意义*/
				if(tx[0] == 0x01){/*允许升级*/
					/*更新电池升级模式*/
					switch(upgrModel){
						case 0x01:/*正常模式*/
							llp_ULP->stateInfoChange.sysLogic.batUpgr &= (DoorNumDefine)~upgrTempField;
							break;
						case 0x02:/*测试模式*/
							llp_ULP->stateInfoChange.sysLogic.batUpgr |= (DoorNumDefine)upgrTempField;
							break;
					}
					
					llp_ULP->stateInfoChange.sysLogic.batUpgr |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
					llp_ULP->stateInfoChange.sysLogic.batUpgrIsOk &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
					hmiUpgrIsFinshFlag &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);					
				}
			}
		}
		/*清模式参数*/
		upgrModel = 0;
		/*清同步标识参数*/
		upgrSynTemp[addr] = 0;
		isChkSynFlag[addr] = false;
		/*回复分控板*/
		CAN_TransmitAnalysis(CAN_Port_1, txlen, (uint8 *)&tx[0], addr, LL_FunId_BatUpgrState);
	}
}

/*
** 充电仓系统信息
*/
void LLPrse_ChgSysInfo(uint8 rxlen,uint8* item,uint8 addr){
	uint8 len = 0;
	LPare03_01 lpare03_01 = {0};
	
	/*
	** 电池状态信息
	*/
	lpare03_01.flag = item[len];
	if(lpare03_01.bits.batOnline == true){/*电池在线*/
		llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = true;
	}

	/*查询分控退出升级或固件下载中*/
	if(lpare03_01.bits.subExitUpgr == true){
		/*20210128--修改:清升级开始标志*/
		llp_ULP->stateInfoChange.sysLogic.comUpgr &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);
		llp_ULP->stateInfoChange.sysLogic.batFileDownload &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);	
	}

	/*空仓,充电中,满仓*/
	if(lpare03_01.bits.idle == true){
		doorStateFlag[addr] = 0;/*空仓*/
	}else if(lpare03_01.bits.ing == true){
		doorStateFlag[addr] = 1;/*充电中*/
	}else if(lpare03_01.bits.full == true){/*满仓*/
		doorStateFlag[addr] = 2;/*满仓*/
	}else{/*均未置位-->空仓*/
		doorStateFlag[addr] = 0;/*空仓*/
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
		/*-------------------美团:美团助力车-主控_电池通信协议1.15.pdf----------------------*/
		case 0x11:/*物料变更记录,电芯数,FCC*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batMTV115.materialChangeR,(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x12:/*真实SOC,电池充电状态,电池内阻*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batMTV115.realitySoc,(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x13:/*均衡管状态,复位次数*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batMTV115.banlanceState[0],(uint8*)&item[len],6);
			len += 6;			
			break;
		case 0x14:/*充电停止真正原因*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batMTV115.chgStopReason[0],(uint8*)&item[len],4);
			len += 6;			
			break;
		/*----------------------------------------------------------------------------------*/
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
** 检测是否存在失联分控板--强制置位--测试模式下
*/
DoorNumDefine force_SetFlag(DoorNumDefine setFlag){
	uint8 i =0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		if(llParam.comBoardChk[i].comAbn == true/*通讯板通讯丢失*/){
			setFlag |= (DoorNumDefine)((((DoorNumDefine)1)<<i));
		}
	}

	return setFlag;
}

/*
** 更新软件版本 
*/
extern uint8 assignLabel;
extern uint16 assignSoftVer[SysCtr_AllDoorNum];
void LLParse_UpdateSoftVer(uint8 rxlen,uint8* item,uint8 addr){
	/*uint32 comAppRunVer = {0};*/
	uint16 comAppRunVer = 0;
	uint16 verTemp = 0;
	uint8 len = 0;
	uint8 label = 0;
	static uint8 lastLabel = 0;
	static DoorNumDefine clearBatFire = 0; 
	static DoorNumDefine enterTestMode = 0;
	uint16 upgrFailNum = 0;
	uint8 i = 0;

	/*
	** 标号
	*/
	label = item[len];
	len += sizeof(uint8);

	/*检测是否变更标号--请标志位*/
	if(label != lastLabel){
		lastLabel = label;
		clearBatFire = 0;
		enterTestMode = 0;
		for(i=0;i<SysCtr_AllDoorNum;i++){
			/*更换固件标号,先清除统计升级失败次数*/
			set_statisAccUpgrNum(0, i);
		}
	}

	/*
	** 软件版本号
	*/
	memcpy((uint8*)&verTemp,(uint8*)&item[len],sizeof(uint16));
	len += sizeof(uint16);

	/*
	** 兼容版本迭代处理--判断数据长度
	*/
	if((rxlen - len) > 0){
		/*清除对应固件包*/
		switch(item[len]){
			case 0x01:/*清除成功*/
			case 0x04:/*固件清除已执行*/
				/*清除电池固件包响应*/
				clearBatFire |= (DoorNumDefine)((((DoorNumDefine)1)<<addr));
				/*新增:20210525--指定清除电池固件--分控失联默认已清除*/
				clearBatFire = force_SetFlag(clearBatFire);
				if(label >= 1 && label < (get_batFireSize() + 1)){
					if(get_ClearFire(0x03, label - 1) == true){
						if(clearBatFire == (DoorNumDefine)0xFFFFFFFFFFFF){
							clearBatFire = 0;
							set_ClearFire(0x03, false, label - 1);
						}
					}else{
						clearBatFire = 0;
					}
				}
				break;
			case 0x02:/*清除失败*/
				
				break;
		}
		len += sizeof(uint8);
		
		/*检测对应固件包是否已接受测试模式*/
		if((rxlen - len) > 0){
			switch(item[len]){
				case 0x01:/**/
					/*电池固件进入测试模式响应*/
					enterTestMode |= (DoorNumDefine)((((DoorNumDefine)1)<<addr));
					/*
					** 检测是否存在失联分控板--强制置位--测试模式下 -- 修改:失联之后,重新在线再配置测试模式,完成测试模式的条件:AC断开(分控复位),所有分控均以回复进入测试模式
					** 20210525 -- 变更为:分控失联默认为已清除固件-- 已发布:CDG_MD_B_B1_300_380.bin/CDG_MD_B_B1_300_381.bin
					** 修改:逻辑屏蔽掉20210525
					*/
					//enterTestMode = force_SetFlag(enterTestMode);
					if(label >= 1 && label < (get_batFireSize() + 1)){
						if(get_FireEnterTestModel(0x03, label - 1) == true){
							if(enterTestMode == (DoorNumDefine)0xFFFFFFFFFFFF){
								enterTestMode = 0;
								set_FireEnterTestModel(0x03, false, label - 1);
							}
						}else{
							enterTestMode = 0;
						}
					}
					break;
				case 0x02:/*清除失败*/
					
					break;
			}
		}
		len += sizeof(uint8);

		if((rxlen - len) > 0){
			/*相应固件是否已连续3升级失败 -- 20210301 -- 现阶段(针对同一固件统一软件):唐工仅处理分控升级失败三次之后,不再升级,-- 电池固件升级失败三次并未限制*/
			memcpy((uint8*)&upgrFailNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			set_statisAccUpgrNum(upgrFailNum, addr);
		}
	}

	/*
	** 更新通讯板软件版本号 -- 仅用于注册信息帧--分控软件版本号的更新--20210301--可不再维护
	*/
	if(label == 0x00){/*通讯板*/
		update_ComSfotVer(verTemp,addr);
	}

	/*
	** 指定标号升级查询指令 -- 用于线程升级逻辑软件版本号比对逻辑 -- 20210301
	*/
	if(assignLabel == label){
		assignSoftVer[addr] = verTemp;
	}
	

	/*
	** get 当前控制板保存的相应地址通讯板的软件 -- 用于是否进入线程,软件版本号比对 -- 20210301
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
/*
** upgr Fail Reason
*/
void LLParse_UpgrFailReason(uint8 rxlen,uint8* item,uint8 addr){
	static uint8 frame[SysCtr_AllDoorNum] = {0};
	uint8 len = 0;
	uint8 frameLabel = 0;
	uint8 txlen = 1;
	uint8 tx[8] = {0};
	static UpgrFail upgrFailTemp[SysCtr_AllDoorNum] = {0};
	uint8 fireType = 0;
	UpgrFilePara upgrTemp = {0};
	int16 loc = -1;
	uint16 num = 0;
	static uint16 upgrNumSyn[SysCtr_AllDoorNum] = {0};
	static uint16 upgrSynTemp[SysCtr_AllDoorNum] = {0};/*使用static 标识原因是-->包顺序不一致*/
	static bool isChkSynFlag[SysCtr_AllDoorNum] = {0};/*使用static 标识原因是-->包顺序不一致*/
	
	/*
	** 帧标识 帧数据长度暂时未使用
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);

	switch(frameLabel){/*帧标识*/
		case 0x01:/*硬件版本+固件ID(4字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.hardVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[0],(uint8*)&item[len],4);
			len +=  4;
			break;
		case 0x02:/*固件ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[4],(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x03:/*固件ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireId[10],(uint8*)&item[len],6);
			len += 6;
			break;
		case 0x04:/*原软件版本号+电池/充电器ID(4字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.softVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[0],(uint8*)&item[len],4);
			len += 4;
			break;
		case 0x05:/*电池/充电器ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[4],(uint8*)&item[len],6);
			len += 6;				
			break;
		case 0x06:/*电池/充电器ID(6字节)*/
			memcpy((uint8*)&upgrFailTemp[addr].bat.id[10],(uint8*)&item[len],6);
			len += 6;					
			break;
		case 0x07:/*升级失败原因,固件软件版本号 + 升级模式*/
			upgrFailTemp[addr].bat.reson = item[len];
			len += sizeof(uint8);
			memcpy((uint8*)&upgrFailTemp[addr].bat.fireSoftVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			upgrFailTemp[addr].bat.upgrModel = item[len];/*升级模式*/
			len += sizeof(uint8);
			if(rxlen == 0x08){/*协议V1.9新增:升级标识*/
				memcpy((uint8*)&upgrSynTemp[addr],(uint8*)&item[len],sizeof(uint16));
				len += sizeof(uint16);
				isChkSynFlag[addr] = true;/*检测同步标志*/
			}			
			break;
	}


	/*
	** 检测帧是否接受完整
	*/
	frame[addr] |= 0x01<<(frameLabel - 1);
	if(frame[addr] == 0x7F){
		frame[addr] = 0;
		/*获取固件位置*/
		memcpy((uint8*)&upgrTemp.upgrFileType.detailedInfo.hardVer,(uint8*)&upgrFailTemp[addr].bat.hardVer,sizeof(DetailedInfo));
		/*固件类型*/
		if(upgrFailTemp[addr].bat.fireId[0] == 'A'){/*'A'--电池固件*/
			fireType = 0x03;
			loc = get_BatLocation(upgrTemp);
		}

		/*分控检测是否为正常模式-->主控根据正常模式下次数限制判断解析出:正常升级模式/Pre OTA*/
		if(get_FireUpgrNum(fireType, loc) == -1 /*表明本次升级:正常模式下,非Pre OTA模式*/
		|| (((batNomal_UpgrageIsTrue&(1<<addr)) != 0)&& (fireType == 0x03))){
			if(loc != -1){
				if(upgrFailTemp[addr].bat.upgrModel == 0x01){/*分控判断为"正常模式"*/
					upgrFailTemp[addr].bat.upgrModel = 0x03;/*正常模式*/
					switch(fireType){
						case 0x03:
							batNomal_UpgrageIsTrue &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
							break;
						case 0x04:

							break;
					}
					
				}
			}
		}

		if(upgrNumSyn[addr] != upgrSynTemp[addr] /*同步标志下-->检测是否为不同升级需求*/
			|| isChkSynFlag[addr] == false /*V1.8协议之间无同步逻辑*/
		){
			/*将升级失败原因插入队列中*/
			set_UpgrFailPara(fireType,upgrFailTemp[addr], addr);
		}
		/*统计升级次数逻辑*/
		switch(upgrFailTemp[addr].bat.upgrModel){/*20210425将PROOAT升级次数统计放在--申请升级中*/
			case 0x01:/*Pre OTA模式*/
// 				if(loc != -1){
// 					num = get_FireRealityRunNum(fireType, loc);
// 					num++;
// 					if(num == 0xFFFF){
// 						num = 1;
// 					}
// 					set_FireRealityRunNum(fireType, num, loc);
// 				}
				break;
			case 0x02:/*测试模式*/
				if(loc != -1){
					if(upgrNumSyn[addr] != upgrSynTemp[addr] /*同步标志下-->检测是否为不同升级需求*/
						|| isChkSynFlag[addr] == false /*V1.8协议之间无同步逻辑*/
					){
						if(upgrFailTemp[addr].bat.reson == 0xFF/*升级成功*/){
							num = get_TestModelFireUpgrNum(fireType, loc);
							num++;
							if(num == 0xFFFF){
								num = 1;
							}
							set_TestModelFireUpgrNum(fireType, num, loc);
						}else{/*升级失败*/
							num = get_TestModelFireUpgrNumFail(fireType, loc);
							num++;
							if(num == 0xFFFF){
								num = 1;
							}
							set_TestModelFireUpgrNumFail(fireType, num, loc);
						}
					}
				}
				break;
			case 0x03:/*正常模式*/
				
				break;
		}
		/*同步参数*/
		upgrNumSyn[addr] = upgrSynTemp[addr];
		/*清变量数据*/
		memset((uint8*)&upgrFailTemp[addr].bat.reson,0,sizeof(UpgrFail));	
		/*清同步标识*/
		upgrSynTemp[addr] = 0;
		isChkSynFlag[addr] = false;
		/*回复分控板*/
		CAN_TransmitAnalysis(CAN_Port_1, txlen, (uint8 *)&tx[0], addr, LL_FunId_upgrFailReason);
	}
}

/*
** 下层协议之解析任务函数接口
*/
void SM_LowerLayerParse_Task(void* p_arg){
	/*
	** 下层协议之参数初始化
	*/
	LowerLayerParam_Init();

	Sleep(2000);
	
	for(;;){
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

