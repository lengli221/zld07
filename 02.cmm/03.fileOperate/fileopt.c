#include "includes.h"

/*
** 软件版本号 限制ASCII数
*/
#define SoftVerNum					3

/*
** 分析是否是.bin文件以及获取下标参数
** @return:返回版本信息下标
*/
int16 fileNameVerSuffix(uint8* data,uint16 len){
	int16 indexSuffix = -1;
	uint16 i = 0;

	for(i = 0;i < len - 3;i++){
		if(data[i] == '.' && data[i+1] == 'b'&& data[i+2] == 'i'&& data[i+3] == 'n'){
			/*
			** 返回版本信息下标
			*/
			if(i >= SoftVerNum){/*目前美团软件版本号:3 ASCII*/
				indexSuffix = (i - SoftVerNum);
			}
			break;
		}
	}
	return indexSuffix;
}

/*
** 文件名版本号操作
*/
bool FileNameVerOpt(uint8* data,uint16 len){
	bool ret = false;
	int16 indexSuffix = fileNameVerSuffix((uint8 *)&data[0], len);
	uint16 ver = 0;
	uint16 i = 0;
	uint8 temp[SoftVerNum] = {0};
	UpgrFilePara upgr = getUpgrFilePara();

	if(indexSuffix != -1){/*.bin文件,合法*/
		memcpy((uint8*)&temp[0],(uint8*)&data[indexSuffix],SoftVerNum);
		for(i = 0;i < SoftVerNum;i++){/*从文件名提取软件版本号*/
			ver *= 10;
			ver += temp[i] - '0';
		}
		/*
		** 是否增加文件名与硬件版本号的匹配分析
		*/
		upgr.upgrFileType.fileNameLen = len;
		upgr.upgrFileType.softVer = ver;
		setUpgrFilePara(upgr);
		
		return true;
	}

	return ret;
}

