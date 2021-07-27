#include "includes.h"

/*
** 系统参数之本地保存参数设置之结构体声明
*/
SysParam sysParam;

/*
** 使用指针提供外部文件访问变量
*/
SysParam* get_SysParaPtr(void){
	return &sysParam;
} 

/*
** 控制板文件名长度
*/
uint8 sysCtrFileNameLen;
uint8 sysCtrFileName[FileName_MaxLen];

/*
** 通讯板/电池包固件参数
*/
UpgrFilePara upgrComBinFilePara = {0};
UpgrFilePara upgrBatBinFilePara[44] = {0};

/*
** clear 通讯板/电池包软件版本号
*/
void clear_AppVer(void){
	memset((uint8*)&sysParam.newSoftVer.comApp[0],0x00,sizeof(uint16)*SysCtr_AllDoorNum);
}

/*
** get 通讯板/电池包运行App域版本号
*/
uint16 getComRunAppVer(uint8 addr){
	return sysParam.newSoftVer.comApp[addr];
}

/*
** set 通讯板/电池包运行App域版本号
*/
void setComRunAppVer(uint16 ver,uint8 addr){
	sysParam.newSoftVer.comApp[addr] = ver;
}

/*
** get Ctr SoftVer
*/
uint16 get_CtrSoftVer(void){
	return sysParam.newSoftVer.ctr;
}

/*
** 更新控制板软件/通讯板软件版本号
*/
void updateSoftVerPara(void){
	UpgrFilePara upgrTemp = {0};
	/*
	** 控制板软件版本号
	*/
	sysParam.newSoftVer.ctr = (uint16)get_CtrVer();
	/*
	** 通讯板 -->外部Flash中保存的通讯板文件软件版本号
	*/
	upgrTemp = get_UpgrComFilePara();
	sysParam.newSoftVer.comFile = upgrTemp.upgrFileType.softVer;
}

