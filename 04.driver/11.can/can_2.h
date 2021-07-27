#ifndef __CAN2_H_
#define __CAN2_H_

/*
**CAN中断初始化
*/
void CAN2_NVIC_Init(void);
/*
** CAN2 Rx数据获取
*/
bool CAN2_GetMsgInfo(CanRxMsg *msg);
/*
**CAN2初始化配置
*/
void CAN2_Cfg_Init(void);

#endif

