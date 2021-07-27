#include "includes.h"

uint8 uart2TxBuffer[UART_BUFLEN_S];
uint8 uart2RxBuffer[UART_BUFLEN_R];
static uint8 uart2DMABuffer[DMA_BUFLEN];

DECLARE_QUEUE(uart2TxQueue,uart2TxBuffer, sizeof(uart2TxBuffer));
DECLARE_QUEUE(uart2RxQueue,uart2RxBuffer, sizeof(uart2RxBuffer));

bool uart2Busy = false;
const UartInfo uart2Info = {
    &uart2Busy,
    &uart2TxQueue,
    &uart2RxQueue
};

void Usart2_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	UpperLayer_Rx;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void Usart2_DmaConfig(void){/*DMA*/
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Stream6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = USART2_DR_ADDRESS;
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
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32)uart2DMABuffer;
	DMA_InitStructure.DMA_BufferSize = DMA_BUFLEN;
	DMA_Init(DMA1_Stream6, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Stream6, DMA_IT_HT, DISABLE);
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);   
}

void Usart2_DmaSend(void){/*DMA发送*/
	uint32 bufferCount = 0;
	while((bufferCount<DMA_BUFLEN )&&(!QueueIsEmpty(uart2Info.TxQueue)))
	{
		Dequeue(uart2Info.TxQueue,&uart2DMABuffer[bufferCount++]);/*出列*/
	}
	DMA1_Stream6->NDTR = bufferCount;/*缓冲大小*/
	DMA1_Stream6->M0AR = (uint32)uart2DMABuffer;    
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE);    
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);  
	DMA_Cmd(DMA1_Stream6,ENABLE);	               
}

bool Usart2_DmaWrite(uint8 * txBuf,uint32 txNum){
	uint32 timeOutCount = 4000;
	if( (txNum == 1) || (txNum > 256) || (txNum == 0)){
		return false;
	}
	else
	{
		while(timeOutCount--){
			if(!(*uart2Info.Busy)){/*端口*/
				*uart2Info.Busy = true;
				UpperLayer_Tx;
				FlushQueue(uart2Info.TxQueue);
				/*FlushQueue(uart2Info.RxQueue);*/
				while(txNum>0){/*全部入列*/
					if(Enqueue(uart2Info.TxQueue, *txBuf++))
						txNum--;
					else
						break;
				}
				Usart2_DmaSend();
				while(*uart2Info.Busy){
					Sleep(1);
				}
				return true;
			}else{
				timeOutCount--;
				if(timeOutCount == 0)
				{
					*uart2Info.Busy = false;
					FlushQueue(uart2Info.TxQueue);
					return false;
				}
			}
		}
	}
	return true;
}

void DMA1_Stream6_IRQHandler(void){
	DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_FEIF6|DMA_FLAG_DMEIF6|DMA_FLAG_TEIF6|DMA_FLAG_HTIF6);
	if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6)){
		DMA_Cmd(DMA1_Stream6,DISABLE);	/*关闭DMA*/
		USART_ITConfig(USART2,USART_IT_TC,ENABLE);/*开发送完成中断*/
		DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,DISABLE);
		DMA_ClearITPendingBit(DMA1_Stream6,DMA_IT_TCIF6);
	}
}

void Usart2_BufferInit(void){
	FlushQueue(uart2Info.TxQueue);
	FlushQueue(uart2Info.RxQueue);
	*uart2Info.Busy = false;
	memset(uart2TxBuffer,0, sizeof(uart2TxBuffer));
	memset(uart2RxBuffer,0, sizeof(uart2RxBuffer));
	memset(uart2DMABuffer,0, sizeof(uart2DMABuffer));
}

int Usart2_Rcv(uint8 *rxBuf, int rxNum) 
{
	int32 iTempRxNum = 0;
	if(rxNum == 0)
		return -1;
	if(QueueIsEmpty(uart2Info.RxQueue))
		return -1;

	while(rxNum >0 && (!QueueIsEmpty(uart2Info.RxQueue))){
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		Dequeue(uart2Info.RxQueue, rxBuf++);
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		rxNum--;
		iTempRxNum++;
	};
	return iTempRxNum;
}	

void USART2_IRQHandler(void)
{
	uint8 dat;
	if(USART_GetFlagStatus(USART2,USART_FLAG_PE | USART_FLAG_FE|USART_FLAG_NE|USART_FLAG_ORE)){
		USART_ReceiveData(USART2);/*将错误数据读取出来*/
	}
	
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)/*接收到数据*/
	{
		USART_ITConfig(USART2,USART_IT_RXNE,DISABLE); 
		dat = USART_ReceiveData(USART2);
		Enqueue(uart2Info.RxQueue,dat);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	}
	
	if(USART_GetITStatus(USART2,USART_IT_TC)!=RESET){
		USART_ITConfig(USART2,USART_IT_TC,DISABLE); 
		UpperLayer_Rx;
		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE); 
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		*uart2Info.Busy = false;
	}
}

void Usart2_Stop(void){
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);   
}

