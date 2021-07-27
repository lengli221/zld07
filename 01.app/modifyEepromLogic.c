#include "includes.h"

/*
** 详细说明:根据位置信息填充文件名信息
*/
void fill_FileName(uint8 type,uint8 loc,uint8* fileName){
	uint8 subFileNmae[24] = 		{
									"CDG_MD_A_B1_200_000.bin"};
	uint8 batFileName[][24] = {
					/*欣旺达*/		{"SBT_SW_A_A1_A100_000.bin"},{"SBT_SW_A_B1_B200_000.bin"},{"SBT_SW_A_B1_B210_000.bin"},{"SBT_SW_A_B1_B220_000.bin"},
									{"SBT_SW_A_B1_B230_000.bin"},{"SBT_SW_A_B1_B300_000.bin"},
					/*飞毛腿*/ 	{"SBT_SC_A_A1_A100_000.bin"},{"SBT_SC_A_A1_A200_000.bin"},{"SBT_SC_A_B1_B300_000.bin"},{"SBT_SC_A_B1_B310_000.bin"},
									{"SBT_SC_A_B1_B320_000.bin"},{"SBT_SC_A_B1_B330_000.bin"},{"SBT_SC_A_B1_B400_000.bin"},
					/*南都*/		{"SBT_ND_A_B1_B100_000.bin"},{"SBT_ND_A_B1_B200_000.bin"},{"SBT_ND_A_B1_B300_000.bin"},
					/*新日动力*/	{"SBT_XR_A_B1_B100_000.bin"},{"SBT_XR_A_B1_B200_000.bin"},
					/*星恒*/		{"SBT_XH_A_B1_B100_000.bin"},{"SBT_XH_A_B1_B200_000.bin"},{"SBT_XH_A_B1_B300_000.bin"},
					/*ATL*/		{"SBT_AT_A_B1_B100_000.bin"},
					/*CATL*/		{"SBT_CA_A_B1_B100_000.bin"},
					/*欣旺达*/		{"SBT_SW_A_B1_B211_000.bin"},
					/*20210520--美团新增电池固件--20210524新增依据唐工提供的列表*/
					{"SBT_SC_A_B1_B500_000.bin"},
					{"SBT_SW_A_B1_B400_000.bin"},
					{"SBT_AT_A_B1_B200_000.bin"},
					{"SBT_AT_A_B1_B201_000.bin"},
					/*新增:20210524--依据唐工提供列表*/
					{"SBT_CA_A_B1_B200_000.bin"}};
	uint8 fileNameTemp[24] = {0};
	

	switch(type){
		case ComBoradType:/*分控固件*/
			memcpy((uint8*)&fileNameTemp[0],(uint8*)&subFileNmae[0],24);
			break;
		case BatBoardType:/*电池固件*/
			memcpy((uint8*)&fileNameTemp[0],(uint8*)&batFileName[loc][0],24);
			break;
	}

	/*拷贝文件名*/
	memcpy((uint8*)&fileName[0],(uint8*)&fileNameTemp[0],24);
}

/*
** 详细说明:修改文件名软件版本号
*/
void modify_FileNameSoftVer(uint8* softVer,uint16 sv){
	uint8 fileNameSoftVer[3] = {0};
	uint8 i = 0;

	for(i=0;i<3;i++){
		fileNameSoftVer[2-i] = (sv%10) + '0';
		sv /= 10;
	}

	memcpy((uint8*)&softVer[0],(uint8*)&fileNameSoftVer[0],3);
}

/*
** 20210202-->转换结构体存储
*/
ModifyEepromLogic set_TypedefModify(UpgrFilePara* upgr){
	ModifyEepromLogic modifyEL = {0};
	int16 loc = 0;
	int16 indexSuffix = -1;

	/*板卡固件类型拷贝*/
	modifyEL.board = upgr->upgrFileType.board;
	

	/*存储拷贝关键数据*/
	switch(upgr->upgrFileType.board){
		case ComBoradType:/*分控固件*/
			modifyEL.label = 0;
			break;
		case BatBoardType:/*电池固件*/
			loc = get_BatLocation(*upgr);
			if(loc != -1){
				modifyEL.label = loc;
			}
			break;
	}
	modifyEL.softver = upgr->upgrFileType.softVer;
	modifyEL.flashAddr = upgr->upgrFileType.flashAddr;
	modifyEL.binFileItemPara = upgr->upgrFileType.binFileItemPara;

	/*20210221--首次初始化逻辑,填充文件名*/
	if(upgr->upgrFileType.fileNameLen == 0){
		switch(upgr->upgrFileType.board){
			case ComBoradType:/*分控固件*/
				upgr->upgrFileType.fileNameLen = 23;								
				break;
			case BatBoardType:/*电池固件*/
				upgr->upgrFileType.fileNameLen = 24;		
				break;
		}
		/*拷贝文件名*/
		fill_FileName(modifyEL.board, modifyEL.label, (uint8 *)&upgr->upgrFileType.fileName[0]);
		/*修改文件名软件版本号*/
		indexSuffix = fileNameVerSuffix((uint8 *)&upgr->upgrFileType.fileName[0], upgr->upgrFileType.fileNameLen);
		if(indexSuffix != -1){
			modify_FileNameSoftVer((uint8 *)&upgr->upgrFileType.fileName[indexSuffix], upgr->upgrFileType.softVer);
		}
	}
	return modifyEL;
}

