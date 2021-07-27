#ifndef __SYSPARAM_H_
#define __SYSPARAM_H_

#include "lcdData_DW.h"

/****************************ϵͳ������ַ����Backup��EEPROM**********************************/
#define UpgradeKey_Head								(uint8)0xA5

/*backup���������ַ����*/
#define ByteReserveOffset								(0x04)/*Ԥ��֮������Կ����*/

#define UpgradeKey_Addr								0x0000
#define UpgradeKey_Len_Addr							sizeof(UpgradeKey)+0x03/*head+crc16*/

#define UpgradeVer_Addr								ByteReserveOffset + UpgradeKey_Addr + UpgradeKey_Len_Addr
#define UpgradeVer_Len_Addr							sizeof(uint32) + 0x03

#define UpgradeFileNameLen_Addr					ByteReserveOffset+UpgradeVer_Addr+UpgradeVer_Len_Addr
#define UpgradeFileNameLen_Len_Addr				sizeof(uint16)+0x03

#define UpgradeFileName_Addr							ByteReserveOffset + UpgradeFileNameLen_Addr + UpgradeFileNameLen_Len_Addr
#define UpgradeFileName_Len_Addr					128 + 0x03

#define UpgradeOffset_Addr							0x00B4/*ƫ�Ƶ�ַ:180*//*0x100*/

/****************************************************************************************************************************************/
/*
** ���ñ���ע����Ϣ������ַ
*/
 #define Register_LogoutInfo_Addr					UpgradeOffset_Addr
 #define Register_LogoutInfo_Len						(sizeof(uint8) + sizeof(uint16))

 #define LocalRemoteDomain_Addr						Register_LogoutInfo_Addr + Register_LogoutInfo_Len
#define LocalRemoteDomain_Len						(128 + sizeof(uint16))
#define SysDeviceID_Addr								LocalRemoteDomain_Addr + LocalRemoteDomain_Len
#define SysDeviceId_Len									sizeof(DeviceId) + sizeof(uint16)

#define WifiNamePara_Addr								(SysDeviceID_Addr + SysDeviceId_Len)
#define WifiNamePara_Len								(32+sizeof(uint16))

#define WifiPwPara_Addr								(WifiNamePara_Addr + WifiNamePara_Len)
#define WifiPwPara_Len									(32+sizeof(uint16))

/*
** ��ذ�����(Ӳ���汾��,���ID)����
*/
#define UpgrBatFileNumAddr							(WifiPwPara_Addr + WifiPwPara_Len)
#define UpgrBatFileNumLen								sizeof(uint16) + sizeof(uint16)/*CRCУ��*/

/*-------------------------------------------ʹ����λ�����������ѹ----------------------------------------------------------------*/
#define AdjusrPhaseVoltAddr							(UpgrBatFileNumAddr+UpgrBatFileNumLen)
#define AdjusrPhaseVoltLen								(((sizeof(bool)+sizeof(float)+sizeof(float))*3)+sizeof(uint16))
/*---------------------------------------------------------------------------------------------------------------------------------*/

/*
** �ֿذ�(ͨѶ������bin�ļ�����)
*/
#define UpgrBinFilePara_CommAddr					(AdjusrPhaseVoltAddr+AdjusrPhaseVoltLen)/*UpgrBatFileNumAddr + UpgrBatFileNumLen*/
/*
** ��ذ�
*/
#define UpgrBinFilePara_BatAddr(loc)							((UpgrBinFilePara_CommAddr + UpgrBinFilePara_Len) + (loc*UpgrBinFilePara_Len))
/*
** bin�ļ���ز�������
*/
#define UpgrBinFilePara_Len							(sizeof(UpgrFilePara) + sizeof(uint16))/*CRC16У��*/

/*
** Ӧ�ò��ַ����˵��:
** 		1.��ʼ��ַ:Ԥ����������44�ֵ�� 
**		�޸Ĳ���:	UpgrBinFilePara_Len * (44 + 1) (ͨѶ��) + ���ư�(0xB4) + regLog(3) + batSize(4) + 216(����,�豸ID,Wifi����) = 3328 = 0x0D00
**				+ 29(��������У׼ϵ��) --> 3357 = 0x0D1D
*/
#define App_InitBaseAddr										0x0D1D

#define TSave_Addr												App_InitBaseAddr
#define TSave_Len													(sizeof(Tsave)+ sizeof(uint16))/*CRC16У��*/
#define SocLimit_Addr												TSave_Addr + TSave_Len
#define SocLimit_Len												(sizeof(uint8) + sizeof(uint16))
#define ChgTimeLimit_Addr										SocLimit_Addr + SocLimit_Len
#define ChgTimeLimit_Len										(sizeof(uint16) + sizeof(uint16))
#define ChgOverTempLimit_Addr								ChgTimeLimit_Addr + ChgTimeLimit_Len
#define ChgOverTempLimit_Len									(sizeof(uint16) + sizeof(uint16))
#define ChgDoorOTLimit_Addr									ChgOverTempLimit_Addr + ChgOverTempLimit_Len
#define ChgDoorOTLimit_Len									(sizeof(uint16) + sizeof(uint16))


/****����:Ŀǰ����������ַ���ݽ��洢��Backup*********************/
typedef struct{
	uint8 key[6];
}UpgradeKey;

/*
** ϵͳ����֮���ر����������֮�ṹ������
*/
#pragma pack(1)
typedef struct{
	uint16 ctr;/*���ư����а汾��*/
	uint16 comFile;/*�ⲿFlash�б����ͨѶ���ļ�����汾��*/
	uint16 comApp[SysCtr_AllDoorNum];/*ͨѶ������App�汾��--�ֿذ汾��,��ذ汾*/
}NewSoftVer;

