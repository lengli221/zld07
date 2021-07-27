#include "includes.h"
/*
**CAN1 Rx存储数据之变量处理
*/
static CAN_Rx_MsgInfo can1RxMsgInfo;

/*
** CAN1 Tx发送存储数据
*/
static CAN_TX_MsgInfo can1TxMsgInfo;
static CAN_TX_MsgInfo can2TxMsgInfo;

/*
**CAN1 参数初始化处理
*/
static void CAN1_ParamInit(void){
	memset((uint8*)&can1RxMsgInfo.msg[0].StdId,0x00,sizeof(CAN_Rx_MsgInfo));
} 

/*
**CAN1 时钟 引脚 配置
*/
void CAN1_Configure(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	/*
	**配置引脚时钟 引脚工作模式及初始化处理
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	/*
	** 配置引脚复用 明确207和105 复用时钟和引脚复用的区别 正常引脚配置区别
	*/
	GPIO_PinAFConfig((GPIO_TypeDef *)GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
	GPIO_PinAFConfig((GPIO_TypeDef *)GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//CAN_Rx | CAN_Tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init((GPIO_TypeDef *)GPIOD, (GPIO_InitTypeDef *)&GPIO_InitStructure);

	/*
	**CAN1寄存器默认初始化
	*/
	CAN_DeInit((CAN_TypeDef *)CAN1);

	/*
	**CAN1外设时钟使能
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
}

/*
**CAN1滤波器配置
*/
void CAN1_Cfg_Filter(void){
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	FilterDesign filterDesginTmp = Sys_GetFilterDesign1();
	/*
	**CAN过滤器激活寄存器CAN_FxRi 疑问:存在过滤器组的原因? 使用多个过滤组的业务逻辑
	*/
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	/*
	**标识符过滤 
	**1)--1.屏蔽为模式 2.标识符列表模式
	**2)--过滤器组的位宽和模式设置
	*/
  	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16)(filterDesginTmp.id.id>>16);
  	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16)(filterDesginTmp.id.id&0x0000FFFF);
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16)(filterDesginTmp.idMask.id>>16);
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16)(filterDesginTmp.idMask.id&0x0000FFFF);	
	/*
	**CAN过滤器FIFO关联寄存器CAN_FFA1R
	*/
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	/*
	**CAN过滤激活寄存器 CAN_FA1R
	*/
  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit((CAN_FilterInitTypeDef *)&CAN_FilterInitStructure);
}

/*
**CAN1 参数配置 波特率设置 中断设置
*/
void CAN1_BaudRate(void){
	CAN_InitTypeDef CAN_InitStructure;
	uint8 ret = 0;

	CAN_StructInit((CAN_InitTypeDef *)&CAN_InitStructure);
	/*
	**CAN1初始化
	**1.CAN1时间触发通信模式
	**2.自动离线管理--离线恢复+CAN网络稳定性分析
	**3.睡眠模式--自动唤醒处理方案
	**4.禁止报文自动重传模式--DISABLE:重传--1.(发送)邮箱是否存在关系 2.错误管理是否触发
	**5.接收FIFO锁定模式(Receive FIFO locked mode)--DISABLE:覆盖保留最新报文
	**6.发送FIFO优先级 (Transmit FIFO priority)--DISABLE:优先级标识符决定
	**7.CAN正常模式
	*/
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE/*DISABLE*/;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/*
	**CAN1波特率 = 1MBps
	** The maximum allowed frequency of the low-speed APB1 domain is 30 MHz
	*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	#if CAN1_BaudRateParam == CAN1_BaudRateParam_1M
		CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
		CAN_InitStructure.CAN_Prescaler = 2;
	#elif CAN1_BaudRateParam == CAN1_BaudRateParam_500K
		CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
		CAN_InitStructure.CAN_Prescaler = 5;
	#elif CAN1_BaudRateParam == CAN1_BaudRateParam_250K
		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
		CAN_InitStructure.CAN_Prescaler = 10;	
	#elif CAN1_BaudRateParam == CAN1_BaudRateParam_50K		
		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
		CAN_InitStructure.CAN_Prescaler = 50;	
	#elif CAN1_BaudRateParam == CAN1_BaudRateParam_100K		
		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
		CAN_InitStructure.CAN_Prescaler = 25;	
	#elif CAN1_BaudRateParam == CAN1_BaudRateParam_125K		
		CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
		CAN_InitStructure.CAN_Prescaler = 20;			
	#endif
	
	ret = CAN_Init((CAN_TypeDef *)CAN1, (CAN_InitTypeDef *)&CAN_InitStructure);

	switch(ret){
		case CAN_InitStatus_Failed:
			
			break;
		case CAN_InitStatus_Success:
			
			break;
	}

	/*
	**CAN中断设置
	**FIFO 0消息挂号中断使能 (FIFO message pending interrupt enable)
	** Note:
			1.Once a message has been stored in the FIFO, the FMP[1:0] bits are updated and an
				interrupt request is generated if the FMPIE bit in the CAN_IER register is set.
	*/
	CAN_ITConfig((CAN_TypeDef *)CAN1, CAN_IT_FMP0, ENABLE);
}

