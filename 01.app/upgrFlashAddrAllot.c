#include "includes.h"

/*
** get Flash 升级存储地址
*/
uint32 getUpgrFlashAddr(UpgrFilePara upgr){
	uint32 flashInitAddr = Upgrade_FlashAddrInit; 
	int16 loc = 0;
	
	
	if(upgr.upgrFileType.board == 0/*异常类型*/ || upgr.upgrFileType.board == ComBoradType){
		 flashInitAddr = Upgrade_FlashAddrInit; 
	}else{
		loc = get_BatLocation(upgr);
		flashInitAddr = Upgr_FlashAddr_Bat(loc);
	}

	return flashInitAddr;
}

