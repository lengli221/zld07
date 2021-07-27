#include "includes.h"

static int32 sysCtrlLogic_TaskStk[256] = {0};

/*
** 使用指针调用其他文件参数
*/
UpperLayerPara* sys_ULP = null;

/*
** 固件控制之初始化参数
*/
void init_FirewarePara(void){
	sys_ULP = getUpperLayerPara();
}

/*
** 固件--Output--风扇
*/
void firewareCtr_Fan(void){
	bool ret = false;
	Tsave ts = get_Tsave();
	int16 curTemp = 0;
	
	ret = CC_Control_Fan((uint16)(ts.base/10), (uint16)(ts.diff/10), (int16*)&curTemp);

	/*
	** 设置充电仓环温
	*/
	sys_ULP->sysFire.chgDoorTemp = curTemp;
	/*
	** 风扇控制 标志位控制
	*/
	firewareCtr(setFirewareFan_Label, ret);
	sys_ULP->sysFire.state.bits.fan = ret;
	sys_ULP->stateInfoChange.sysModuleStateInfo.bits.fanOpen = ret;
}

/*
** 固件--Output--报警灯
*/
void firewareCtrLed(void){
	if(sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*烟感*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*急停*/
		||  sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*充电器仓过温*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*充电器过温*/
		||  sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*电池过温*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault == true/*相电压过压>270V,需下电才能恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring == true/*相电压过压可恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseLV == true/*相电压欠压,可恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOC == true/*相电流过流*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseCShort == true/*相电流短路*/
		){
		firewareCtr(setFireFalutLed_Label, true);
		/*
		** 增加声光报警--上报
		*/
		sys_ULP->stateInfoChange.sysModuleStateInfo.bits.ledOpen = true;
	}else{
		firewareCtr(setFireFalutLed_Label, false);
		sys_ULP->stateInfoChange.sysModuleStateInfo.bits.ledOpen = false;
	}
}

/*
** ctr run led 
*/
void ctr_RunLed(void){
	static uint32 itick = 0;
	static bool flag = false;

	if(TickOut((uint32 *)&itick, 500) == true){
		TickOut((uint32 *)&itick, 0);
		flag = flag == false?true:false;
		firewareCtr(setFireCtrRunLed_Label, flag);
	}
}

/*
** 固件--Output
*/
void firewareCtrAnalyze(void){
	/*
	** 固件--Output--风扇
	*/
	firewareCtr_Fan();
	/*
	** 固件--Output--报警灯
	*/
	firewareCtrLed();
	/*
	** ctr run led 
	*/
	ctr_RunLed();	
}



/*
** 固件--Input--烟感
*/
void firewareSmoke(void){
	/*static int8 cnt = 0;*/
	static int16 cnt = -150;

	if(firewareYxIn(getYxIn_Smoke)){
		if(cnt++ >= 150/*5*/){
			cnt = 150/*5*/;
			#if VerCtr == VerCtr_Normal
				sys_ULP->sysFire.state.bits.smoke = true;
				sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke = true;
			#endif
		}
	}else{
		if(cnt-- <= -150/*-5*/){
			cnt = -150/*-5*/;
			sys_ULP->sysFire.state.bits.smoke = false;
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke = false;
		}
	}
}

/*
** 固件--input--急停
*/
void firewareEmer(void){
	static int8 cnt = 0;

	if(firewareYxIn(getYxIn_Emer)){
		if(cnt++ >= 5){
			cnt = 5;
			#if VerCtr == VerCtr_Normal
				sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer = true;
			#endif
		}
	}else{
		if(cnt-- <= -5){
			cnt = -5;
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer = false;
		}
	}
}

/*
** 固件--input--防雷
*/
void firewareAnti(void){
	static int8 cnt = 0;

	if(firewareYxIn(getYxIn_Anti)){
		if(cnt++ >= 5){
			cnt = 5;
			#if VerCtr == VerCtr_Normal
				sys_ULP->stateInfoChange.sysModuleStateInfo.bits.anti = true;
			#endif
		}
	}else{
		if(cnt-- <= -5){
			cnt = -5;
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.anti = false;
		}
	}
}

/*
** 固件--Input
*/
void firewareYxInAnalyze(void){
	/*
	** 固件--Input--烟感
	*/
	firewareSmoke();
	/*
	** 固件--input--急停
	*/
	firewareEmer();	
	/*
	** 固件--input--防雷
	*/
	firewareAnti();	
	/*
	** 实时检测一级报警禁止升级
 	*/
 	oneWaring_ForbidUpgrDownFile();
}

