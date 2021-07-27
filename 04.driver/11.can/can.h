#ifndef __CAN_H_
#define __CAN_H_

/*
**CAN Rx连续接收未处理数据帧数
*/
#define CAN_Rx_MaxNum						(uint8)150
#define CAN_Tx_MaxNum						(uint8)150

/*
** CAN Port声明
*/
#define CAN_Port_1								(uint8)0x01
#define CAN_Port_2								(uint8)0x02

/*
**CAN Rx处理结构体
*/
typedef struct{
	CanRxMsg msg[CAN_Rx_MaxNum];/*CAN Rx邮箱信息*/
	uint16 rx_wptr;/*写存储位置标记*/
	uint16 rx_rptr;/*读存储位置标记*/
}CAN_Rx_MsgInfo;


/*
** CAN Tx 处理结构体
*/
typedef struct{
	CanTxMsg msg[CAN_Tx_MaxNum];/*CAN Tx邮箱信息*/
	uint16 wCnt;
	uint16 rCnt;
}CAN_TX_MsgInfo;

/*
**CAN Rx数据分析
*/
bool CAN_RecAnalysis(uint8 canPort,CanRxMsg *msg);
/*
**CAN Transmit 分析
*/
void CAN_TransmitAnalysis(uint8 canPort,uint8 len,uint8* tx,uint8 dst,uint8 funId);
/*
**CAN1初始化
*/
void CAN_Cfg_Init(void);

#endif

