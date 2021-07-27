#include "includes.h"

static int32 lowerLayerParse_TaskStk[512] = {0};

/*
** ʹ��ָ�뷽ʽ���������ļ�����
*/
UpperLayerPara* llp_ULP = null;

/*
** �²�Э�������������
*/
LowerLayerParam llParam;

/*
** �²�Э��֮�����ӿ��ṩ����
*/
LowerLayerParam* getLowerLayerParaPtr(void){
	return(&llParam);
}

/*
** �²�Э��֮������ʼ��
*/
static void LowerLayerParam_Init(void){
	memset((uint8*)&llParam.compatibility[0].fireCode,0x00,sizeof(LowerLayerParam));
	llp_ULP = getUpperLayerPara();
}

/*
** �²�Э��֮��ز�״̬��Ϣ�ֶ�֮ʵʱ��ѹ���ݸ���
*/
#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
void FillBatDoorStateInfo_MeiTuan(uint8 fireCode,uint8* item,uint8 addr){
	uint8 len = 0;
	
	switch(fireCode){	
		case 0x01:/*������*/
		case 0x02:/*������*/
		case 0x03:/*��ë��*/
		case 0x04:/*�϶�*/
		case 0x05:/*���ն���*/
		case 0x06:/*�Ǻ�*/
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
		case 0x81:/*�Ǻ�*/
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
** �ж��쳣������(0b0000 0011 1100 1110)
*/
#define AbnDoorNumBits						(uint16)0x03CE

/*
** ����״̬��Ϣ���²�����(�ղ���,������,������,�쳣������,�ܲ���)
*/
static void updateDoorNum(void){
	DoorTypeNum doorTypeTemp = {0};
	uint8 i = 0;

	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*
		** ״̬λ�Ĺ���λ���ȼ����
		*/
		if((llParam.batDoor[i].batDoorStateInfo.flag & AbnDoorNumBits) != 0
			|| llParam.comBoardChk[i].comAbn == true){
			doorTypeTemp.abn++;
		}else{/*�޹���*/
			if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == false){/*��ز�����*/
				doorTypeTemp.idle++;
			}else{/*�������*/
				if(llParam.batDoor[i].bmsInfoMeiTuan.soc >= get_SocLimit()){
					doorTypeTemp.full++;
				}				
			}
		}
	}

	doorTypeTemp.ing = SysCtr_AllDoorNum - (doorTypeTemp.abn + doorTypeTemp.idle + doorTypeTemp.full);
	doorTypeTemp.total = SysCtr_AllDoorNum;

	/*
	** ��������,���¼�����ͳ����Ϣ����
	*/
	if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){
		memset((uint8*)&doorTypeTemp.idle,0,sizeof(DoorTypeNum));
		doorTypeTemp.abn = SysCtr_AllDoorNum;
		doorTypeTemp.total = SysCtr_AllDoorNum;
	}

	/*
	** �����ϲ�Э�����������
	*/
	llp_ULP->stateInfoChange.doorTypeNum = doorTypeTemp;
}

