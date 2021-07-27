#include "includes.h"

void ComOpen(int32 port, int32 boadrat, int32 Parity, int32 dataBit, int32 stopBit){
	USART_InitTypeDef USART_InitStructure;
	uint16_t tmpParity;
	uint16_t tmpStopBits;
	uint16_t WordLength;

	USART_TypeDef *ch;

	/*port*/
	if(port == 1){
		ch = USART1;
	}else if(port == 2){
		ch = USART2;
	}else if(port == 3){
		ch = USART3;
	}else if(port == 4){
		ch = UART4;
	}else if(port == 5){
		ch = UART5;
	}else if(port == 6){
		ch = USART6;
	}

	/*parity*/
	if(Parity == 0){
		tmpParity =  USART_Parity_No;
	}else if (Parity == 1){
		tmpParity =  USART_Parity_Even;
	}else if (Parity == 2){
		tmpParity = USART_Parity_Odd;
	}else{
		tmpParity =  USART_Parity_No;
	}

	/*数据长度*/
	if(dataBit == 8){
		WordLength = USART_WordLength_8b;
	}else if (dataBit == 9){
		WordLength =  USART_WordLength_9b;
	}else{
		WordLength = USART_WordLength_8b;
	}

	/*停止位*/
	if (stopBit ==  1){
		tmpStopBits  = USART_StopBits_1;
	}else if(stopBit ==  2){
		tmpStopBits = USART_StopBits_2;
	}else if(stopBit == 3){
		tmpStopBits =  USART_StopBits_0_5;
	}else if(stopBit == 4){
		tmpStopBits = USART_StopBits_1_5;
	}else{
		tmpStopBits = USART_StopBits_1;
	}

	USART_InitStructure.USART_BaudRate = boadrat;
	USART_InitStructure.USART_WordLength = WordLength;
	USART_InitStructure.USART_StopBits = tmpStopBits;
	USART_InitStructure.USART_Parity = tmpParity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(ch, &USART_InitStructure);
	USART_ITConfig(ch, USART_IT_RXNE, ENABLE);
	USART_Cmd(ch, ENABLE);
}

bool com_open(int8 port, int32 baud_rate, int32 dataBit, int8 parity, int8 stopBit){
	if(port > 6)
	{
		return false;
	}

	if(parity > 0)
		dataBit += 1;
	ComOpen(port, baud_rate, parity, dataBit, stopBit);

	return true;
}

/*
** USART通讯初始化
*/
void Com_Init(void){
	Usart2_Init();
	Usart2_DmaConfig();
	Usart2_BufferInit();

	Usart3_Init();
	Usart3_DmaConfig();
	Usart3_BufferInit();

	com_open(BSP_ComUpperLayer, 115200, 8, BSP_COM_NO, BSP_COM_STOP1);
	com_open(BSP_ComDW_Usart, 115200, 8, BSP_COM_NO, BSP_COM_STOP1); 	
}

void Com_Write(int8 port, uint8 *buf, int32 len, int32 time_out){
	switch(port){
		case BSP_ComUpperLayer:/*硬件接口:上层协议*/
			Usart2_DmaWrite(buf, len);
			break;
		case BSP_ComDW_Usart:/*硬件接口:迪文屏*/
			Usart3_DmaWrite(buf, len);
			break;
		default:	
			break;
	}
}

int32 Com_Read(int8 port, uint8 *buf, int32 len, int32 time_out){
	switch(port){
		case BSP_ComUpperLayer:	/*硬件接口:上层协议*/
			return Usart2_Rcv(buf, len);	
		case BSP_ComDW_Usart:/*硬件接口:迪文屏*/
			return Usart3_Rcv(buf, len);
		default:
			break;
	}
	return -1;
}

void BSP_SmSend(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen){
	if(ucComType == BSP_ComType_USART){
		Com_Write(ucSmNo,(uint8 *)msgbuf, *uclen, 0);
	}	
}

void BSP_SmRecv(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen){
	int len = 0;
	
	if(ucComType == BSP_ComType_USART){ 
		len = Com_Read(ucSmNo, (uint8 *)msgbuf, 1000, 0);
		if(len < 0){
			*uclen = 0;
		}else{
			*uclen = len;
		}
	}
}