/*
**CAN中断初始化
*/
void CAN1_NVIC_Init(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init((NVIC_InitTypeDef *)&NVIC_InitStructure);
}

/*
**CAN1接收中断函数
*/
void CAN1_RX0_IRQHandler(void){
	CanRxMsg RxMessage = {0};
	uint16 i = can1RxMsgInfo.rx_wptr;

	/*
	**复位值存在不确定性
	*/
	memset((uint8*)&RxMessage.StdId,0x00,sizeof(CanRxMsg));

	if(CAN_GetITStatus((CAN_TypeDef *)CAN1, CAN_IT_FMP0) != RESET){
		CAN_Receive((CAN_TypeDef *)CAN1, CAN_FIFO0, (CanRxMsg *)&RxMessage);	
		can1RxMsgInfo.msg[i] = RxMessage;
		can1RxMsgInfo.rx_wptr++;
		if(can1RxMsgInfo.rx_wptr==CAN_Rx_MaxNum){
			can1RxMsgInfo.rx_wptr = 0;
		}
		
		/*
		**分析清中断标志函数中为何无相应标志:CAN_IT_FMP0
		**理论分析:
		*/
		CAN_ClearITPendingBit((CAN_TypeDef *)CAN1, CAN_IT_FMP0);
	}
}

/*
** CAN1 Rx数据获取
*/
bool CAN1_GetMsgInfo(CanRxMsg *msg){
	bool ret = false;
	uint16 i=0;

	if(can1RxMsgInfo.rx_rptr == can1RxMsgInfo.rx_wptr){
		ret = false;
	}else{
		i = can1RxMsgInfo.rx_rptr;
		*msg = can1RxMsgInfo.msg[i];
		/*
		**清存储区域内CAN相应存储的CAN邮箱数据
		*/
		memset((uint8*)&can1RxMsgInfo.msg[i].StdId,0x00,sizeof(CanRxMsg));
		can1RxMsgInfo.rx_rptr++;
		if(can1RxMsgInfo.rx_rptr == CAN_Rx_MaxNum){
			can1RxMsgInfo.rx_rptr = 0;
		}
		ret = true;
	}

	return ret;
}

/*
**调试CAN 发送数据
*/
#if Debug_CAN1_TxFun == 1
void DEBUG_CAN1_TxFun(void){
	CanTxMsg TxMessage = {0};
	uint8 ret = CAN_TxStatus_NoMailBox;
	IdAddr idAddrTmp = Sys_GetIdAddr();

	/*
	**复位值存在不确定性
	*/
	memset((uint8*)&TxMessage.StdId,0x00,sizeof(CanTxMsg));

	/*
	**问题
	**:1.分析时间戳 为何官方未提供时间戳控制位系统函数
	*/
	TxMessage.StdId = 0;/*Standard identifier*/
	TxMessage.ExtId |= (uint32)((idAddrTmp.dst<<8) | idAddrTmp.src);/*Extended identifier*/
	/*
	**Identifier extension
	**Note:This bit defines the identifier type of message in the mailbox.
	*/
	TxMessage.IDE = CAN_Id_Extended;
	TxMessage.RTR = CAN_RTR_Data;/*Data Frame*/
	/*
	**Data length code
	**Note:This field defines the number of data bytes a data frame contains or a remote frame request
	*/
	TxMessage.DLC = 4;
	/*
	**Data Item
	*/
	TxMessage.Data[0] = 0x92;
	TxMessage.Data[1] = 0x93;
	TxMessage.Data[2] = 0x02;
	TxMessage.Data[3] = 0x21;
	
	ret = CAN_Transmit((CAN_TypeDef *)CAN1, (CanTxMsg *)&TxMessage);

	switch(ret){
		case CAN_TxStatus_NoMailBox:
			
			break;
		default:
			
			break;
	}
}
#endif

