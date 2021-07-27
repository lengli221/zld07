#include "includes.h"

static int32 sysCtrlLogic_TaskStk[256] = {0};

/*
** ʹ��ָ����������ļ�����
*/
UpperLayerPara* sys_ULP = null;

/*
** �̼�����֮��ʼ������
*/
void init_FirewarePara(void){
	sys_ULP = getUpperLayerPara();
}

/*
** �̼�--Output--����
*/
void firewareCtr_Fan(void){
	bool ret = false;
	Tsave ts = get_Tsave();
	int16 curTemp = 0;
	
	ret = CC_Control_Fan((uint16)(ts.base/10), (uint16)(ts.diff/10), (int16*)&curTemp);

	/*
	** ���ó��ֻ���
	*/
	sys_ULP->sysFire.chgDoorTemp = curTemp;
	/*
	** ���ȿ��� ��־λ����
	*/
	firewareCtr(setFirewareFan_Label, ret);
	sys_ULP->sysFire.state.bits.fan = ret;
	sys_ULP->stateInfoChange.sysModuleStateInfo.bits.fanOpen = ret;
}

/*
** �̼�--Output--������
*/
void firewareCtrLed(void){
	if(sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*�̸�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*��ͣ*/
		||  sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*������ֹ���*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*���������*/
		||  sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*��ع���*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault == true/*���ѹ��ѹ>270V,���µ���ָܻ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring == true/*���ѹ��ѹ�ɻָ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseLV == true/*���ѹǷѹ,�ɻָ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOC == true/*���������*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseCShort == true/*�������·*/
		){
		firewareCtr(setFireFalutLed_Label, true);
		/*
		** �������ⱨ��--�ϱ�
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
** �̼�--Output
*/
void firewareCtrAnalyze(void){
	/*
	** �̼�--Output--����
	*/
	firewareCtr_Fan();
	/*
	** �̼�--Output--������
	*/
	firewareCtrLed();
	/*
	** ctr run led 
	*/
	ctr_RunLed();	
}



/*
** �̼�--Input--�̸�
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
** �̼�--input--��ͣ
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
** �̼�--input--����
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
** �̼�--Input
*/
void firewareYxInAnalyze(void){
	/*
	** �̼�--Input--�̸�
	*/
	firewareSmoke();
	/*
	** �̼�--input--��ͣ
	*/
	firewareEmer();	
	/*
	** �̼�--input--����
	*/
	firewareAnti();	
	/*
	** ʵʱ���һ��������ֹ����
 	*/
 	oneWaring_ForbidUpgrDownFile();
}

/*
** �ڲ�ͨѶ�쳣(����60S��ȡ�������зֿذ���Ϣ)
*/
void sysInnerComErr(void){
	/*----------------------------------------------------------------------------------------*/
	/*
	** ���ζ�ʱԭ��:
	**				�ֽ�����ֿذ��ʧ��ʱ���޸�Ϊ:60S
	*/
	/*----------------------------------------------------------------------------------------*/
	/*static uint32 itick = 0;*/
	if(sys_ULP->stateInfoChange.sysLogic.comIsOnline == (DoorNumDefine)0xFFFFFFFFFFFF){
		/*Ӳ�����غ��ϲż��ֿ�ʧ��ָ��*/
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
** ���³�����ֹ��±�־
*/
void updateChgDoorOverTempFlag(void){
	if(sys_ULP->sysFire.chgDoorTemp >= get_ChgDoorOTLimit()){
		#if VerCtr == VerCtr_Normal
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp = true;
		#endif
	}else{
		if(sys_ULP->sysFire.chgDoorTemp /*>= get_ChgDoorOTLimit()*/ < (get_ChgDoorOTLimit() - 50)){/*�ز�5���϶�*/
			sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp = false;
		}
	}
}

/*
** ���һ��ҳ�澯��
*/
extern Hmi_PhaseVC hmi_PhaseVc;
bool checkFirstPageWaring(void){
	bool ret = false;
	
	if(sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*��ع���*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0 /*���������*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*������ֹ���*/
		|| sys_ULP->stateInfoChange.sysLogic.batChgOTime != 0 /*���ʱ�����*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*������*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*��ͣ����*/
		|| sys_ULP->stateInfo.batOnline_1minNoCur != 0 /*���������*/
		|| sys_ULP->stateInfoChange.sysLogic.batFault != 0/*��ع���*/
		|| sys_ULP->stateInfoChange.sysLogic.comFault != 0/*�ֿع���*/
		|| sys_ULP->sysFire.state.bits.interCom == true/*�ڲ�ʧ��*/
		|| hmi_PhaseVc.flag != 0/*��ѹ,Ƿѹ,����*/ 
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.anti == true/*���׹���*/){
		
		ret = true;
	}

	return ret;
}

/*
** ����һ��������ֹ�����Լ������ļ�
*/
bool oneWaringResetSetFlag = false;
bool oneWaring_ForbidUpgrDownFile(void){
	bool ret = false;
	
	if(sys_ULP->stateInfoChange.sysModuleStateInfo.bits.smoke == true/*�̸�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.emer == true/*��ͣ*/
		||  sys_ULP->stateInfoChange.sysModuleStateInfo.bits.chgDoorOverTemp == true /*������ֹ���*/
		|| sys_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*���������*/
		||  sys_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*��ع���*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault == true/*���ѹ��ѹ>270V,���µ���ָܻ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring == true/*���ѹ��ѹ�ɻָ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseLV == true/*���ѹǷѹ,�ɻָ�*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseOC == true/*���������*/
		|| sys_ULP->stateInfoChange.sysModuleStateInfo.bits.phaseCShort == true/*�������·*/){
		ret = true;
		oneWaringResetSetFlag = true;
	}
	return ret;
}

/*
** ϵͳ Check
*/
void sysCheck(void){
	/*
	** �ڲ�ͨѶ�쳣(����60S��ȡ�������зֿذ���Ϣ)
	*/
	sysInnerComErr();
	/*
	** ���³�����ֹ��±�־
	*/
	updateChgDoorOverTempFlag();	
}

void SM_SysCtrlLogic_Task(void* p_arg){
	/*
	** �̼�����֮��ʼ������
	*/
	init_FirewarePara();

	Sleep(500);

	#if Debug_Flash_BugAnalysis == 1
	/*
	** Flash Bug����˵��
	*/	
	sFlash_BugAnalysis();
	#endif
	
	for(;;){
		/*
		** �̼�--Output
		*/
		firewareCtrAnalyze();		
		/*
		** ϵͳ Check
		*/
		sysCheck();
		/*
		** �̼�--Input
		*/
		firewareYxInAnalyze();

		/*
		** ͨѶ���������+������ز�������
		*/
		ComBoard_TaskSwitch();
		/*
		** ����ͨѶ��/��ذ��ļ�����:
		**		1.��������ͨѶС������
		**		2.�ȴ�ִ�лظ�����,���800ms֮������ϲ������л�
		*/
		ctrBoard_TaskSwitch();
		/*
		** �ָ��ϲ�Э��֮APPӦ�ò�Э�� 
		**		���Ʋ���:
		**				1.�ļ��������֮��,���ñ�־
		**				2.�ȴ��ظ�(��ʱ)800ms֮�����������л�
		*/
		ctrBoard_TaskRecover();	

		/*
		** CAN1 Tx Task
		*/
		CAN1_TxDataTask();

		/*
		** ���¿��Ź��Ĵ���--ι��
		*/
		watchdogUpdate();
		
		Sleep(10);
	}
}

/*
**�ṩϵͳ�������ϵͳ���ƽӿں���
*/
void SysCtrlLogic_Init(void){
	Thread_create(SM_SysCtrlLogic_Task, 0, (int32 *)&sysCtrlLogic_TaskStk[256-1], SysCtrlLogic_Prio);
}

