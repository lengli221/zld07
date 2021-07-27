#include "includes.h"
/*
**CAN2 Rx存储数据之变量处理
*/
static CAN_Rx_MsgInfo can2RxMsgInfo;

/*
**CAN1 参数初始化处理
*/
static void CAN2_ParamInit(void){
	memset((uint8*)&can2RxMsgInfo.msg[0].StdId,0x00,sizeof(CAN_Rx_MsgInfo));
} 

/*
**CAN2 时钟 引脚 配置
*/
void CAN2_Configure(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	/*
	**配置引脚时钟 引脚工作模式及初始化处理
	*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/*
	** 配置引脚复用 明确207和105 复用时钟和引脚复用的区别 正常引脚配置区别
	*/
	GPIO_PinAFConfig((GPIO_TypeDef *)GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
	GPIO_PinAFConfig((GPIO_TypeDef *)GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;//CAN_Rx | CAN_Tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init((GPIO_TypeDef *)GPIOB, (GPIO_InitTypeDef *)&GPIO_InitStructure);

	/*
	**CAN1寄存器默认初始化
	*/
	CAN_DeInit((CAN_TypeDef *)CAN2);

	/*
	**CAN1外设时钟使能
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
}

/*
**CAN2滤波器配置
*/
void CAN2_Cfg_Filter(void){
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	FilterDesign filterDesginTmp = Sys_GetFilterDesign1();
	/*
	**CAN过滤器激活寄存器CAN_FxRi 疑问:存在过滤器组的原因? 使用多个过滤组的业务逻辑
	*/
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
	/*
	**标识符过滤 
	**1)--1.屏蔽为模式 2.标识符列表模式
	**2)--过滤器组的位宽和模式设置
	*/
  	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  	CAN_FilterInitStructure.CAN_FilterIdHigh =(uint16)(filterDesginTmp.id.id>>16);
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
**CAN2 参数配置 波特率设置 中断设置
*/
void CAN2_BaudRate(void){
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
	
	ret = CAN_Init((CAN_TypeDef *)CAN2, (CAN_InitTypeDef *)&CAN_InitStructure);

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
	CAN_ITConfig((CAN_TypeDef *)CAN2, CAN_IT_FMP0, ENABLE);
}

/*
** CAN中断初始化
*/
void CAN2_NVIC_Init(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init((NVIC_InitTypeDef *)&NVIC_InitStructure);
}

/*
**CAN1接收中断函数
*/
void CAN2_RX0_IRQHandler(void){
	CanRxMsg RxMessage = {0};
	uint16 i = can2RxMsgInfo.rx_wptr;

	/*
	**复位值存在不确定性
	*/
	memset((uint8*)&RxMessage.StdId,0x00,sizeof(CanRxMsg));

	if(CAN_GetITStatus((CAN_TypeDef *)CAN2, CAN_IT_FMP0) != RESET){
		CAN_Receive((CAN_TypeDef *)CAN2, CAN_FIFO0, (CanRxMsg *)&RxMessage);	
		can2RxMsgInfo.msg[i] = RxMessage;
		can2RxMsgInfo.rx_wptr++;
		if(can2RxMsgInfo.rx_wptr==CAN_Rx_MaxNum){
			can2RxMsgInfo.rx_wptr = 0;
		}
		
		/*
		**分析清中断标志函数中为何无相应标志:CAN_IT_FMP0
		**理论分析:
		*/
		CAN_ClearITPendingBit((CAN_TypeDef *)CAN2, CAN_IT_FMP0);
	}
}

/*
** CAN2 Rx数据获取
*/
bool CAN2_GetMsgInfo(CanRxMsg *msg){
	bool ret = false;
	uint16 i=0;

	if(can2RxMsgInfo.rx_rptr == can2RxMsgInfo.rx_wptr){
		ret = false;
	}else{
		i = can2RxMsgInfo.rx_rptr;
		*msg = can2RxMsgInfo.msg[i];
		/*
		**清存储区域内CAN相应存储的CAN邮箱数据
		*/
		memset((uint8*)&can2RxMsgInfo.msg[i].StdId,0x00,sizeof(CanRxMsg));
		can2RxMsgInfo.rx_rptr++;
		if(can2RxMsgInfo.rx_rptr == CAN_Rx_MaxNum){
			can2RxMsgInfo.rx_rptr = 0;
		}
		ret = true;
	}

	return ret;
}

/*
**CAN2初始化配置
*/
void CAN2_Cfg_Init(void){
	/*
	**CAN1 参数初始化处理
	*/
	CAN2_ParamInit();
	/*
	**CAN2 时钟 引脚 配置
	*/
	CAN2_Configure();
	/*
	**CAN2滤波器配置
	*/
	CAN2_Cfg_Filter();
	/*
	**CAN2 参数配置 波特率设置 中断设置
	*/
	CAN2_BaudRate();	
}