/*
** �ж�ͨѶ���Ƿ�ʧ��,ID�Ƿ��ظ�,����ϲ��ֶ���Ϣ
*/
void updateComIsAbn(void){
	uint8 i = 0;
	bool ret = false;
	static uint32 closeACtTick = 0;/*��AC֮�����¼���ֿ�ʧ��ʱ���־--�ϱ���־*/

	if(llp_ULP->stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){/*AC�Ͽ�*/
		TickOut((uint32 *)&closeACtTick, 0);
	}

	for(i = 0;i < SysCtr_AllDoorNum;i++){
		if(llParam.comBoardChk[i].comAbn == true){/*ͨѶ��ʧ��*/
			/*Ӳ��������������ͨѶ��ʧ�����*/
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
	** ����ϵͳ״̬λ
	*/
	if(llp_ULP->stateInfoChange.sysLogic.comIsOnline != 0){
		llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comErr = true;
	}else{
		llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comErr = false;
	}	

	llp_ULP->stateInfoChange.sysModuleStateInfo.bits.comIdRep = ret;
}

/*
** �����²�->��ز�״̬��Ϣ->����ϲ�->����״̬��Ϣ���
*/
extern void LLReply_ChkBatStateInfo(uint8 cmd,uint8* len,uint8* item);
void stateInfoSwitch(uint8 addr){
	volatile uint8 temp = 0;
	uint8 tx[8] = {0};
	uint8 len = 0;
	
	/*
	** ���״̬��Ϣ:
	**			Bit0:��1--������� Bit1:��1--��زճ�������� Bit2:��1--��ع��� Bit3:��1--���������(�������,����1min�޵�������)
	**			Bit4:��1--������߿���AC Bit5:��1--Զ�̶�AC Bit6:��1--ϵͳ���϶ϳ����AC Bit7:BMSͨѶ��ID�ظ� 
	**			Bit8:��1--��ع��� Bit9:��1--BMS���ջ�,��ع��� Bit10:��1--��������Ƿ���� Bit11:��1--��������Ƿ�ɹ�
	*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == true){
		llp_ULP->stateInfoChange.sysLogic.batOnline |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batOnline &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
		/*��ز�����--������е�������Ϣ*/
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

	/*��ع���*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.batTrans == true){
		llp_ULP->stateInfoChange.sysLogic.batFault |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batFault &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*���ͨѶ����*/
	if( llParam.batDoor[addr].batDoorStateInfo.bits.bmsErr == true){
		llp_ULP->stateInfoChange.sysLogic.batComErr |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.batComErr &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*�ֿع���*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.comIsFault == true){
		llp_ULP->stateInfoChange.sysLogic.comFault |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.comFault &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}	

	/*����쳣*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.chargingErr == true){
		llp_ULP->stateInfoChange.sysLogic.chargingErr |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	}else{
		llp_ULP->stateInfoChange.sysLogic.chargingErr &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}

	/*��ص���*/
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
** �²�Э��֮��ز�״̬����
*/
void LLParse_BatDoorState(uint8 rxlen,uint8* item,uint8 addr){
	uint8 len = 0;
	
	/*
	** ��������--ͨѶС��һ��ID�ظ����ٸ�����Ӧ����--�ж�ͨѶ�쳣
	*/
	if(llParam.batDoor[addr].batDoorStateInfo.bits.comIdRep == false){/*ͨѶС��δ�ظ�*/
		/*
		** ������ ��ز�״̬��Ϣ ��س��̴��� ʵʱ��ѹ(����)/����ܵ�ѹ(���ɹ�) ʵʱ���� SOC(����)/SOH(���ɹ�)
		*/
		/*
		** ���״̬��Ϣ:
		**			Bit0:��1--������� Bit1:��1--��زճ�������� Bit2:��1--��ع��� Bit3:��1--���������(�������,����1min�޵�������)
		**			Bit4:��1--������߿���AC Bit5:��1--Զ�̶�AC Bit6:��1--ϵͳ���϶ϳ����AC Bit7:BMSͨѶ��ID�ظ� 
		**			Bit8:��1--��ط��� Bit9:��1--BMS���ջ�,��ع��� Bit10:��1--��������Ƿ���� Bit11:��1--��������Ƿ�ɹ�
		*/
		
		memcpy((uint8*)&llParam.batDoor[addr].batDoorStateInfo.flag,(uint8*)&item[len],sizeof(uint16));
		len += sizeof(uint16);
		/*
		** ��س��̴��� �ֶκ�(����)�������ݽ��ڵ�����߸���
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
		** ����������ر�־����
		*/
		if(llp_ULP->stateInfoChange.sysLogic.batUpgr & (DoorNumDefine)((DoorNumDefine)0x01<<addr)){
			if(llParam.batDoor[addr].batDoorStateInfo.bits.batUpgrIsFinsh == true
				|| llParam.batDoor[addr].batDoorStateInfo.bits.batOnline == false/*��ز�����*/){
				llp_ULP->stateInfoChange.sysLogic.batUpgr &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
				if(llParam.batDoor[addr].batDoorStateInfo.bits.batUpgrIsOk == true){
					llp_ULP->stateInfoChange.sysLogic.batUpgrIsOk |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
				}
			}
		}
		/*
		** �����²�->��ز�״̬��Ϣ->����ϲ�->����״̬��Ϣ���
		*/
		stateInfoSwitch(addr);
	}
}

/*
** ����:20210115--�Ź��������� �ֿ��������� -- �ڳ��������,��ع���(����������ִ���--�ܾ�����)
*/
bool chk_IsExitBatChargerOT(void){
	bool flag = false;

	if(llp_ULP->stateInfoChange.sysLogic.chgOverTemp != 0/*���������*/
		|| llp_ULP->stateInfoChange.sysLogic.batIsErr != 0 /*��ع���*/){
		flag = true;
	}

	return flag;
}

/*
** �������״̬
*/
extern DoorNumDefine hmiUpgrIsFinshFlag;
void LLParse_UpgrState(uint8 rxlen,uint8* item,uint8 addr){
	static uint8 frame[SysCtr_AllDoorNum] = {0};
	uint8 len = 0;
	uint8 frameLabel = 0;
	uint8 txlen = 1;
	uint8 tx[8] = {0};
	
	/*
	** ֡��ʶ ֡���ݳ�����ʱδʹ��
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);
	
	switch(frameLabel){/*֡��ʶ*/
		case 0x01:/*���̴���,����ͺ�,���ԭ����汾��,��ش���������汾��*/
			
			break;
		case 0x02:/*Ӳ���汾��,���ID*/
			
			break;
		case 0x03:
			
			break;
		case 0x04:
			
			break;
	}

	/*
	** ���֡�Ƿ��������
	*/
	frame[addr] |= 0x01<<(frameLabel - 1);
	if(frame[addr] == 0x0F){
		frame[addr] = 0;
		/*���ִ��ڵ�ع���,��������� -- �ܾ����� 0x02*/
		if(chk_IsExitBatChargerOT() == true){
			tx[0] = 0x02;/*�ܾ�����*/
		}else{
			llp_ULP->stateInfoChange.sysLogic.batUpgr |= (DoorNumDefine)((DoorNumDefine)0x01<<addr);
			llp_ULP->stateInfoChange.sysLogic.batUpgrIsOk &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
			hmiUpgrIsFinshFlag &= (DoorNumDefine)~((DoorNumDefine)0x01<<addr);
			tx[0] = 0x01;/*��������*/
		}
		/*
		** �ظ��ֿذ�
		*/
		CAN_TransmitAnalysis(CAN_Port_1, txlen, (uint8 *)&tx[0], addr, LL_FunId_BatUpgrState);
	}
}

/*
** ����ϵͳ��Ϣ
*/
void LLPrse_ChgSysInfo(uint8 rxlen,uint8* item,uint8 addr){
	uint8 len = 0;
	/*
	** ���״̬��Ϣ
	*/
	/*llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = item[len]&0x01;*/
	if((item[len]&0x01) == 0x01){/*�������*/
		llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = true;
	}
	len += sizeof(uint8);

	/*
	** ��ؽ���ʱSOC ��س��ʱ�� ��ؽ���ʱ��
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
	** ���µ�ز���ʱ�������־
	*/
	if(llParam.batDoor[addr].batDoorSysPara.insertTime >= get_ChgTimeLimit()){
		llp_ULP->stateInfoChange.sysLogic.batChgOTime |= (DoorNumDefine)((DoorNumDefine)1<<addr); 
	}else{
		llp_ULP->stateInfoChange.sysLogic.batChgOTime &= (DoorNumDefine)~((DoorNumDefine)1<<addr);
	}
	
	/*
	** ������¶�
	*/
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.chargerTemp,(uint8*)&item[len],sizeof(uint16));
	len += sizeof(uint16);
}

/*
** �²�Э��֮BMS��Ϣ֮���BMS�ֶ���Ϣ
*/
#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
void FillBatInfo_Data_MetTuan(uint8* item,uint8 frameLabel,uint8 addr){
	uint8 len = 0;
	uint8 dataLen  = 0;
	
	/*
	** �Ż���������߼�--���յ���ϱ���Ϣ���Ͽɵ������
	** ��ȷ:�Ƿ���ڲ�ѯʱ��һ�µ��������л��쳣,ͨѶ�Ƿ����
	*/
	llParam.batDoor[addr].batDoorStateInfo.bits.batOnline = true;
	
	switch(frameLabel){
		case 0x01:/*������:ʵʱ��ѹ,ʵʱ����,SOC,���̴���*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			llParam.batDoor[addr].bmsInfoMeiTuan.soc = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoMeiTuan.firmCode = item[len];
			len += sizeof(uint8);		
			/*����:���̴���*/
			llParam.compatibility[addr].fireCode = llParam.batDoor[addr].bmsInfoMeiTuan.firmCode;
			break;
		case 0x02:/*������:����״̬ ����״̬ SOH SOP*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.pState.flag,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			llParam.batDoor[addr].bmsInfoMeiTuan.faultState.flag = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoMeiTuan.soh = item[len];
			len += sizeof(uint8);	
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.sop,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			break;
		case 0x03:/*������:���ڵ�ص�ѹ 16*2*/
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
		case 0x09:/*������:���ID 16*/
		case 0x0A:	
		case 0x0B:
			dataLen = frameLabel != (uint8)0x0B?0x06:0x04;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.batID[(frameLabel-0x09)*0x06],
				(uint8*)&item[len],dataLen);
			len += dataLen;
			break;
		case 0x0C:/*������:������,���Ŵ���,��·����*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.chgNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.disChgNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.shortNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);			
			break;
		case 0x0D:/*ѭ������ ʣ������ �������*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.circNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.surplusCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.designCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);			
			break;
		case 0x0E:/*������:�������¶�,�������¶�,MOS����¶�,PCB�¶�,��ʷ�������¶� ����״̬*/
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
		case 0x0F:/*������:Ӳ���汾,����汾,BOOT�汾��,����ͺ�*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.hardVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoMeiTuan.softVer,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);	
			llParam.batDoor[addr].bmsInfoMeiTuan.bootVer = item[len];
			len += sizeof(uint8);		
			llParam.batDoor[addr].bmsInfoMeiTuan.batType = item[len];
			len += sizeof(uint8);				
			break;
		case 0x10:/*������:��ع���ģʽ,��س��ģʽ,MOS״̬*/
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
		case 0x01:/*������*/
		case 0x02:/*������*/
		case 0x03:/*��ë��*/
		case 0x04:/*�϶�*/
		case 0x05:/*���ն���*/
		case 0x06:/*�Ǻ�*/
		case 0x07:/*ATL*/
		case 0x08:/*CATL*/
			FillBatInfo_Data_MetTuan((uint8 *)&item[0], frameLabel, addr);
			break;
		default:
			/*
			** �Ż�����,���̱��Ϊ��,�������BMS�����Ϣ
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
		case 0x01:/*������:����ܵ�ѹ,ʵʱ����,��������ٷֱ�,SOH*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batTotalVol,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.realCur,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);	
			llParam.batDoor[addr].bmsInfoPineCone.relaCapPer = item[len];
			len += sizeof(uint8);
			llParam.batDoor[addr].bmsInfoPineCone.soh = item[len];
			len += sizeof(uint8);			
			break;
		case 0x02:/*������:ʣ������,��������,ѭ������*/
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.surplusCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.fullCap,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.circNum,(uint8*)&item[len],sizeof(uint16));
			len += sizeof(uint16);				
			break;
		case 0x03:/*������:���ڵ�ص�ѹ 13*2*/
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
			dataLen = frameLabel == 0x07?0x06:0x02;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.singleBatVol[(frameLabel-0x03)*(0x06/sizeof(uint16))],
				(uint8*)&item[len],dataLen);
			len += dataLen;
			break;
		case 0x08:/*������:ID���� 20*/
		case 0x09:
		case 0x0A:
		case 0x0B:	
			dataLen = frameLabel == 0x0B?0x06:0x02;
			memcpy((uint8*)&llParam.batDoor[addr].bmsInfoPineCone.batID[(frameLabel-0x03)*0x06],
				(uint8*)&item[len],dataLen);
			len += dataLen;			
			break;
		case 0x0C:/*������:����¶�,�汾��*/
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
		case 0x81:/*�Ǻ�*/
			FillBatInfo_Data_PineCone((uint8 *)&item[0], frameLabel, addr);
			break;
		default:
			
			break;
	}
}
#endif

