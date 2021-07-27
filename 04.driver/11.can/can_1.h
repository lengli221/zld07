#ifndef __CAN_1_H
#define __CAN_1_H

/*
** CAN模块之波特率速率
*/
#define CAN1_BaudRateParam_1M								1
#define CAN1_BaudRateParam_500K								2
#define CAN1_BaudRateParam_250K								3
#define CAN1_BaudRateParam_50K								4
#define CAN1_BaudRateParam_100K								5
#define CAN1_BaudRateParam_125K								6
#define CAN1_BaudRateParam										CAN1_BaudRateParam_125K	

/*
**CAN中断初始化
*/
void CAN1_NVIC_Init(void);
/*
** CAN1 Tx Task
*/
void CAN1_TxDataTask(void);
/*
**CAN1初始化配置
*/
void CAN1_Cfg_Init(void);
/*
** CAN1 Rx数据获取
*/
bool CAN1_GetMsgInfo(CanRxMsg *msg);
/*
**调试CAN 发送数据
*/
#if Debug_CAN1_TxFun == 1
void DEBUG_CAN1_TxFun(void);
#endif
/*
**CAN1 Transmit数据函数处理
*/
void CAN1_TransmitData(uint8 len, uint8* tx,uint8 dst,uint8 funId);

#endif

