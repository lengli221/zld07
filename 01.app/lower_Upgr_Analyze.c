#include "includes.h"

/*
** 下层协议参数变量定义
*/
extern LowerLayerParam llParam;

/*
** 下层协议之电池舱状态解析
*/
extern void LLParse_BatDoorState(uint8 rxlen,uint8* item,uint8 addr);
/*
** 电池升级状态
*/
extern void LLParse_UpgrState(uint8 rxlen,uint8* item,uint8 addr);
/*
** 充电仓系统信息
*/
extern void LLPrse_ChgSysInfo(uint8 rxlen,uint8* item,uint8 addr);
/*
** 下层协议之BMS信息
*/
extern void LLParse_BatInfo(uint8 rxlen,uint8* item,uint8 addr);
/*
** 更新软件版本 
*/
extern void LLParse_UpdateSoftVer(uint8 rxlen,uint8* item,uint8 addr);
/*
** 升级之查询通讯小板运行区域
*/
extern void UpgradeLLParse_ChkComRunArea(uint8 len,uint8* item,uint8 addr);
/*
** 升级之升级文件版本号
*/
extern void UpgradeLLParse_FileVer(uint8 len,uint8* item,uint8 addr);
/*
** 升级之文件字节数+文件内容校验
*/
extern void UpgradeLLParse_FileByteItemCrc(uint8 len,uint8* item,uint8 addr);
/*
** 升级之文件内容
*/
extern void UpgradeLLParse_FileItem(uint8 len,uint8* item,uint8 addr);
/*
** upgr Fail Reason
*/
extern void LLParse_UpgrFailReason(uint8 rxlen,uint8* item,uint8 addr);
/*
** 下层协议之数据解析
*/
static const LowerUpgrFunTable lowerUpgrFunTable[] = {
	{LL_FunId_BatDoorState,LLParse_BatDoorState},
	{LL_FunId_BatUpgrState,LLParse_UpgrState},
	{LL_FunId_ChgSysInfo,LLPrse_ChgSysInfo},
	{LL_FunId_BmsInfo,LLParse_BatInfo},
	{UpgradeLL_FunId_ChkAppRunVer,LLParse_UpdateSoftVer},
	{UpgradeLL_FunId_RunArea,UpgradeLLParse_ChkComRunArea},
	{UpgradeLL_FunId_Ver,UpgradeLLParse_FileVer},
	{UpgradeLL_FunId_FileByteCrc,UpgradeLLParse_FileByteItemCrc},
	{UpgradeLL_FunId_FileItem,UpgradeLLParse_FileItem},
	{LL_FunId_upgrFailReason,LLParse_UpgrFailReason}
};
static uint8 lowerUpgrFunTableNum = sizeof(lowerUpgrFunTable)/sizeof(LowerUpgrFunTable);

/*
** 下层协议之数据解析之任务函数处理
*/
void lowerUpgrFunAnalyze(void){
	CanRxMsg rxMsg = {0};
	uint8 funId = 0;
	uint8 i=0;
	uint8 deviceAddr = 0;	

	for(;CAN_RecAnalysis(CAN_Port_1, (CanRxMsg *)&rxMsg) == true;){
		funId = (uint8)((rxMsg.ExtId>>16)&0x000000FF);
		for(i=0;i<lowerUpgrFunTableNum;i++){
			if(funId == lowerUpgrFunTable[i].funId){
				deviceAddr = (uint8)(rxMsg.ExtId&0x000000FF);		
				/*
				** 增加地址容错处理
				*/
				if(deviceAddr<=(uint8)23){
					lowerUpgrFunTable[i].handle(rxMsg.DLC,(uint8*)&rxMsg.Data[0],deviceAddr);
					/*
					** 判断通讯是否失联
					*/
					TickOut((uint32 *)&llParam.comBoardChk[deviceAddr].itick, 0);
					llParam.comBoardChk[deviceAddr].cnt = 0;
					llParam.comBoardChk[deviceAddr].comAbn = false;
				}
				break;
			}
		}
		/*
		** 看门口喂狗
		*/
		watchdogUpdate();
	}

	for(;CAN_RecAnalysis(CAN_Port_2, (CanRxMsg *)&rxMsg) == true;){
		funId = (uint8)((rxMsg.ExtId>>16)&0x000000FF);
		for(i=0;i<lowerUpgrFunTableNum;i++){
			if(funId == lowerUpgrFunTable[i].funId){
				deviceAddr = (uint8)(rxMsg.ExtId&0x000000FF);	
				/*
				** 增加地址容错处理
				*/
				if(deviceAddr >= (uint8)24 && deviceAddr <= (uint8)47){
					lowerUpgrFunTable[i].handle(rxMsg.DLC,(uint8*)&rxMsg.Data[0],deviceAddr);
					/*
					** 判断通讯是否失联
					*/
					TickOut((uint32 *)&llParam.comBoardChk[deviceAddr].itick, 0);
					llParam.comBoardChk[deviceAddr].cnt = 0;
					llParam.comBoardChk[deviceAddr].comAbn = false;
				}
				break;
			}
		}
		/*
		** 看门口喂狗
		*/
		watchdogUpdate();
	}		
}