/*----------------------检测:实际运行版本号同文件名版本号是否一致-----------------------------*/
void reset_ChkCtrSoftVer(void){
	/*----------------------每次发布新版板必须修改-------------------------------------*/
	uint16 ctr_SoftVer = 367;
	uint8 realityFileName[23] ={ "CDG_MD_B_B1_200_367.bin"};/*测试版本:111--用于做升级测试处理*/
	/*---------------------------------------------------------------------------------*/
	uint32 ver = (uint32)ctr_SoftVer;
	uint16 wFileNameLen = 23;
	uint8 tmpp[sizeof(uint32)+0x03] = {0};
	uint8 fileNameLen[sizeof(uint16)+0x03] = {0};
	uint8 fileName[FileName_MaxLen+0x03] = {0};/*fileName+head+crc16*/
	uint16 crc16 = 0;

	
	if(sysParam.newSoftVer.ctr != ctr_SoftVer || memcmp((uint8*)&realityFileName[0],(uint8*)&sysCtrFileName[0],23) != 0){
		sysParam.newSoftVer.ctr = ctr_SoftVer;
		memcpy((uint8*)&sysCtrFileName[0],(uint8*)&realityFileName[0],23);	

		/*1.写软件版本号*/
		tmpp[0] = UpgradeKey_Head;
		memcpy((uint8*)&tmpp[1],(uint8*)&ver,sizeof(uint32));
		crc16 = CRC16((uint8 *)&tmpp[0], sizeof(uint32)+sizeof(uint8));
		memcpy((uint8*)&tmpp[sizeof(uint32)+sizeof(uint8)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeVer_Addr, (uint8*)&tmpp[0], UpgradeVer_Len_Addr);
		eeprom_write(UpgradeVer_Addr, (uint8 *)&tmpp[0], UpgradeVer_Len_Addr);		

		/*2.写文件名长度,文件名*/
		fileNameLen[0] = UpgradeKey_Head;
		memcpy((uint8*)&fileNameLen[1],(uint8*)&wFileNameLen,sizeof(uint16));
		crc16 = CRC16((uint8 *)&fileNameLen[0], sizeof(uint8)+sizeof(uint16));
		memcpy((uint8*)&fileNameLen[sizeof(uint8)+sizeof(uint16)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeFileNameLen_Addr, (uint8 *)&fileNameLen[0], UpgradeFileNameLen_Len_Addr);
		eeprom_write(UpgradeFileNameLen_Addr, (uint8 *)&fileNameLen[0], UpgradeFileNameLen_Len_Addr);	

		fileName[0] = UpgradeKey_Head;
		memcpy((uint8*)&fileName[1],(uint8*)&sysCtrFileName[0],wFileNameLen);
		crc16 = CRC16((uint8 *)&fileName[0], wFileNameLen+sizeof(uint8));
		memcpy((uint8*)&fileName[wFileNameLen+sizeof(uint8)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeFileName_Addr, (uint8 *)&fileName[0], wFileNameLen+sizeof(uint8)+sizeof(uint16));
		eeprom_write(UpgradeFileName_Addr, (uint8 *)&fileName[0], wFileNameLen+sizeof(uint8)+sizeof(uint16));		
	}
}
/*--------------------------------------------------------------------------------------------*/

/*
** set 升级秘钥参数
*/
void set_UpgrCtrKey(void){
	uint8 buf_bkp[UpgradeKey_Len_Addr] = {UpgradeKey_Head,
		0x7A,0x6C,0x64,0x12,0x04,0x01,0x00,0x00};
	uint16 crc16 = 0;

	crc16 = CRC16((uint8 *)&buf_bkp[0], UpgradeKey_Len_Addr - sizeof(uint16));
	memcpy((uint8*)&buf_bkp[sizeof(uint8)+sizeof(UpgradeKey)],(uint8*)&crc16,sizeof(uint16));

	BKP_WriteBackupDat(UpgradeKey_Addr, (uint8*)&buf_bkp[0], UpgradeKey_Len_Addr);
}

/*
** get Ctr软件版本号
*/
uint32 get_CtrVer(void){
	uint32 ver = 0;
	uint8 buf[UpgradeVer_Len_Addr] = {0};
	uint8 buf_bkp[UpgradeVer_Len_Addr] = {0};
	uint8 len = 0;
	uint8 head = 0;
	uint8 head_bkp = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	
	eeprom_read(UpgradeVer_Addr, (uint8 *)&buf[len], UpgradeVer_Len_Addr);
	BKP_ReadBackupRegDat(UpgradeVer_Addr, (uint8*)&buf_bkp[len], UpgradeVer_Len_Addr);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(uint8) + sizeof(uint32));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(uint8) + sizeof(uint32));

	/*
	** 帧头
	*/
	head = buf[len];
	head_bkp = buf_bkp[len];
	len += sizeof(uint8);
	len += sizeof(uint32);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(head == UpgradeKey_Head && crc == getCrc){
		memcpy((uint8*)&ver,(uint8*)&buf[sizeof(uint8)],sizeof(uint32));
		BKP_WriteBackupDat(UpgradeVer_Addr, (uint8*)&buf[0], UpgradeVer_Len_Addr);
	}else if(head_bkp == UpgradeKey_Head && crc_bkp == getCrc_bkp){
		memcpy((uint8*)&ver,(uint8*)&buf_bkp[sizeof(uint8)],sizeof(uint32));
		eeprom_write(UpgradeVer_Addr, (uint8*)&buf_bkp[0], UpgradeVer_Len_Addr);
	}else{
		ver = 1;
	}

	return ver;		
}

/*
** get Ctr文件名长度
*/
static uint16 get_CtrFileNameLen(void){
	return sysCtrFileNameLen;
}

/*
** 初始化更新 Ctr文件名长度
*/
void init_UpgrCtrFileNameLen(void){
	uint8 buf[UpgradeFileNameLen_Len_Addr] = {0};
	uint8 buf_bkp[UpgradeFileNameLen_Len_Addr] = {0};
	uint8 len = 0;
	uint8 head = 0;
	uint8 head_bkp = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	
	eeprom_read(UpgradeFileNameLen_Addr, (uint8 *)&buf[len], UpgradeFileNameLen_Len_Addr);
	BKP_ReadBackupRegDat(UpgradeFileNameLen_Addr, (uint8*)&buf_bkp[len], UpgradeFileNameLen_Len_Addr);

	/*
	** 计算CRC校验
	*/
	crc = CRC16((uint8*)&buf[len],sizeof(uint8)+sizeof(uint16));
	crc_bkp = CRC16((uint8*)&buf_bkp[len],sizeof(uint8)+sizeof(uint16));

	/*
	** 帧头
	*/
	head = buf[len];
	head_bkp = buf_bkp[len];
	len += sizeof(uint8);
	len += sizeof(uint16);

	/*
	** get CRC校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(head == UpgradeKey_Head && crc == getCrc){
		memcpy((uint8*)&sysCtrFileNameLen,(uint8*)&buf[sizeof(uint8)],sizeof(uint16));
		BKP_WriteBackupDat(UpgradeFileNameLen_Addr, (uint8*)&buf[0], UpgradeFileNameLen_Len_Addr);
	}else if(head_bkp == UpgradeKey_Head && crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysCtrFileNameLen,(uint8*)&buf_bkp[sizeof(uint8)],sizeof(uint16));
		eeprom_write(UpgradeFileNameLen_Addr, (uint8*)&buf_bkp[0], UpgradeFileNameLen_Len_Addr);
	}else{
		sysCtrFileNameLen = 0;
	}
}


/*
** get Ctr文件名
*/
void get_CtrFileName(uint8* rx,uint8* rxlen){
	uint16 fileNameLen = 0;

	fileNameLen = get_CtrFileNameLen();

	if(fileNameLen > 0 && fileNameLen <= FileName_MaxLen){/*文件名长度合法*/
		memcpy((uint8*)&rx[0],(uint8*)&sysCtrFileName[0],fileNameLen);
	}

	*rxlen = (uint8)fileNameLen;
}

/*
** 初始化控制板文件名
*/
void init_sysCtrFileName(void){
	uint8 buf[UpgradeFileName_Len_Addr] = {0};
	uint8 buf_bkp[UpgradeFileName_Len_Addr] = {0};
	uint8 len = 0;
	uint8 head = 0;
	uint8 head_bkp = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	uint16 fileNameLen = 0;
	uint16 frameLen = 0;
	fileNameLen = get_CtrFileNameLen();

	if(fileNameLen > 0 && fileNameLen <= FileName_MaxLen){/*文件名长度合法*/
		frameLen = fileNameLen + sizeof(uint8) + sizeof(uint16);
		eeprom_read(UpgradeFileName_Addr, (uint8 *)&buf[len], frameLen);
		BKP_ReadBackupRegDat(UpgradeFileName_Addr, (uint8*)&buf_bkp[len], frameLen);

		/*
		** 计算CRC校验
		*/
		crc = CRC16((uint8*)&buf[len],sizeof(uint8)+fileNameLen);
		crc_bkp = CRC16((uint8*)&buf_bkp[len],sizeof(uint8)+fileNameLen);

		/*
		** 帧头
		*/
		head = buf[len];
		head_bkp = buf_bkp[len];
		len += sizeof(uint8);
		len += fileNameLen;

		/*
		** get CRC校验
		*/
		memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
		memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

		if(head == UpgradeKey_Head && crc == getCrc){
			memcpy((uint8*)&sysCtrFileName[0],(uint8*)&buf[sizeof(uint8)],fileNameLen);
			BKP_WriteBackupDat(UpgradeFileName_Addr, (uint8*)&buf[0], frameLen);
		}else if(head_bkp == UpgradeKey_Head && crc_bkp == getCrc_bkp){
			memcpy((uint8*)&sysCtrFileName[0],(uint8*)&buf_bkp[sizeof(uint8)],fileNameLen);
			eeprom_write(UpgradeFileName_Addr, (uint8*)&buf_bkp[0], frameLen);
		}else{
			memset((uint8*)&sysCtrFileName[0],0x00,fileNameLen);
		}
	}
}

/*
** 美团电池包个数初始化
*/
#define Init_BatFileSzie			23

/*
** default 电池包信息
*/
DetailedInfo default_DetailedInfo(uint16 loc){
	DetailedInfo detailInfo[Init_BatFileSzie] = {
		/*欣旺达*/{100,"A20301"},{200,"A20608"},{210,"A20708"},{220,"A20508"},{230,"A20308"},{300,"A20614"},
		/*飞毛腿*/{100,"A30301"},{200,"A30301"},{300,"A30608"},{310,"A30708"},{320,"A30508"},{330,"A30208"},{400,"A31013"},
		/*南都*/{100,"A41002"},{200,"A41002"},{300,"A41002"},
		/*新日动力*/{100,"A50606"},{200,"A50607"},
		/*星恒*/{100,"A61103"},{200,"A61104"},{300,"A61109"},
		/*ATL*/{100,"A71112"},
		/*CATL*/{100,"A81015"}};

	DetailedInfo detailInfoTemp = {0};
	
	if(loc < Init_BatFileSzie){
		detailInfoTemp = detailInfo[loc];
	}
	return detailInfoTemp;
}

/*
** set升级bin文件参数
** 策略:根据电池包详细信息(硬件版本/ID)存储指定地址位置
*/
void set_UpgrBinFilePara(UpgrFilePara upgr){
	uint8 buf[UpgrBinFilePara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint32 addr = 0;

	memcpy((uint8*)&buf[len],(uint8*)&upgr.upgrFileType.cmdType,sizeof(UpgrFilePara));
	len += sizeof(UpgrFilePara);
	/*
	** 计算CRC16
	*/
	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	addr = get_UpgrEepromAddr(upgr);
	eeprom_write(addr, (uint8 *)&buf[0], len);
	BKP_WriteBackupDat(addr, (uint8*)&buf[0], len);
}

/*
** set 升级bin文件参数(电池初始化)
*/
void set_InitUpgrBatBinFile(UpgrFilePara upgr,uint8 loc){
	uint8 buf[UpgrBinFilePara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint32 addr = 0;

	/*
	** 更新通讯板/电池包参数
	*/
// 	if(upgr.upgrFileType.board == ComBoradType){/*通讯板*/
// 		upgrComBinFilePara = upgr;		
// 	}else{
// 		upgrBatBinFilePara[loc] = upgr;
// 	}
	
	if(upgr.upgrFileType.board == BatBoardType){/*电池固件包优化处理--该函数仅针对电池固件包处理*/
		upgrBatBinFilePara[loc] = upgr;
		memcpy((uint8*)&buf[len],(uint8*)&upgr.upgrFileType.cmdType,sizeof(UpgrFilePara));
		len += sizeof(UpgrFilePara);
		/*
		** 计算CRC16
		*/
		crc = CRC16((uint8 *)&buf[0], len);
		memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
		len += sizeof(uint16);

		addr = get_UpgrBatEepromAddr(loc);
		eeprom_write(addr, (uint8 *)&buf[0], len);
		BKP_WriteBackupDat(addr, (uint8*)&buf[0], len);
	}
}

/*
** set Assign电池包bin文件参数
*/
void set_AssignUpgrBinFilePara(UpgrFilePara upgr,int16 loc){
	uint8 buf[UpgrBinFilePara_Len] = {0};
	uint16 len = 0;
	uint16 crc = 0;
	
	memcpy((uint8*)&buf[len],(uint8*)&upgr.upgrFileType.cmdType,sizeof(UpgrFilePara));
	len += sizeof(UpgrFilePara);
	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);
	eeprom_write(UpgrBinFilePara_BatAddr(loc), (uint8 *)&buf[0], len);
	BKP_WriteBackupDat(UpgrBinFilePara_BatAddr(loc), (uint8*)&buf[0], len);	

	/*
	** 更新电池包参数
	*/
	upgrBatBinFilePara[loc] = upgr;
}

/*
** get升级bin文件参数--电池包
** @para:
**		loc:指定位置--(0,max(get_SizeBatFile()))
*/
UpgrFilePara get_UpgrBatFilePara(uint16 loc){

	return upgrBatBinFilePara[loc];
}

/*
** 初始化更新电池包固件参数
*/
void init_UpdateBatFileParam(void){
	uint8 buf[UpgrBinFilePara_Len] = {0};
	uint8 buf_bkp[UpgrBinFilePara_Len] = {0};
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	uint8 len = 0;
	DetailedInfo detailedInfoTemp = {0};
	uint8 loc = 0;

	for(loc=0;loc<sysParam.batFireSize;loc++){
		len = 0;
		eeprom_read(UpgrBinFilePara_BatAddr(loc), (uint8 *)&buf[len], UpgrBinFilePara_Len);
		BKP_ReadBackupRegDat(UpgrBinFilePara_BatAddr(loc), (uint8*)&buf_bkp[len], UpgrBinFilePara_Len);

		/*
		** 计算 CRC校验
		*/
		crc = CRC16((uint8 *)&buf[len], sizeof(UpgrFilePara));
		crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(UpgrFilePara));

		len += sizeof(UpgrFilePara);

		/*
		** get CRC16校验
		*/
		memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
		memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

		#if ClearAllFireware == 1
			getCrc = 0;
			getCrc_bkp = 0;
		#endif

		if(crc == getCrc){
			memcpy((uint8*)&upgrBatBinFilePara[loc].upgrFileType.cmdType,(uint8*)&buf[0],sizeof(UpgrFilePara));
			BKP_WriteBackupDat(UpgrBinFilePara_BatAddr(loc),(uint8*)&buf[0], UpgrBinFilePara_Len);
		}else if(crc_bkp == getCrc_bkp){
			memcpy((uint8*)&upgrBatBinFilePara[loc].upgrFileType.cmdType,(uint8*)&buf_bkp[0],sizeof(UpgrFilePara));
			eeprom_write(UpgrBinFilePara_BatAddr(loc), (uint8 *)&buf_bkp[0], UpgrBinFilePara_Len);
		}else{
			detailedInfoTemp = default_DetailedInfo(loc);
			upgrBatBinFilePara[loc].upgrFileType.board = BatBoardType;
			memcpy((uint8*)&upgrBatBinFilePara[loc].upgrFileType.detailedInfo.hardVer,(uint8*)&detailedInfoTemp.hardVer,sizeof(DetailedInfo));
			if(detailedInfoTemp.hardVer != 0){/*defalut电池包系列*/
				set_AssignUpgrBinFilePara(upgrBatBinFilePara[loc], loc);
			}
		}
		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();		
	}
}

/*
** get升级bin文件参数--通讯板
*/
UpgrFilePara get_UpgrComFilePara(void){
	return upgrComBinFilePara;
}

/*
** 初始上电更新通讯板参数--init_UpdateUpgrCom
*/
void init_UpdateUpgrComFilePara(void){
	UpgrFilePara upgrTemp = {0};
	uint8 buf[UpgrBinFilePara_Len] = {0};
	uint8 buf_bkp[UpgrBinFilePara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
		
	eeprom_read(UpgrBinFilePara_CommAddr, (uint8 *)&buf[len], UpgrBinFilePara_Len);
	BKP_ReadBackupRegDat(UpgrBinFilePara_CommAddr, (uint8*)&buf_bkp[len], UpgrBinFilePara_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(UpgrFilePara));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(UpgrFilePara));

	len += sizeof(UpgrFilePara);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	#if ClearAllFireware == 1
		getCrc = 0;
		getCrc_bkp = 0;
	#endif

	if(crc == getCrc){
		memcpy((uint8*)&upgrComBinFilePara.upgrFileType.cmdType, (uint8*)&buf[0], sizeof(UpgrFilePara));
		BKP_WriteBackupDat(UpgrBinFilePara_CommAddr, (uint8*)&buf[0], UpgrBinFilePara_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&upgrComBinFilePara.upgrFileType.cmdType, (uint8*)&buf_bkp[0], sizeof(UpgrFilePara));
		eeprom_write(UpgrBinFilePara_CommAddr, (uint8*)&buf_bkp[0], UpgrBinFilePara_Len);
	}else{
		upgrTemp.upgrFileType.board = ComBoradType;
		set_UpgrBinFilePara(upgrTemp);
	}
}


/*
** get电池包存储队列的大小(个数)
*/
uint16 get_SizeBatFile(void){
	uint16 num = 0;
	uint8 buf[UpgrBatFileNumLen] = {0};
	uint8 buf_bkp[UpgrBatFileNumLen] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	
	eeprom_read(UpgrBatFileNumAddr, (uint8 *)&buf[len], UpgrBatFileNumLen);
	BKP_ReadBackupRegDat(UpgrBatFileNumAddr, (uint8*)&buf_bkp[len], UpgrBatFileNumLen);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(uint16));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(uint16));

	len += sizeof(uint16);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));
	if(crc == getCrc){
		memcpy((uint8*)&num,(uint8*)&buf[0],sizeof(uint16));
		BKP_WriteBackupDat(UpgrBatFileNumAddr, (uint8*)&buf[0], UpgrBatFileNumLen);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&num,(uint8*)&buf_bkp[0],sizeof(uint16));
		eeprom_write(UpgrBatFileNumAddr, (uint8*)&buf_bkp[0], UpgrBatFileNumLen);
	}else{
		num = Init_BatFileSzie;
		init_BatPara();
		set_SizeBatFile(Init_BatFileSzie);
	}
	/*
	** get电池包存储队列的大小(个数)--初始参数
	*/
	sysParam.batFireSize = num;
	
	return num;
}

