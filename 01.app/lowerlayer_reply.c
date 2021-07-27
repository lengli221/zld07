#include "includes.h"

static int32 lowerLayerReply_TaskStk[512] = {0};

/*
** ���������ļ���������--ʹ�ú���ָ��
*/
UpperLayerPara* llR_ULP = null;
SysParam* llR_SysPara = null;

/*
** �²�Э���������֮��������ļ�ʹ��extern 
*/
extern LowerLayerParam llParam;

/*
** �²�Э��֮������ʼ��
*/
static void LowerLayerReplyParam_Init(void){
	llR_ULP = getUpperLayerPara();
	llR_SysPara = get_SysParaPtr();
	/*
	** clear ͨѶ��/��ذ������汾��
	*/	
	clear_AppVer();
}

/*
** �²�Э��֮�ظ�֮��ѯ�����Ϣ
*/
void LLReply_ChkBatStateInfo(uint8 cmd,uint8* len,uint8* item){
	uint8 txlen = 0;
	uint8 socLinit = get_SocLimit();
	uint16 chgOverTemp = get_ChgOverTempLimit();
	uint16 chgTime = get_ChgTimeLimit();
	uint16 sysModulePara = 0;

	/*
	** ������
	** 1.ϵͳ����--�漰ͨѶ��ֹͣ��紦���߼�
	** 2.soc��ֵ
	** 3.�����������ֵ
	** 4.���ʱ����ֵ 0xD38F -- 1101 0011 1000 1111
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
	** �����Ӧ��־λ
	*/
	item[txlen] = cmd;
	txlen += sizeof(uint8);

	/*
	** �������
	*/
	*len = txlen;
}


/*
** �²�Э��֮��ѯ��ѯ�����Ϣ
*/
void LLReplay_PollingChkBatStateInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 0;
	uint8 tx[8] = {0};

	if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0
		&& (llR_ULP->stateInfoChange.sysLogic.batFileDownload&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0){		
		LLReply_ChkBatStateInfo(0x00,(uint8 *)&len, (uint8 *)&tx[0]);
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, LL_FunId_BatDoorState);
	}
	
	llParam.comBoardChk[deviceAddr].cnt++;
	if((TickOut((uint32 *)&llParam.comBoardChk[deviceAddr].itick, 60000) == true) && llParam.comBoardChk[deviceAddr].cnt >= LLParse_FrameCntMax){
		llParam.comBoardChk[deviceAddr].cnt = LLParse_FrameCntMax;
		if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0
			&& (llR_ULP->stateInfoChange.sysLogic.batFileDownload&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0){
			llParam.comBoardChk[deviceAddr].comAbn = true;/*ͨѶ�嶪ʧ*/
		}
	}
	
	deviceAddr++;
	if(deviceAddr == SysCtr_AllDoorNum){
		deviceAddr = 0;
	}
}

/*
** �²�Э��֮��ѯ����ϵͳ��Ϣ
*/
void LLReply_ChkChgSysInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 1;
	uint8 tx[8] = {0};

	if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0
		&& (llR_ULP->stateInfoChange.sysLogic.batFileDownload&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0){
		CAN_TransmitAnalysis(CAN_Port_1, len, (uint8 *)&tx[0], deviceAddr, LL_FunId_ChgSysInfo);
	}
	deviceAddr++;
	if(deviceAddr == SysCtr_AllDoorNum){
		deviceAddr = 0;
	}
}

