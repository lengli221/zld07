#include "includes.h"
uint8   uart3TxBuffer[UART3_BUFLEN_S];
uint8   uart3RxBuffer[UART3_BUFLEN_R];
static uint8 uart3DMABuffer[DMA3_BUFLEN];

bool uart3Busy = false;
DECLARE_QUEUE(uart3TxQueue,uart3TxBuffer, sizeof(uart3TxBuffer));
DECLARE_QUEUE(uart3RxQueue,uart3RxBuffer, sizeof(uart3RxBuffer));
const UartInfo uart3Info={
    &uart3Busy,
    &uart3TxQueue,
    &uart3RxQueue
};

void Usart3_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);		
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void Usart3_DmaConfig(void){
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Stream3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART3_DR_ADDRESS;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32)uart3DMABuffer;
    DMA_InitStructure.DMA_BufferSize = DMA_BUFLEN;
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);
    
    DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);
    DMA_ITConfig(DMA1_Stream3, DMA_IT_HT, DISABLE); 
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  
}

void Usart3_DmaSend(void){
	uint32 bufferCount = 0;
	while((bufferCount<DMA_BUFLEN )&&(!QueueIsEmpty(uart3Info.TxQueue))){
		Dequeue(uart3Info.TxQueue,&uart3DMABuffer[bufferCount++]); 
	}
	DMA1_Stream3->NDTR = bufferCount;              
	DMA1_Stream3->M0AR = (uint32)uart3DMABuffer;    
	DMA_ITConfig(DMA1_Stream3, DMA_IT_TC, ENABLE);    
	USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);    
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  
	DMA_Cmd(DMA1_Stream3,ENABLE);	                  
}

bool Usart3_DmaWrite(uint8 * txBuf,uint32 txNum){
	uint32 timeOutCount = 4000;
	if( (txNum == 1) || (txNum > 256)){
		return false;
	}
	else{
		while(timeOutCount--){
			if(!(*uart3Info.Busy)){
				*uart3Info.Busy = true;
				FlushQueue(uart3Info.TxQueue);
				FlushQueue(uart3Info.RxQueue);
				while(txNum>0){
					if(Enqueue(uart3Info.TxQueue, *txBuf++))
						txNum--;
					else
						break;
				}
				Usart3_DmaSend();
				return true;
			}else{
				timeOutCount--;
				if(timeOutCount == 0){
					*uart3Info.Busy = false;
					FlushQueue(uart3Info.TxQueue);
					return false;
				}
			}
		}
	}
	return true;
}

void DMA1_Stream3_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_FEIF3|DMA_FLAG_DMEIF3|DMA_FLAG_TEIF3|DMA_FLAG_HTIF3);
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)){
		DMA_Cmd(DMA1_Stream3,DISABLE);
		USART_ITConfig(USART3,USART_IT_TC,ENABLE);
		DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);
	}
}

void Usart3_BufferInit(void){
	FlushQueue(uart3Info.TxQueue);
	FlushQueue(uart3Info.RxQueue);
	*uart3Info.Busy = false;
	memset(uart3TxBuffer, 0, sizeof(uart3TxBuffer));  
	memset(uart3RxBuffer, 0, sizeof(uart3RxBuffer)); 
	memset(uart3DMABuffer, 0, sizeof(uart3DMABuffer)); 
}

int Usart3_Rcv(uint8 *rxBuf, int rxNum){
	int32 iTempRxNum = 0;
	if(rxNum == 0)
		return -1;
	if(QueueIsEmpty(uart3Info.RxQueue))
		return -1;
	while(rxNum >0 && (!QueueIsEmpty(uart3Info.RxQueue))){
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		Dequeue(uart3Info.RxQueue, rxBuf++);
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		rxNum--;
		iTempRxNum++;
	};
	return iTempRxNum;
}

void USART3_IRQHandler(void){
	uint8 dat;
	
	if(USART_GetFlagStatus(USART3,USART_FLAG_FE|USART_FLAG_NE|USART_FLAG_ORE)){
		USART_ReceiveData(USART3); 
	}
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET) {
		USART_ITConfig(USART3,USART_IT_RXNE,DISABLE); 
		dat = USART_ReceiveData(USART3);
		Enqueue(uart3Info.RxQueue,dat);
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	}
	
	if(USART_GetITStatus(USART3,USART_IT_TC)!=RESET){
		USART_ITConfig(USART3,USART_IT_TC,DISABLE); 
		USART_ITConfig(USART3,USART_IT_RXNE,ENABLE); 
		USART_ClearITPendingBit(USART3, USART_IT_TC);
		*uart3Info.Busy = false;
	}
}

void Usart3_Stop(void){
	USART_ITConfig(USART3, USART_IT_TC, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART3, DISABLE);   
}