/*
** get 电池固件包个数
** 		策略:仅用于查询电池包个数
*/
uint16 get_batFireSize(void){
	return sysParam.batFireSize;	
}

/*
** set电池包存储队列的大小(个数)
*/
void set_SizeBatFile(uint16 szie){
	uint8 buf[UpgrBatFileNumLen] = {0};
	uint16 crc = 0;
	uint8 len = 0;

	sysParam.batFireSize = szie;
	
	memcpy((uint8*)&buf[len],(uint8*)&szie,sizeof(uint16));
	len += sizeof(uint16);
	crc = CRC16((uint8 *)&buf[0], sizeof(uint16));
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	eeprom_write(UpgrBatFileNumAddr, (uint8 *)&buf[0], UpgrBatFileNumLen);
	BKP_WriteBackupDat(UpgrBatFileNumAddr, (uint8*)&buf[0], UpgrBatFileNumLen);
}

/*
** 初始化电池包(硬件版本号,ID)参数
*/
void init_BatPara(void){

	UpgrFilePara upgr = {0};
	uint16 i  = 0;
	DetailedInfo detailedInfoTemp = {0};

	upgr.upgrFileType.board = BatBoardType;

	for(i=0;i<Init_BatFileSzie;i++){
		detailedInfoTemp = default_DetailedInfo(i);
		memcpy((uint8*)&upgr.upgrFileType.detailedInfo.hardVer,(uint8*)&detailedInfoTemp.hardVer,sizeof(DetailedInfo));
		set_InitUpgrBatBinFile(upgr, i);
		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
	}
}

