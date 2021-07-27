#include "includes.h"

/*
** ��ȡ����(�汾��,�ļ�������,�ļ���)�洢EEPROM��ַ
*/
extern UpgrFilePara upgrComBinFilePara;
extern UpgrFilePara upgrBatBinFilePara[44];
uint32 get_UpgrEepromAddr(UpgrFilePara upgr){
	uint32 addr = 0;
	int16 loc = 0;

	if(upgr.upgrFileType.board == 0/*�ݴ���*/||upgr.upgrFileType.board == ComBoradType){/*ͨѶ��*/
		addr = UpgrBinFilePara_CommAddr;
		upgrComBinFilePara = upgr;
	}else{/*��ذ�*/
		loc = get_BatLocation(upgr);
		addr = UpgrBinFilePara_BatAddr(loc);
		upgrBatBinFilePara[loc] = upgr;
	}

	return addr;
}

/*
** ��ȡ�������(Ӳ���汾��,���ID)�洢EEPROM��ַ
** ���Ʋ���:
** 				���ڵ�س�ʼ��������ȡEEPROM��ַ
*/
uint32 get_UpgrBatEepromAddr(uint8 loc){
	return UpgrBinFilePara_BatAddr(loc);
}