/*
** �²�Э��֮BMS��Ϣ
*/
void LLParse_BatInfo(uint8 rxlen,uint8* item,uint8 addr){
	uint8 frameLabel = 0;
	uint8 len = 0;
	
	/*
	** ������ ֡��ʶ ���������� ����������
	*/
	frameLabel = item[len];
	len += sizeof(uint8);
	len += sizeof(uint8);/*ע��:������������ʱδʹ��*/

	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
		FillBatInfo_MetTuan(llParam.compatibility[addr].fireCode, (uint8 *)&item[len], frameLabel, addr);
	#endif

	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
		FillBatInfo_PineCone(llParam.compatibility[addr].fireCode, (uint8 *)&item[len], frameLabel, addr);
	#endif
}

/*
** ����ͨѶ������汾��
*/
void update_ComSfotVer(uint16 ver,uint8 addr){
	uint8 len = 0;
	uint8 charVer[4] = {0};/*����汾��*/
	#if CtrHardware == CtrHardware_Andriod
		uint8 charVerHard[4] = {"0200"};/*Ӳ���汾��*/
	#else
		uint8 charVerHard[4] = {"0100"};/*Ӳ���汾��*/
	#endif
	
	len = _itoa(ver/*temp*/, (char*)&charVer[0]);
	memset((uint8*)&llParam.batDoor[addr].batDoorSysPara.comSoftVer[0],'0',4);
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.comSoftVer[4-len],(uint8*)&charVer[0],len);
	memcpy((uint8*)&llParam.batDoor[addr].batDoorSysPara.comHareVer[0],(uint8*)&charVerHard[0],4);
}