/*
** get电池包位置信息
** @return:
** 			-1:无电池包存储区域信息--是否添加
**			-2:参数丢失
** 策略:
**		1.用于升级判断是否存在电池包对应型号区域
**		2.以位置信息获取Eeprom地址信息
*/
int16 get_BatLocation(UpgrFilePara upgr){
	int16 loc = -2;
	uint16 i = 0;
	uint16 size = get_batFireSize()/*get_SizeBatFile()*/;
	UpgrFilePara upgrTemp = {0};
	if(size >= Init_BatFileSzie){/*美团电池升级协议默认电池包数量*/
		loc = -1;/*未查找之前,假设无电池包信息*/
		for(i = 0;i < size;i++){
			upgrTemp = get_UpgrBatFilePara(i);
			if(memcmp((uint8*)&upgrTemp.upgrFileType.detailedInfo.hardVer,
				(uint8*)&upgr.upgrFileType.detailedInfo.hardVer,sizeof(DetailedInfo)) == 0){
				loc = i;
				break;
			}
		}
	}else{/*存储电池包大小参数丢失*/
		init_BatPara();
		set_SizeBatFile(Init_BatFileSzie);
		loc = -2;
	}

	if(loc == -2){/*参数丢失:递归,回调*/
		loc = get_BatLocation(upgr);
	}

	return loc;
} 