/*
** 内部通讯异常(主控60S读取不到所有分控板信息)
*/
void sysInnerComErr(void){
	/*----------------------------------------------------------------------------------------*/
	/*
	** 屏蔽定时原因:
	**				现将单块分控板的失联时间修改为:60S
	*/
	/*----------------------------------------------------------------------------------------*/
	/*static uint32 itick = 0;*/
	if(sys_ULP->stateInfoChange.sysLogic.comIsOnline == (DoorNumDefine)0xFFFFFFFFFFFF){
		/*硬件开关合上才检测分控失联指令*/
		if(sys_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == false){
			/*if(TickOut((uint32 *)&itick, 60000) == true){*/
				sys_ULP->sysFire.state.bits.interCom = true;
			/*}*/
		}else{
			sys_ULP->sysFire.state.bits.interCom = false;
		}
	}else{
		/*TickOut((uint32 *)&itick, 0);*/
		sys_ULP->sysFire.state.bits.interCom = false;
	}	
}

/*
** 更新充电器仓过温标志
*/
void updateChgDoorOverTempFlag(void){
	if(sys_ULP->sysFire.chgDoorTemp >= get_ChgDoorOTLimit()){
		#if VerCtr == VerCtr_Normal
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp = true;
		#endif
	}else{
		if(sys_ULP->sysFire.chgDoorTemp /*>= get_ChgDoorOTLimit()*/ < (get_ChgDoorOTLimit() - 50)){/*回差5摄氏度*/
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp = false;
		}
	}
}

/*
** 检测一级页面警报
*/
extern Hmi_PhaseVC hmi_PhaseVc;
bool checkFirstPageWaring(void){
	bool ret = false;
	
	if(sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*电池过温*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0 /*充电器过温*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*充电器仓过温*/
		|| sys_ULP->stateInfoChange.sysLogic.batChgOTime != 0 /*充电时间过长*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*烟雾报警*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*急停按下*/
		|| sys_ULP->stateInfo.batOnline_1minNoCur != 0 /*充电器故障*/
		|| sys_ULP->stateInfoChange.sysLogic.batFault != 0/*电池故障*/
		|| sys_ULP->stateInfoChange.sysLogic.comFault != 0/*分控故障*/
		|| sys_ULP->sysFire.state.bits.interCom == true/*内部失联*/
		|| hmi_PhaseVc.flag != 0/*过压,欠压,过流*/ 
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.anti == true/*防雷故障*/){
		
		ret = true;
	}

	return ret;
}

/*
** 增加一级报警禁止升级以及下载文件
*/
bool oneWaringResetSetFlag = false;
bool oneWaring_ForbidUpgrDownFile(void){
	bool ret = false;
	
	if(sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*烟感*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*急停*/
		||  sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*充电器仓过温*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*充电器过温*/
		||  sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*电池过温*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault == true/*相电压过压>270V,需下电才能恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring == true/*相电压过压可恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseLV == true/*相电压欠压,可恢复*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOC == true/*相电流过流*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseCShort == true/*相电流短路*/){
		ret = true;
		oneWaringResetSetFlag = true;
	}
	return ret;
}

/*
** 系统 Check
*/
void sysCheck(void){
	/*
	** 内部通讯异常(主控60S读取不到所有分控板信息)
	*/
	sysInnerComErr();
	/*
	** 更新充电器仓过温标志
	*/
	updateChgDoorOverTempFlag();	
}

void SM_SysCtrlLogic_Task(void* p_arg){
	/*
	** 固件控制之初始化参数
	*/
	init_FirewarePara();

	Sleep(500);

	#if Debug_Flash_BugAnalysis == 1
	/*
	** Flash Bug调试说明
	*/	
	sFlash_BugAnalysis();
	#endif
	
	for(;;){
		/*
		** 固件--Output
		*/
		firewareCtrAnalyze();		
		/*
		** 系统 Check
		*/
		sysCheck();
		/*
		** 固件--Input
		*/
		firewareYxInAnalyze();

		/*
		** 通讯板任务更新+任务相关参数更新
		*/
		ComBoard_TaskSwitch();
		/*
		** 下载通讯板/电池包文件策略:
		**		1.接收下载通讯小板命令
		**		2.等待执行回复命令,间隔800ms之后控制上层任务切换
		*/
		ctrBoard_TaskSwitch();
		/*
		** 恢复上层协议之APP应用层协议 
		**		控制策略:
		**				1.文件下载完成之后,设置标志
		**				2.等待回复(延时)800ms之后设置任务切换
		*/
		ctrBoard_TaskRecover();	

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
**提供系统软件创建系统控制接口函数
*/
void SysCtrlLogic_Init(void){
	Thread_create(SM_SysCtrlLogic_Task, 0, (int32 *)&sysCtrlLogic_TaskStk[256-1], SysCtrlLogic_Prio);
}