/*
** ��������汾 
*/
extern uint8 assignLabel;
extern uint16 assignSoftVer[SysCtr_AllDoorNum];
void LLParse_UpdateSoftVer(uint8 rxlen,uint8* item,uint8 addr){
	uint32 comAppRunVer = {0};
	uint16 verTemp = 0;
	uint8 len = 0;
	uint8 label = 0;

	/*
	** ���
	*/
	label = item[len];
	len += sizeof(uint8);

	/*
	** ����汾��
	*/
	memcpy((uint8*)&verTemp,(uint8*)&item[len],sizeof(uint16));
	len += sizeof(uint16);

	/*
	** ����ͨѶ������汾��
	*/
	if(label == 0x00){/*ͨѶ��*/
		update_ComSfotVer(verTemp,addr);
	}

	/*
	** ָ�����������ѯָ��
	*/
	if(assignLabel == label){
		assignSoftVer[addr] = verTemp;
	}
	

	/*
	** get ��ǰ���ư屣�����Ӧ��ַͨѶ������
	*/
	comAppRunVer = getComRunAppVer(addr);
	
	if(comAppRunVer != verTemp){
		/*
		** ϵͳ����֮����ͨѶС��App���а汾
		*/
		setComRunAppVer(verTemp, addr);
	}

	/*
	**	�����ϱ��ӿ�ͨѶ����������汾�͵�ع̼�������汾
	*/
	llp_ULP->runComVer[label][addr] = verTemp;
}