/*
** set 系统温度值(基准+回差)
*/
void set_Tsave(Tsave ts){
	uint8 buf[TSave_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.tsave = ts;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.tsave,sizeof(Tsave));
	len += sizeof(Tsave);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*eeprom_write(TSave_Addr, (uint8 *)&buf[0], TSave_Len);*/
	BKP_WriteBackupDat(TSave_Addr, (uint8_t *)&buf[0], TSave_Len);
}

/*
** update 系统温度值
*/
void update_Tsave(void){
	Tsave ts = {400,50};
	uint8 buf[TSave_Len] = {0};
	uint8 buf_bkp[TSave_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	/*eeprom_read(TSave_Addr, (uint8 *)&buf[0], TSave_Len);*/
	BKP_ReadBackupRegDat(TSave_Addr, (uint8*)&buf_bkp[0], TSave_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[0], sizeof(Tsave));
	crc_bkp = CRC16((uint8 *)&buf_bkp[0], sizeof(Tsave));

	len += sizeof(Tsave);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.tsave.base,(uint8*)&buf[0],sizeof(Tsave));
		BKP_WriteBackupDat(TSave_Addr, (uint8*)&buf[0], TSave_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.tsave.base,(uint8*)&buf_bkp[0],sizeof(Tsave));
		/*eeprom_write(TSave_Addr, (uint8*)&buf_bkp[0], TSave_Len);*/
	}else{
		set_Tsave(ts);
	}	
}

/*
** get 系统温度值(基准+回差)
*/
Tsave get_Tsave(void){
	return sysParam.tsave;
}

/*
** set SOC阈值
*/
void set_SocLimit(uint8 soc){
	uint8 buf[SocLimit_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.socLimit = soc;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.socLimit,sizeof(uint8));
	len += sizeof(uint8);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*eeprom_write(SocLimit_Addr, (uint8 *)&buf[0], len);*/
	BKP_WriteBackupDat(SocLimit_Addr, (uint8_t *)&buf[0], len);
}

/*
** update SOC阈值
*/
void update_SocLimit(void){
	uint8 socLimit = 95;
	uint8 buf[SocLimit_Len] = {0};
	uint8 buf_bkp[SocLimit_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	/*eeprom_read(SocLimit_Addr, (uint8 *)&buf[0], SocLimit_Len);*/
	BKP_ReadBackupRegDat(SocLimit_Addr, (uint8*)&buf_bkp[0], SocLimit_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[0], sizeof(uint8));
	crc_bkp = CRC16((uint8 *)&buf_bkp[0], sizeof(uint8));

	len += sizeof(uint8);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.socLimit,(uint8*)&buf[0],sizeof(uint8));
		BKP_WriteBackupDat(SocLimit_Addr, (uint8*)&buf[0], SocLimit_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.socLimit,(uint8*)&buf_bkp[0],sizeof(uint8));
		/*eeprom_write(SocLimit_Addr, (uint8*)&buf_bkp[0], SocLimit_Len);*/
	}else{
		set_SocLimit(socLimit);
	}	
}

/*
** get SOC阈值
*/
uint8 get_SocLimit(void){
	return sysParam.socLimit;
}

/*
** set 充电时间阈值
*/
void set_ChgTimeLimit(uint16 time){
	uint8 buf[ChgTimeLimit_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.chgTimeOut = time;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.chgTimeOut,sizeof(uint16));
	len += sizeof(uint16);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*eeprom_write(ChgTimeLimit_Addr, (uint8 *)&buf[0], len);*/
	BKP_WriteBackupDat(ChgTimeLimit_Addr, (uint8_t *)&buf[0], len);
}