/*
** ��ѯ�����Ϣ
*/
void LLReply_ChkBmsInfo(void){
	static uint8 deviceAddr = 0;
	uint8 len = 1;
	uint8 tx[8] = {0};
	uint8 i = 0;

	for(i=deviceAddr;i<SysCtr_AllDoorNum;i++){
		if(llParam.batDoor[i].batDoorStateInfo.bits.batOnline == true){
			deviceAddr = i;
			if((llR_ULP->stateInfoChange.sysLogic.comUpgr&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0
				&& (llR_ULP->stateInfoChange.sysLogic.batFileDownload&((DoorNumDefine)(((DoorNumDefine)0x01)<<deviceAddr))) == (DoorNumDefine)0){
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
** power On ����ļ��汾��Ϣ�Ƿ�һ����
** @param:
**			label->���
** @return:
**			true->��һ��-->������������
**			false->һ��
*/
bool powerOn_ChkVer(uint8 label){
	uint8 i = 0;
	uint16 binFileVer = 0;
	UpgrFilePara upgrTemp = {0};
	bool ret = false;

	if(label == 0){
		binFileVer = llR_SysPara->newSoftVer.comFile;/*ͨѶ���ⲿFlash�洢�����汾��*/
	}else{
		upgrTemp = get_UpgrBatFilePara(label - 1);
		binFileVer = upgrTemp.upgrFileType.softVer;/*�ⲿ��ع̼��ļ��汾��*/
	}
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*����:��֧��δ����һ�����������,�����ֿ�,�������¿��ļ�*/
		if(oneWaring_ForbidUpgrDownFile() == false){
			if(llParam.comBoardChk[i].comAbn == false){/*ͨѶ������*/
				if(binFileVer != llR_SysPara->newSoftVer.comApp[i]/*binFileVer > llR_SysPara->newSoftVer.comApp[i]*/&& binFileVer != 0){
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
	item[0] = label;

	*len = 0x01;
}

/*
** ��ѯApp�����汾�� -- FunID:0xF4 -- Ӧ��App����ݴ��� -- �㲥��ʽ����
** ���Ʋ���:
**			������ϵ��ж�ͨѶ��/��ع̼���app�����汾�Ƿ�ƫ��
*/
extern void update_ComUpgrQueue(UpgrFilePara upgr);
void chkComAppVer_BC(void){
	static uint8 deviceAddr = DstAddr_BoardCast_Define;/*�㲥������ַ*/
	uint8 len = 0;
	uint8 tx[8] = {0};
	static bool ret = false;/*�ϵ��־*/
	static uint32 itick = 0;
	static uint8 cnt = 0;
	static uint8 label = 0;
	UpgrFilePara upgr = {0};
	
	if(ret == false){
		if(TickOut((uint32 *)&itick, 400) == true){
			TickOut((uint32 *)&itick, 0);
			if(cnt++ >= /*sizeof(uint16)**/UpgradeLL_FrameMax){/*�޸Ĳ���:���5��,ÿ��400ms����--һ���̼�ʱ��2S*/
				cnt = 0;
				if(powerOn_ChkVer(label) == true){/*��������*/
					if(label == 0x00){/*ͨѶ��*/
						upgr = get_UpgrComFilePara();
					}else{
						upgr = get_UpgrBatFilePara(label - 1);
					}
					/*
					** �ݴ�����
					** 		�������:--����Ϸֿذ�����bootloader�����汾�Ѵ��ڰ汾����,���ڴ���������ǿ������
					*/
					upgr.upgrFileType.cmdType = 0x02/*0x01*/;/*ԭʼ���:�ϵ�����֮���޸ĳ�"����ʲô������ʽ�޸ĳ�-->��������"*/
					update_ComUpgrQueue(upgr);
				}
				label++;
				if(label >= get_batFireSize()/*get_SizeBatFile()*/ + 1){
					/*ret = true;*//*�޸�:��δ���������,ʵʱ��ѯ�汾���Ƿ�ƥ��*/
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
** �²�Э��֮�ظ��������ӿ�
*/
void SM_LowerLayerReply_Task(void* p_arg){
	static uint32 itick = 0;
	static uint32 itick1 = 0;
	ComBup comCurBupTemp = {0}; 
	/*
	** �²�Э��֮������ʼ��
	*/
	LowerLayerReplyParam_Init();
	
	Sleep(2000);
	
	for(;;){
		if(TickOut((uint32 *)&itick, 10/*25*/) == true){/*���䷢��ʱ��2ms*/
			TickOut((uint32 *)&itick, 0);
			/*
			** ��ѯApp�����汾�� -- FunID:0xF4 -- Ӧ��App����ݴ��� -- �㲥��ʽ����
			** ���Ʋ���:
			**			������ϵ��ж�ͨѶ��app�����汾�Ƿ�ƫ��
			*/			
			comCurBupTemp = getCurComBup();
			if(comCurBupTemp.binFileType.flag == 0){
				chkComAppVer_BC();
			}
			/*
			** �²�Э��֮��ѯ��ѯ�����Ϣ
			*/
			LLReplay_PollingChkBatStateInfo();
			/*
			** �²�Э��֮��ѯ����ϵͳ��Ϣ
			*/
			LLReply_ChkChgSysInfo();	
	
		}

		if(TickOut((uint32 *)&itick1, 100/*200*/) == true){/*���䷢��ʱ��2ms*/
			TickOut((uint32 *)&itick1, 0);
			/*
			** ��ѯ�����Ϣ
			*/
			LLReply_ChkBmsInfo();			
		}
		/*
		** CAN1 Tx Task
		*/
		CAN1_TxDataTask();
		/*
		** ���¿��Ź��Ĵ���--ι��
		*/
		watchdogUpdate();	
		
		Sleep(5);
	}
}

/*
**�ṩϵͳ���������²�Э���ѯ�ӿں���
*/
void LowerLayerReply_Init(void){
	Thread_create(SM_LowerLayerReply_Task, 0, 
		(int32 *)&lowerLayerReply_TaskStk[512-1], LowerLayerReplyTask_Prio);
}