// /*
// ** �²�Э��֮ϵͳģ���б�
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
** �²�Э��֮�����������ӿ�
*/
void SM_LowerLayerParse_Task(void* p_arg){
// 	CanRxMsg rxMsg = {0};
// 	uint8 funId = 0;
// 	uint8 i=0;
// 	uint8 deviceAddr = 0;

	/*
	** �²�Э��֮������ʼ��
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
// 					** �ж�ͨѶ�Ƿ�ʧ��
// 					*/
// 					llParam.comBoardChk[deviceAddr].cnt = 0;
// 					llParam.comBoardChk[deviceAddr].comAbn = false;
// 					break;
// 				}
// 			}
// 			/*
// 			** ���ſ�ι��
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
// 					** �ж�ͨѶ�Ƿ�ʧ��
// 					*/
// 					llParam.comBoardChk[deviceAddr].cnt = 0;
// 					llParam.comBoardChk[deviceAddr].comAbn = false;
// 					break;
// 				}
// 			}
// 			/*
// 			** ���ſ�ι��
// 			*/
// 			watchdogUpdate();
// 		}		

		/*
		** �²�Э��֮���ݽ���֮����������
		*/
		lowerUpgrFunAnalyze();

		/*
		** �ж�ͨѶ���Ƿ�ʧ��,����ϲ��ֶ���Ϣ
		*/
		updateComIsAbn();

		/*
		** ����״̬��Ϣ���²�����(�ղ���,������,������,�쳣������,�ܲ���)
		*/
		updateDoorNum();		

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
**�ṩϵͳ��������²�Э������ӿں���
*/
void LowerLayerParse_Init(void){
	Thread_create(SM_LowerLayerParse_Task, 0,
		(int32 *)&lowerLayerParse_TaskStk[512-1], (int8)LowerLayerParseTask_Prio);
}