/*
** update 充电时间阈值
*/
void update_ChgTimeLimit(void){
	uint16 chgTimeLimit = 600;/*10小时--600min*/
	uint8 buf[ChgTimeLimit_Len] = {0};
	uint8 buf_bkp[ChgTimeLimit_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	/*eeprom_read(ChgTimeLimit_Addr, (uint8 *)&buf[0], ChgTimeLimit_Len);*/
	BKP_ReadBackupRegDat(ChgTimeLimit_Addr, (uint8*)&buf_bkp[0], ChgTimeLimit_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[0], sizeof(uint16));
	crc_bkp = CRC16((uint8 *)&buf_bkp[0], sizeof(uint16));

	len += sizeof(uint16);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.chgTimeOut,(uint8*)&buf[0],sizeof(uint16));
		BKP_WriteBackupDat(ChgTimeLimit_Addr, (uint8*)&buf[0], ChgTimeLimit_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.chgTimeOut,(uint8*)&buf_bkp[0],sizeof(uint16));
		/*eeprom_write(ChgTimeLimit_Addr, (uint8*)&buf_bkp[0], ChgTimeLimit_Len);*/
	}else{
		set_ChgTimeLimit(chgTimeLimit);
	}	
}

/*
** get 充电时间阈值
*/
uint16 get_ChgTimeLimit(void){
	return sysParam.chgTimeOut;
}

/*
** set 充电器过温阈值
*/
void set_ChgOverTempLimit(uint16 temp){
	uint8 buf[ChgOverTempLimit_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.chgOverTemp = temp;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.chgOverTemp,sizeof(uint16));
	len += sizeof(uint16);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*eeprom_write(ChgOverTempLimit_Addr, (uint8 *)&buf[0], len);*/
	BKP_WriteBackupDat(ChgOverTempLimit_Addr, (uint8_t *)&buf[0], len);
}

/*
** update 充电器过温阈值
*/
void update_ChgOverTempLimit(void){
	uint16 temp = 800;/*80摄氏度*/
	uint8 buf[ChgOverTempLimit_Len] = {0};
	uint8 buf_bkp[ChgOverTempLimit_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	/*eeprom_read(ChgOverTempLimit_Addr, (uint8 *)&buf[len], ChgOverTempLimit_Len);*/
	BKP_ReadBackupRegDat(ChgOverTempLimit_Addr, (uint8*)&buf_bkp[len], ChgOverTempLimit_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(uint16));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(uint16));

	len += sizeof(uint16);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.chgOverTemp,(uint8*)&buf[0],sizeof(uint16));
		BKP_WriteBackupDat(ChgOverTempLimit_Addr, (uint8*)&buf[0], ChgOverTempLimit_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.chgOverTemp,(uint8*)&buf_bkp[0],sizeof(uint16));
		/*eeprom_write(ChgOverTempLimit_Addr, (uint8*)&buf_bkp[0], ChgOverTempLimit_Len);*/
	}else{
		set_ChgOverTempLimit(temp);
	}
}

/*
** get 充电器过温阈值
*/
uint16 get_ChgOverTempLimit(void){
	return sysParam.chgOverTemp;
}

/*
** set 充电器仓过温阈值
*/
void set_ChgDoorOTLimit(uint16 temp){
	uint8 buf[ChgDoorOTLimit_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.chgDoorOTemp = temp;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.chgDoorOTemp,sizeof(uint16));
	len += sizeof(uint16);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*eeprom_write(ChgDoorOTLimit_Addr, (uint8 *)&buf[0], len);*/
	BKP_WriteBackupDat(ChgDoorOTLimit_Addr, (uint8_t *)&buf[0], len);
}

/*
** update 充电器仓过温阈值
*/
void update_ChgDoorOTLimit(void){
	uint16 temp = 700;/*70摄氏度*/
	uint8 buf[ChgDoorOTLimit_Len] = {0};
	uint8 buf_bkp[ChgDoorOTLimit_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	/*eeprom_read(ChgDoorOTLimit_Addr, (uint8 *)&buf[len], ChgDoorOTLimit_Len);*/
	BKP_ReadBackupRegDat(ChgDoorOTLimit_Addr, (uint8*)&buf_bkp[len], ChgDoorOTLimit_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(uint16));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(uint16));

	len += sizeof(uint16);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.chgDoorOTemp,(uint8*)&buf[0],sizeof(uint16));
		BKP_WriteBackupDat(ChgDoorOTLimit_Addr, (uint8*)&buf[0], ChgDoorOTLimit_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.chgDoorOTemp,(uint8*)&buf_bkp[0],sizeof(uint16));
		/*eeprom_write(ChgDoorOTLimit_Addr, (uint8*)&buf_bkp[0], ChgDoorOTLimit_Len);*/
	}else{
		set_ChgDoorOTLimit(temp);
	}
}

/*
** get 充电器仓过温阈值
*/
uint16 get_ChgDoorOTLimit(void){
	return sysParam.chgDoorOTemp;
}

/*
** set 域名
*/
void set_SysDomain(SysDomain s){
	uint8 buf[LocalRemoteDomain_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.sysDomain = s;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.sysDomain.local[0],sizeof(SysDomain));
	len += sizeof(SysDomain);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	eeprom_write(LocalRemoteDomain_Addr, (uint8 *)&buf[0], len);
	BKP_WriteBackupDat(LocalRemoteDomain_Addr, (uint8_t *)&buf[0], len);	
}

