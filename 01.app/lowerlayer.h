#ifndef __LOWERLAYER_H_
#define __LOWERLAYER_H_

/*
** ϵͳ���Ʋ���֮����#define����
** 1.֡��֡����
** 
*/
#define LLParse_FrameCntMax							(uint8)0x0A//0x05	

/*
** ϵͳģ�������Ϣ֮����λ����
*/
#define SysModuleStateInfo_MaskBit_Upgr			(uint16)0xD38F

/*
** �²�Э��֮ϵͳģ�鴦��֮���ܺ���ID����
** 1.��ز�״̬
*/
#define LL_FunId_BatDoorState						(uint8)0x01
#define LL_FunId_BatUpgrState						(uint8)0x02
#define LL_FunId_ChgSysInfo							(uint8)0x03
#define LL_FunId_BmsInfo								(uint8)0x80


/*
** �����ڴ����
*/
#pragma pack(1)
typedef struct{
	uint8 fireCode;/*���̴���--���ݲ�ͬ�ͻ���BmsЭ��*/
}Compatibility;

typedef struct{
	uint32 itick;/*���ڳ�ʱ*/
	uint8 cnt;
	bool comAbn;/*ͨѶ��ʧ*/
}ComBoardChk;

typedef union{
	uint16 flag;
	struct{
		uint16 batOnline:1;/*�������*/
		uint16 batDoorChargerOT:1;/*��زֳ��������*/
		uint16 batOT:1;/*��ع���*/
		uint16 chargerErr:1;/*���������(�������,����1min�޵�������)*/
		uint16 batOnlineOpenAC:1;/*������߿���AC*/
		uint16 remoteCloseAC:1;/*Զ�̶�AC*/
		uint16 sysErrCloseAC:1;/*ϵͳ���϶ϳ����AC*/
		uint16 comIdRep:1;/*ͨѶ��ID�ظ�*/
		uint16 batTrans:1;/*��ط���--����Ϊ��ع���*/
		uint16 bmsErr:1;/*BMS���ջ�,��ع���*/
		uint16 batUpgrIsFinsh:1;/*���������*/
		uint16 batUpgrIsOk:1;/*����Ƿ������ɹ�*/
		uint16 comIsFault:1;/*�ֿع���*/
		uint16 chargingErr:1;/*����쳣*/
		uint16 batLowTemp:1;/*��ص���*/
		uint16 oneErrRecover:1;/*һ�������ֿظ�λ��־*/
	}bits;
}BatDoorStateInfo;


typedef struct{
	uint8 comHareVer[4];/*�ֿذ�Ӳ���汾��-->String*/
	uint8 comSoftVer[4];/*�ֿذ������汾��-->String*/
	uint8 chgBefSoc;/*��ؽ���ʱSOC*/
	uint16 chgTime;/*���ʱ��*/
	uint16 insertTime;/*����ʱ��*/
	int16 chargerTemp;/*������¶�*/
}BatDoorSysPara;

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
typedef union{
	uint16 flag;
	struct{
		uint16 res:16;
	}bits;
}PState;

typedef union{
	uint8 flag;
	struct{
		uint8 res:8;
	}bits;
}FaultState;

typedef union{
	uint8 flag;
	struct{
		uint8 res:8;
	}bits;
}MosState;

typedef struct{
	uint16 realVol;/*ʵʱ��ѹ*/
	int16 realCur;/*ʵʱ����*/
	uint16 surplusCap;/*ʣ������*/
	uint16 circNum;/*ѭ������*/
	uint16 singleBatVol[16];/*���ڵ�ص�ѹ*/
	uint8 soh;
	uint8 batID[16];/*���ID*/
	uint8 soc;
	uint16 sop;/*���sop �������*/
	uint8 batWorkMode;/*��ع���ģʽ*/
	uint8 batChgMode;/*��س��ģʽ*/
	uint8 workState;/*����״̬*/
	PState pState;/*����״̬*/
	FaultState faultState;/*����״̬*/
	MosState mosState;/*MOS״̬*/
	int8 batMaxTemp;/*�������¶�*/
	int8 batMinTemp;/*�������¶�*/
	int8 mosMaxTemp;/*MOS����¶�*/
	int8 pcbTemp;/*PCB�¶�*/
	uint16 chgNum;/*������*/
	uint16 disChgNum;/*���Ŵ���*/
	uint16 shortNum;/*��·����*/
	int8 hisMaxBatTemp;/*��ʷ������¶�*/
	uint8 bootVer;/*BOOT�汾��*/
	uint8 firmCode;/*���̴���*/
	uint16 hardVer;/*Ӳ���汾*/
	uint16 softVer;/*�����汾*/
	uint16 designCap;/*�������*/
	uint8 batType;/*����ͺ�*/
}BmsInfoMeiTuan;
#endif

#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
typedef struct{
	uint16 batTemp;/*����¶�*/
	uint16 batTotalVol;/*����ܵ�ѹ*/
	uint16 realCur;/*ʵʱ����*/
	uint8 relaCapPer;/*��������ٷֱ�*/
	uint16 surplusCap;/*ʣ������*/
	uint16 fullCap;/*��������*/
	uint16 circNum;/*ѭ������*/
	uint16 singleBatVol[13];/*���ڵ�ص�ѹ*/
	uint8 soh;
	uint8 ver[3];/*�汾��*/
	uint8 batID[20];/*���ID*/
}BmsInfoPineCone;
#endif

typedef struct{
	BatDoorStateInfo batDoorStateInfo;
	BatDoorSysPara batDoorSysPara;
	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_MeiTuan
		BmsInfoMeiTuan bmsInfoMeiTuan;
	#endif
	#if Client_Choice == Client_Choice_All ||  Client_Choice == Client_Choice_PineCone
		BmsInfoPineCone bmsInfoPineCone;
	#endif
}BatDoor;

typedef struct{
	Compatibility compatibility[SysCtr_AllDoorNum];
	ComBoardChk comBoardChk[SysCtr_AllDoorNum];
	BatDoor batDoor[SysCtr_AllDoorNum];
}LowerLayerParam;
#pragma pack()

/*
** �²�Э��֮ϵͳģ�鴦���б��ṹ��
*/
typedef struct{
	uint8 funId;/*���ܺ���ID*/
	void (*handle)(uint8,uint8*,uint8);/*1.���ݳ��� 2.������ 3.�豸��ַ*/
}LowerLayerFunTable;


/*
** �²�Э��֮�����ӿ��ṩ����
*/
LowerLayerParam* getLowerLayerParaPtr(void);
/*
**�ṩϵͳ���������²�Э������ӿں���
*/
void LowerLayerParse_Init(void);
/*
**�ṩϵͳ���������²�Э���ѯ�ӿں���
*/
void LowerLayerReply_Init(void);
#endif