/*
**CAN1 Transmit数据函数处理
*/
void CAN1_TransmitData(uint8 len, uint8* tx,uint8 dst,uint8 funId){
	CanTxMsg TxMessage = {0};
	uint8 ret = CAN_TxStatus_NoMailBox;
	IdAddr idAddrTmp = Sys_GetIdAddr();

	/*
	**复位值存在不确定性
	*/
	memset((uint8*)&TxMessage.StdId,0x00,sizeof(CanTxMsg));

	/*
	**问题
	**:1.分析时间戳 为何官方未提供时间戳控制位系统函数
	*/
	TxMessage.StdId = 0;/*Standard identifier*/
	TxMessage.ExtId |= (uint32)((funId<<16) | (dst<<8) | idAddrTmp.src);/*Extended identifier*/
	/*
	**Identifier extension
	**Note:This bit defines the identifier type of message in the mailbox.
	*/
	TxMessage.IDE = CAN_Id_Extended;
	TxMessage.RTR = CAN_RTR_Data;/*Data Frame*/
	/*
	**Data length code
	**Note:This field defines the number of data bytes a data frame contains or a remote frame request
	*/
	TxMessage.DLC = len;
	/*
	**Data Item
	*/
	memcpy((uint8*)&TxMessage.Data[0],(uint8*)&tx[0],len);

	/*
	** Note:
		1.查找空闲邮箱再进行数据项的填充发送的理由:
			Transmission handling:Once the mailbox has left empty state, the software no longer has write access to the mailbox registers
	*/
	/*ret = CAN_Transmit((CAN_TypeDef *)CAN1, (CanTxMsg *)&TxMessage);*/

	/*
	** 优化逻辑 防止指针溢出 模拟中断填充数据
	*/
	if(dst < 24 || dst == DstAddr_BoardCast_Define){
		can1TxMsgInfo.msg[can1TxMsgInfo.wCnt++] = TxMessage;
		if(can1TxMsgInfo.wCnt== CAN_Tx_MaxNum){
			can1TxMsgInfo.wCnt= 0;
		}
	}

	if((dst>=24 && dst < 48) || dst == DstAddr_BoardCast_Define){
		can2TxMsgInfo.msg[can2TxMsgInfo.wCnt++] = TxMessage;
		if(can2TxMsgInfo.wCnt== CAN_Tx_MaxNum){
			can2TxMsgInfo.wCnt= 0;
		}
	}	
	
	switch(ret){
		case CAN_TxStatus_NoMailBox:
			
			break;
		default:
			
			break;
	}
}


/*
** CAN1 Tx Task
*/
void CAN1_TxDataTask(void){
	CanTxMsg TxMessage = {0};
	uint16 i = 0;
	uint8 ret = CAN_TxStatus_NoMailBox; 
	uint8 ret2 = CAN_TxStatus_NoMailBox; 
	uint8 circ = 0;
	uint8 circ2 = 0;
	static uint32 itick = 0;
	static uint32 itick2= 0;
	static uint8 itickCnt = 0;
	static uint8 itickCnt2 = 0;

	TickOut((uint32 *)&itick, 0x00);
	TickOut((uint32 *)&itick2, 0x00);

	for(circ = 0;circ < 3;/*circ++*/){
		if(can1TxMsgInfo.rCnt != can1TxMsgInfo.wCnt){
			i = can1TxMsgInfo.rCnt;		
			TxMessage = can1TxMsgInfo.msg[i];
			ret = CAN_Transmit((CAN_TypeDef *)CAN1, (CanTxMsg *)&TxMessage);/*地址0-23采用CAN1 广播地址 发送*/
			if(ret != CAN_TxStatus_NoMailBox){
				can1TxMsgInfo.rCnt++;
				if(can1TxMsgInfo.rCnt == CAN_Tx_MaxNum){
					can1TxMsgInfo.rCnt = 0;
				}
				circ++;
				itickCnt = 0;/*匹配电阻有效*/
			}else{
				if(itickCnt>=5){/*匹配电阻失效*/
					itickCnt = 5;
					break;
				}
				Sleep(2);
				if(TickOut((uint32 *)&itick, 1000) == true){
					itickCnt++;
					break;
				}
			}
			/*
			** 更新看门狗寄存器--喂狗
			*/
			watchdogUpdate();
		}else{
			break;
		}
	}

	for(circ2 = 0;circ2 < 3;){
		if(can2TxMsgInfo.rCnt != can2TxMsgInfo.wCnt){
			i = can2TxMsgInfo.rCnt;		
			TxMessage = can2TxMsgInfo.msg[i];
			ret2 = CAN_Transmit((CAN_TypeDef *)CAN2, (CanTxMsg *)&TxMessage);/*地址24-47采用CAN2 广播地址 发送*/			
			if(ret2 != CAN_TxStatus_NoMailBox){
				can2TxMsgInfo.rCnt++;
				if(can2TxMsgInfo.rCnt == CAN_Tx_MaxNum){
					can2TxMsgInfo.rCnt = 0;
				}
				circ2++;
				itickCnt2 = 0;/*匹配电阻有效*/
			}else{
				if(itickCnt2 >=5){/*匹配电阻失效*/
					itickCnt2 = 5;
					break;
				}
				Sleep(2);
				if(TickOut((uint32 *)&itick2, 1000) == true){
					itickCnt2++;
					break;
				}
			}	
			/*
			** 更新看门狗寄存器--喂狗
			*/
			watchdogUpdate();
		}	else{
			break;
		}	
	}
}

/*
**CAN1初始化配置
*/
void CAN1_Cfg_Init(void){
	/*
	**CAN1 参数初始化处理
	*/
	CAN1_ParamInit();
	/*
	**CAN1 时钟 引脚 配置
	*/
	CAN1_Configure();
	/*
	**CAN1滤波器配置
	*/
	CAN1_Cfg_Filter();
	/*
	**CAN1 参数配置 波特率设置 中断设置
	*/
	CAN1_BaudRate();	
}