/*
** update 域名
*/
void update_SysDomain(void){
	SysDomain sysDomainTemp = {{"https://mt.smart2charge.com:30003"},{"https://ebike.meituan.com"}};
	uint8 buf[LocalRemoteDomain_Len] = {0};
	uint8 buf_bkp[LocalRemoteDomain_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	eeprom_read(LocalRemoteDomain_Addr, (uint8 *)&buf[len], LocalRemoteDomain_Len);
	BKP_ReadBackupRegDat(LocalRemoteDomain_Addr, (uint8*)&buf_bkp[len], LocalRemoteDomain_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], 128);
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], 128);

	len += sizeof(SysDomain);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.sysDomain.local[0],(uint8*)&buf[0],sizeof(SysDomain));
		BKP_WriteBackupDat(LocalRemoteDomain_Addr, (uint8*)&buf[0], LocalRemoteDomain_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.sysDomain.local[0],(uint8*)&buf_bkp[0],sizeof(SysDomain));
		eeprom_write(LocalRemoteDomain_Addr, (uint8*)&buf_bkp[0], LocalRemoteDomain_Len);
	}else{
		set_SysDomain(sysDomainTemp);
	}
}

/*
** get 域名
*/
SysDomain get_SysDomain(void){
	return sysParam.sysDomain;
}

/*
** set Sys 设备ID
*/
void set_SysDeviceId(DeviceId id){
	uint8 buf[SysDeviceId_Len] = {0};
	uint16 crc = 0;
	uint8 len = 0;
	
	/*
	** 更新系统参数
	*/	
	sysParam.deviceId = id;

	memcpy((uint8*)&buf[len],(uint8*)&sysParam.deviceId.show[0],sizeof(DeviceId));
	len += sizeof(DeviceId);

	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	eeprom_write(SysDeviceID_Addr, (uint8 *)&buf[0], len);
	BKP_WriteBackupDat(SysDeviceID_Addr, (uint8_t *)&buf[0], len);		
}

/*
** update Sys 设备ID
*/
void update_SysDeviceId(void){
	uint8 temp[16] = {'G',0x31,0x31,0x35,0x32,0x31,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x31};
	DeviceId deviceIdTemp = {0};
	uint8 buf[SysDeviceId_Len] = {0};
	uint8 buf_bkp[SysDeviceId_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	eeprom_read(SysDeviceID_Addr, (uint8 *)&buf[len], SysDeviceId_Len);
	BKP_ReadBackupRegDat(SysDeviceID_Addr, (uint8*)&buf_bkp[len], SysDeviceId_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(DeviceId));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(DeviceId));

	len += sizeof(DeviceId);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.deviceId.show[0],(uint8*)&buf[0],sizeof(DeviceId));
		BKP_WriteBackupDat(SysDeviceID_Addr, (uint8*)&buf[0], SysDeviceId_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.deviceId.show[0],(uint8*)&buf_bkp[0],sizeof(DeviceId));
		eeprom_write(SysDeviceID_Addr, (uint8*)&buf_bkp[0], SysDeviceId_Len);
	}else{
		memcpy((uint8*)&deviceIdTemp.show[0],(uint8*)&temp[0],16);
		set_SysDeviceId(deviceIdTemp);
	}
}

/*
** get Sys 设备ID
*/
DeviceId get_SysDeviceId(void){
	return sysParam.deviceId;
}

/*
** get 服务端参数
*/
Serve get_ServePara(void){
	return sysParam.serve;
}

/*
** set 服务端参数
*/
void setServePara(Serve s){
 	uint8 buf[Register_LogoutInfo_Len] = {0};
 	uint16 crc = 0;
 	uint8 len = 0;
 	
 	/*
 	** 更新系统参数
 	*/	
 	sysParam.serve = s;

 	memcpy((uint8*)&buf[len],(uint8*)&sysParam.serve.registerFlag,sizeof(Serve));
 	len += sizeof(Serve);

 	crc = CRC16((uint8 *)&buf[0], len);
 	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
 	len += sizeof(uint16);

 	eeprom_write(Register_LogoutInfo_Addr, (uint8 *)&buf[0], len);
 	BKP_WriteBackupDat(Register_LogoutInfo_Addr, (uint8_t *)&buf[0], len);	
}

/*
** update 服务端参数
*/
void update_ServerPara(void){
	Serve ss = {0};
	uint8 buf[Register_LogoutInfo_Len] = {0};
	uint8 buf_bkp[Register_LogoutInfo_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	

	eeprom_read(Register_LogoutInfo_Addr, (uint8 *)&buf[len], Register_LogoutInfo_Len);
	BKP_ReadBackupRegDat(Register_LogoutInfo_Addr, (uint8*)&buf_bkp[len], Register_LogoutInfo_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], sizeof(Serve));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(Serve));

	len += sizeof(Serve);

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.serve.registerFlag,(uint8*)&buf[0],sizeof(Serve));
		BKP_WriteBackupDat(Register_LogoutInfo_Addr, (uint8*)&buf[0], Register_LogoutInfo_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.serve.registerFlag,(uint8*)&buf_bkp[0],sizeof(Serve));
		eeprom_write(Register_LogoutInfo_Addr, (uint8*)&buf_bkp[0], Register_LogoutInfo_Len);
	}else{
		ss.registerFlag = false;
		setServePara(ss);
	}

	/*
	** 更新注册注册状态
	*/
	if(sysParam.serve.registerFlag == true){
		set_RegisterState(1);/*已注册*/
	}
}

/*
** get WIFI Name PW
*/
WifiSet get_WifiNamePwSysSet(void){
	return sysParam.wifiSet;
}