/*
** 20210202-->转换结构体存储 -- 指定初始化参数
*/
ModifyEepromLogic set_TypedefModify_Assign(UpgrFilePara* upgr,uint8 lo){
	ModifyEepromLogic modifyEL = {0};
	int16 loc = 0;
	int16 indexSuffix = -1;

	/*板卡固件类型拷贝*/
	modifyEL.board = upgr->upgrFileType.board;
	

	/*存储拷贝关键数据*/
	switch(upgr->upgrFileType.board){
		case ComBoradType:/*分控固件*/
			modifyEL.label = 0;
			break;
		case BatBoardType:/*电池固件*/
			loc = lo;
			if(loc != -1){
				modifyEL.label = loc;
			}
			break;
	}
	modifyEL.softver = upgr->upgrFileType.softVer;
	modifyEL.flashAddr = upgr->upgrFileType.flashAddr;
	modifyEL.binFileItemPara = upgr->upgrFileType.binFileItemPara;

	/*20210221--首次初始化逻辑,填充文件名*/
	if(upgr->upgrFileType.fileNameLen == 0){
		switch(upgr->upgrFileType.board){
			case ComBoradType:/*分控固件*/
				upgr->upgrFileType.fileNameLen = 23;								
				break;
			case BatBoardType:/*电池固件*/
				upgr->upgrFileType.fileNameLen = 24;		
				break;
		}
		/*拷贝文件名*/
		fill_FileName(modifyEL.board, modifyEL.label, (uint8 *)&upgr->upgrFileType.fileName[0]);
		/*修改文件名软件版本号*/
		indexSuffix = fileNameVerSuffix((uint8 *)&upgr->upgrFileType.fileName[0], upgr->upgrFileType.fileNameLen);
		if(indexSuffix != -1){
			modify_FileNameSoftVer((uint8 *)&upgr->upgrFileType.fileName[indexSuffix], upgr->upgrFileType.softVer);
		}
	}
	return modifyEL;
}

/*
** 20210220 --> 结构体转换
*/
UpgrFilePara conv_TypedefModify(ModifyEepromLogic modifyEL){
	UpgrFilePara upgrTemp = {0};
	DetailedInfo detailedInfoTemp = {0};
	int16 indexSuffix = -1;
	
	/*板卡类型copy*/
	upgrTemp.upgrFileType.board = modifyEL.board;

	/*将关键参数转换成全部参数*/
	switch(modifyEL.board){
		case ComBoradType:/*分控固件*/
			upgrTemp.upgrFileType.detailedInfo.hardVer = 200;/*6A硬件版本号为:200*/
			upgrTemp.upgrFileType.fileNameLen = 23;
			break;
		case BatBoardType:/*电池固件*/
			detailedInfoTemp = default_DetailedInfo(modifyEL.label);
			upgrTemp.upgrFileType.detailedInfo = detailedInfoTemp;
			upgrTemp.upgrFileType.fileNameLen = 24;
			break;
	}
	upgrTemp.upgrFileType.softVer = modifyEL.softver;
	upgrTemp.upgrFileType.flashAddr = modifyEL.flashAddr;
	upgrTemp.upgrFileType.binFileItemPara = modifyEL.binFileItemPara;
	/*拷贝文件名*/
	fill_FileName(modifyEL.board, modifyEL.label, (uint8 *)&upgrTemp.upgrFileType.fileName[0]);
	/*修改文件名软件版本号*/
	indexSuffix = fileNameVerSuffix((uint8 *)&upgrTemp.upgrFileType.fileName[0], upgrTemp.upgrFileType.fileNameLen);
	if(indexSuffix != -1){
		modify_FileNameSoftVer((uint8 *)&upgrTemp.upgrFileType.fileName[indexSuffix], upgrTemp.upgrFileType.softVer);
	}
	return upgrTemp;
}

