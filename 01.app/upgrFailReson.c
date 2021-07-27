#include "includes.h"

/*---------------------电池升级失败原因上报--------------------------------*/
FIFOQueue fifoQueue[SysCtr_AllDoorNum] = {0};
UpgrFail upgrFail[SysCtr_AllDoorNum][FIFOQueue_DepthLimit] = {0};
/*-------------------------------------------------------------------------*/

/*原因是否未读取标志*/
DoorEUResonNoRead doorEUResonNoRead = {0};
/*
** 升级失败原因参数初始化--电池失败原因存在3(FIFOQueue_DepthLimit)级缓存
*/
void init_UpgrFailPara(void){
	uint16 i = 0;

	/*FIFO队列初始化*/
	for(i=0;i<SysCtr_AllDoorNum;i++){
		init_FIFOQueue((FIFOQueue *)&fifoQueue[i]);
	}
}

/*
** set--升级失败原因 -- 升级成功,失败均上报--协议V1.8
*/
void set_UpgrFailPara(uint8 type,UpgrFail u,uint8 loc){
	uint16 depth = 0;
	switch(type){
		case 0x03:/*电池固件包*/
			depth = size_FIFOQueue((FIFOQueue *)&fifoQueue[loc]);
			if(depth == FIFOQueue_DepthLimit){
				pop_FIFOQueue((FIFOQueue *)&fifoQueue[loc]);/*队列满之后丢掉最初失败原因*/
				depth = size_FIFOQueue((FIFOQueue *)&fifoQueue[loc]);
			}
			memset((uint8*)&upgrFail[loc][depth],0,sizeof(UpgrFail));
			upgrFail[loc][depth] = u;
			push_FIFOQueue((FIFOQueue *)&fifoQueue[loc], (void *)&upgrFail[loc][depth]);			
			break;
		case 0x04:/*充电器固件包*/		
			break;
	}

}

/*
** get--升级失败原因
*/
void* get_UpgrFailPara(uint8 type,uint8 loc){
	switch(type){
		case 0x03:/*电池固件包*/		
			return pop_FIFOQueue((FIFOQueue *)&fifoQueue[loc]);
		case 0x04:/*充电器固件包*/
			return NULL;
	}

	return NULL;
}

/*
** 20210518--检测是否存在升级原因未读取
*/
void runtimeChk_UpgrResonFlag(void){
	uint8 i = 0;
	
	for(i=0;i<SysCtr_AllDoorNum;i++){
		/*电池*/
		if(size_FIFOQueue((FIFOQueue *)&fifoQueue[i]) != 0){/*存在原因*/
			doorEUResonNoRead.bat |= (DoorNumDefine)((DoorNumDefine)0x01<<i);
		}else{
			doorEUResonNoRead.bat &= (DoorNumDefine)~((DoorNumDefine)1<<i);	
		}
	}
}

/*
** 20210518--获取原因上报
*/
DoorEUResonNoRead get_DoorEUResonNoRead(void){
	return doorEUResonNoRead;
}

