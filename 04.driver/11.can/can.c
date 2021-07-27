/*
**CAN相关说明 ***************************************************************************************
**Note:
*****1.时钟:CAN2单独使用也必须开启CAN1时钟--疑问:1.仅针对模块时钟还是包含引脚使能时钟:不是
******分析:函数CAN_FilterInit和官方参考手册RM0008中CAN filter register寄存器均是以CAN基准地址
**设置参数
*****2.过滤器设置
**疑问:CAN相关寄存器的设置顺序,是否影响逻辑关系
*****1.Control/Status/Configuration
*****2.Acceptance Filters
*****3.Master/Slave Receive FIFO/1
*****4.Master/Slave Tx Mailboxes
*****************************************************************************************************
*****远程升级:CAN升级拆包滤波方案选取
*****************************************************************************************************
*/
#include "includes.h"


/*
**CAN Rx数据分析
*/
bool CAN_RecAnalysis(uint8 canPort,CanRxMsg *msg){
	bool ret=false;

	switch(canPort){
		case CAN_Port_1:
			ret = CAN1_GetMsgInfo((CanRxMsg *)msg);
			break;
		case CAN_Port_2:
			ret = CAN2_GetMsgInfo((CanRxMsg *)msg);
			break;
		default:
			
			break;
	}
	return ret;
}

/*
**CAN Transmit 分析
*/
void CAN_TransmitAnalysis(uint8 canPort,uint8 len,uint8* tx,uint8 dst,uint8 funId){
	switch(canPort){
		case CAN_Port_1:
			CAN1_TransmitData(len, (uint8 *)&tx[0],dst,funId);
			break;
		case CAN_Port_2:

			break;
	}
}


/*
**CAN初始化
*/
void CAN_Cfg_Init(void){
	/*
	** CAN1初始化配置
	*/
	CAN1_Cfg_Init();
	/*
	** CAN2初始化配置
	*/
	CAN2_Cfg_Init();	
}

