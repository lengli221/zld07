#include "includes.h"

uint8 uart1TxBuffer[UART1_BUFLEN_S];
uint8 uart1RxBuffer[UART1_BUFLEN_R];
static uint8 uart1DMABuffer[DMA1_BUFLEN];

DECLARE_QUEUE(uart1TxQueue,uart1TxBuffer, sizeof(uart1TxBuffer));
DECLARE_QUEUE(uart1RxQueue,uart1RxBuffer, sizeof(uart1RxBuffer));

bool uart1Busy = false;

/*
** 配置
*/
const UartInfo uart1Info={
    &uart1Busy,
    &uart1TxQueue,
    &uart1RxQueue
};

/*
** 串口初始化
*/
void Usart1_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*
** DMA 配置
*/
void Usart1_DmaConfig(void){
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_DeInit(DMA2_Stream7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_ADDRESS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32)uart1DMABuffer;
	DMA_InitStructure.DMA_BufferSize = DMA1_BUFLEN;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_HT, DISABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/*
** DMA 发送
*/
void Usart1_DmaSend(void){
	uint32 bufferCount = 0;
	while((bufferCount<DMA1_BUFLEN )&&(!QueueIsEmpty(uart1Info.TxQueue))){
		Dequeue(uart1Info.TxQueue,&uart1DMABuffer[bufferCount++]);
	}
	DMA2_Stream7->NDTR = bufferCount;
	DMA2_Stream7->M0AR = (uint32)uart1DMABuffer;
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA2_Stream7,ENABLE);
}

/*
** DMA Write
*/
bool Usart1_DmaWrite(uint8 * txBuf,uint32 txNum){
	uint32 timeOutCount = 4000;
	
	if( (txNum == 1) || (txNum > 256) || (txNum == 0)){
		return false;
	}else{
		while(timeOutCount--){
			if(!(*uart1Info.Busy)){
				*uart1Info.Busy = true;
				FlushQueue(uart1Info.TxQueue);
				/*FlushQueue(uart1Info.RxQueue);*/
				while(txNum>0){
					if(Enqueue(uart1Info.TxQueue, *txBuf++))
						txNum--;
					else
						break;
				}
				Usart1_DmaSend();
				/*20210323--对比:12A和6A区别*/
				while(*uart1Info.Busy){
					Sleep(1);
				}
				return true;
			}else{
				timeOutCount--;
				if(timeOutCount == 0){
					*uart1Info.Busy = false;
					FlushQueue(uart1Info.TxQueue);
					return false;
				}
			}
		}
	}
	return true;
}

/*
** DMA 发送中断
*/
void DMA2_Stream7_IRQHandler(void){
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_FEIF7|DMA_FLAG_DMEIF7|DMA_FLAG_TEIF7|DMA_FLAG_HTIF7);
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7)){
		DMA_Cmd(DMA2_Stream7,DISABLE);
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);
		DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,DISABLE);
		DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
	}
}

/*
** 初始化缓冲区
*/
void Usart1_BufferInit(void){
	FlushQueue(uart1Info.TxQueue);
	FlushQueue(uart1Info.RxQueue);
	*uart1Info.Busy = false;
	memset(uart1TxBuffer, 0, sizeof(uart1TxBuffer));
	memset(uart1RxBuffer, 0, sizeof(uart1RxBuffer));
	memset(uart1DMABuffer, 0, sizeof(uart1DMABuffer));
}

/*
** 从缓冲区读数据
*/
int Usart1_Rcv(uint8 *rxBuf, int rxNum){
	int32 iTempRxNum = 0;
	
	if(rxNum == 0)
		return -1;
	if(QueueIsEmpty(uart1Info.RxQueue))
		return -1;
	while(rxNum >0 && (!QueueIsEmpty(uart1Info.RxQueue))){
		USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		Dequeue(uart1Info.RxQueue, rxBuf++);
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
		rxNum--;
		iTempRxNum++;
	};
	return iTempRxNum;
}

/*
** usart1 中断
*/
uint32 cntt = 0;/*统计应用层是否越界和解包时差*/
void USART1_IRQHandler(void){  
	uint8 dat;

	if(USART_GetFlagStatus(USART1,USART_FLAG_FE|USART_FLAG_NE|USART_FLAG_ORE)){
		USART_ReceiveData(USART1);
	}
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
		USART_ITConfig(USART1,USART_IT_RXNE,DISABLE); 
		dat = USART_ReceiveData(USART1);
		Enqueue(uart1Info.RxQueue,dat);
		cntt++;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	}
	
	if(USART_GetITStatus(USART1,USART_IT_TC)!=RESET){
		USART_ITConfig(USART1,USART_IT_TC,DISABLE); 
		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); 
		USART_ClearITPendingBit(USART1, USART_IT_TC);
		* uart1Info.Busy = false;
	}
}