typedef struct{
	uint16 base;/*ϵͳ�¶Ȼ�׼ֵ*/
	uint16 diff;/*ϵͳ�ز��¶�*/
}Tsave;

/*
** ����
*/
typedef struct{
	uint8 local[64];
	uint8 remote[64];
}SysDomain;

typedef struct{
	bool registerFlag;/*�����ע���Ƿ�ɹ�*/
}Serve;

typedef struct{
	uint8 name[32];
	uint8 pw[32];
}WifiSet;

typedef struct{
	NewSoftVer newSoftVer;/*����汾�ſ���*/
	uint8 socLimit;/*soc��ֵ*/
	uint16 chgTimeOut;/*���ʱ�䳬ʱ*/
	uint16 chgOverTemp;/*���������*/
	uint16 chgDoorOTemp;/*���ֹ���*/
	Tsave tsave;/*ϵͳ�¶Ȳ���--��׼�¶�/�ز�*/
	uint16 batFireSize;/*��ذ��̼�����*/
	SysDomain sysDomain;/*ϵͳ����:����/Զ��*/
	DeviceId  deviceId;/*�豸ID*/
	Serve serve;/*����˲���*/
	WifiSet wifiSet;/*Wifi ��������*/
}SysParam;
#pragma pack()


/*
** ʹ��ָ���ṩ�ⲿ�ļ����ʱ���
*/
SysParam* get_SysParaPtr(void);
/*
** get Ctr�ļ���
*/
void get_CtrFileName(uint8* rx,uint8* rxlen);
/*
** clear ͨѶ��/��ذ�����汾��
*/
void clear_AppVer(void);
/*
** get ͨѶ��/��ذ�����App��汾��
*/
uint16 getComRunAppVer(uint8 addr);
/*
** set ͨѶ��/��ذ�����App��汾��
*/
void setComRunAppVer(uint16 ver,uint8 addr);
/*
** get Ctr SoftVer
*/
uint16 get_CtrSoftVer(void);
/*
** ���¿��ư����/ͨѶ������汾��
*/
void updateSoftVerPara(void);
/*
** set ������Կ����
*/
void set_UpgrCtrKey(void);
/*
** get Ctr����汾��
*/
uint32 get_CtrVer(void);
/*
** get��ذ��洢���еĴ�С(����)
*/
uint16 get_SizeBatFile(void);
/*
** get ��ع̼�������
** 		����:�����ڲ�ѯ��ذ�����
*/
uint16 get_batFireSize(void);
/*
** set��ذ��洢���еĴ�С(����)
*/
void set_SizeBatFile(uint16 szie);
/*
** ��ʼ����ذ�(Ӳ���汾��,ID)����
*/
void init_BatPara(void);
/*
** set����bin�ļ�����
** ����:���ݵ�ذ���ϸ��Ϣ(Ӳ���汾/ID)�洢ָ����ַλ��
*/
void set_UpgrBinFilePara(UpgrFilePara upgr);
/*
** set Assign��ذ�bin�ļ�����
*/
void set_AssignUpgrBinFilePara(UpgrFilePara upgr,int16 loc);
/*
** get����bin�ļ�����--��ذ�
** @para:
**		loc:ָ��λ��--(0,max(get_SizeBatFile()))
*/
UpgrFilePara get_UpgrBatFilePara(uint16 loc);
/*
** get����bin�ļ�����--ͨѶ��
*/
UpgrFilePara get_UpgrComFilePara(void);
/*
** get��ذ�λ����Ϣ
** @return:
** 			-1:�޵�ذ���Ϣ--�Ƿ����
**			-2:������ʧ
** ����:
**		1.���������ж��Ƿ���ڵ�ذ���Ӧ�ͺ�����
**		2.��λ����Ϣ��ȡEeprom��ַ��Ϣ
*/
int16 get_BatLocation(UpgrFilePara upgr);
/*
** set ϵͳ�¶�ֵ(��׼+�ز�)
*/
void set_Tsave(Tsave ts);
/*
** get ϵͳ�¶�ֵ(��׼+�ز�)
*/
Tsave get_Tsave(void);
/*
** set SOC��ֵ
*/
void set_SocLimit(uint8 soc);
/*
** get SOC��ֵ
*/
uint8 get_SocLimit(void);
/*
** set ���ʱ����ֵ
*/
void set_ChgTimeLimit(uint16 time);
/*
** get ���ʱ����ֵ
*/
uint16 get_ChgTimeLimit(void);
/*
** set �����������ֵ
*/
void set_ChgOverTempLimit(uint16 temp);
/*
** get �����������ֵ
*/
uint16 get_ChgOverTempLimit(void);
/*
** set ������ֹ�����ֵ
*/
void set_ChgDoorOTLimit(uint16 temp);
/*
** get ������ֹ�����ֵ
*/
uint16 get_ChgDoorOTLimit(void);
/*
** set ����
*/
void set_SysDomain(SysDomain s);
/*
** get ����
*/
SysDomain get_SysDomain(void);
/*
** set Sys �豸ID
*/
void set_SysDeviceId(DeviceId id);
/*
** get Sys �豸ID
*/
DeviceId get_SysDeviceId(void);
/*
** get ����˲���
*/
Serve get_ServePara(void);
/*
** set����˲���
*/
void setServePara(Serve s);
/*
** get WIFI Name PW
*/
WifiSet get_WifiNamePwSysSet(void);
/*
** set Wifi Name  
*/
void set_WifiNameSysSet(uint8* space);
/*
** set Wifi Pw 
*/
void set_WifiPwSysSet(uint8* space);
/*
** ϵͳ������ʼ��
*/
void init_SysPara(void);
#endif