/*
** set Wifi Name  
*/
void set_WifiNameSysSet(uint8* space){
	uint8 buf[WifiNamePara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	WifiSet wifiSetTemp = sysParam.wifiSet;

	memcpy((uint8*)&wifiSetTemp.name[0],(uint8*)&space[0],32);

	 memcpy((uint8*)&buf[len],(uint8*)&wifiSetTemp.name[0],32);
 	len += 32;

 	crc = CRC16((uint8 *)&buf[0], len);
 	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
 	len += sizeof(uint16);

 	eeprom_write(WifiNamePara_Addr, (uint8 *)&buf[0], len);
 	BKP_WriteBackupDat(WifiNamePara_Addr, (uint8_t *)&buf[0], len);	

	/*
	** 更新系统参数
	*/
	sysParam.wifiSet = wifiSetTemp;
}

/*
** update Wifi Name
*/
void update_WifiNameSysPara(void){
	uint8 buf[WifiNamePara_Len] = {0};
	uint8 buf_bkp[WifiNamePara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	
	uint8 initName[32] = {"ZLD"};

	eeprom_read(WifiNamePara_Addr, (uint8 *)&buf[len], WifiNamePara_Len);
	BKP_ReadBackupRegDat(WifiNamePara_Addr, (uint8*)&buf_bkp[len], WifiNamePara_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], 32);
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], 32);

	len += 32;

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.wifiSet.name[0],(uint8*)&buf[0],32);
		BKP_WriteBackupDat(WifiNamePara_Addr, (uint8*)&buf[0], WifiNamePara_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.wifiSet.name[0],(uint8*)&buf_bkp[0],32);
		eeprom_write(WifiNamePara_Addr, (uint8*)&buf_bkp[0], WifiNamePara_Len);
	}else{	
		set_WifiNameSysSet((uint8 *)&initName[0]);
	}
}

/*
** set Wifi Pw 
*/
void set_WifiPwSysSet(uint8* space){
	uint8 buf[WifiPwPara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	WifiSet wifiSetTemp = sysParam.wifiSet;

	memcpy((uint8*)&wifiSetTemp.pw[0],(uint8*)&space[0],32);

	 memcpy((uint8*)&buf[len],(uint8*)&wifiSetTemp.pw[0],32);
 	len += 32;

 	crc = CRC16((uint8 *)&buf[0], len);
 	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
 	len += sizeof(uint16);

 	eeprom_write(WifiPwPara_Addr, (uint8 *)&buf[0], len);
 	BKP_WriteBackupDat(WifiPwPara_Addr, (uint8_t *)&buf[0], len);	
	
	/*
	** 更新系统参数
	*/
	sysParam.wifiSet = wifiSetTemp;
}

/*
** update Wifi Pw
*/
void update_WifiPwSysPara(void){
	uint8 buf[WifiPwPara_Len] = {0};
	uint8 buf_bkp[WifiPwPara_Len] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	
	uint8 initPw[32] = {"zld16888888"};

	eeprom_read(WifiPwPara_Addr, (uint8 *)&buf[len], WifiPwPara_Len);
	BKP_ReadBackupRegDat(WifiPwPara_Addr, (uint8*)&buf_bkp[len], WifiPwPara_Len);

	/*
	** 计算CRC16校验
	*/
	crc = CRC16((uint8 *)&buf[len], 32);
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], 32);

	len += 32;

	/*
	** get CRC16校验
	*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&sysParam.wifiSet.pw[0],(uint8*)&buf[0],32);
		BKP_WriteBackupDat(WifiPwPara_Addr, (uint8*)&buf[0], WifiPwPara_Len);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&sysParam.wifiSet.pw[0],(uint8*)&buf_bkp[0],32);
		eeprom_write(WifiPwPara_Addr, (uint8*)&buf_bkp[0], WifiPwPara_Len);
	}else{	
		set_WifiPwSysSet((uint8 *)&initPw[0]);
	}
}


/*
** 等待MCU 稳定时间
*/
void wait_McuVoltStableTime(void){
	uint16 i=0;
	uint16 j = 0;
	for(i=0;i<4000;i++){
		for(j=0;j<1000;j++){
			/*
			** 更新看门狗寄存器--喂狗
			*/
			watchdogUpdate();
		}
	}
}

/*
** 系统参数初始化
*/
void init_SysPara(void){
	/*
	** 等待MCU 稳定时间
	*/
	wait_McuVoltStableTime();
	/*
	** update 服务端参数
	*/
	update_ServerPara();	
	/*
	** get电池包存储队列的大小(个数)--初始参数
	*/
	get_SizeBatFile();
	/*
	** 初始化更新 Ctr文件名长度
	*/
	init_UpgrCtrFileNameLen();
	/*
	** 初始化控制板文件名
	*/
	init_sysCtrFileName();
	/*
	** 初始上电更新通讯板参数--init_UpdateUpgrCom
	*/
	init_UpdateUpgrComFilePara();
	/*
	** 初始化更新电池包固件参数
	*/
	init_UpdateBatFileParam()	;
	/*
	** 更新控制板软件/通讯板软件版本号
	*/
	updateSoftVerPara();	
	/*----------------------检测:实际运行版本号同文件名版本号是否一致-----------------------------*/
	/*
	** 每次发布新版软件必须变更函数内关于文件名和软件版本号的定义
	*/
	 reset_ChkCtrSoftVer();
	/*--------------------------------------------------------------------------------------------*/

	/*-----------------------------------三相电压校准---------------------------------------------*/
	reset_PhaseVoltAdjust();
	/*--------------------------------------------------------------------------------------------*/
	
	/*
	** update 系统温度值
	*/
	update_Tsave();
	/*
	** update SOC阈值
	*/
	update_SocLimit();	
	/*
	** update 充电时间阈值
	*/
	update_ChgTimeLimit();
	/*
	** update 充电器过温阈值
	*/
	update_ChgOverTempLimit();	
	/*
	** update 充电器过温阈值
	*/
	update_ChgDoorOTLimit();	
	/*
	** update 域名
	*/
	update_SysDomain();	
	/*
	** update Sys 设备ID
	*/
	update_SysDeviceId();	
	/*
	** update Wifi Name
	*/
	update_WifiNameSysPara();	
	/*
	** update Wifi Pw
	*/
	update_WifiPwSysPara();	
}

