#ifndef __USART_H_
#define __USART_H_

/*
** 串口1 缓存参数
*/
#define UART1_BUFLEN_R								sizeof(uint16)*(1024+sizeof(UpgradeProFrame))
#define UART1_BUFLEN_S								256
#define DMA1_BUFLEN         								256  

/*
** 串口2 缓存参数
*/
#define UART_BUFLEN_R								sizeof(uint16)*(1024+sizeof(UpgradeProFrame))/*1024*/
#define UART_BUFLEN_S								256/*1024*/
#define DMA_BUFLEN         							256/*1024*/      

/*
** 串口3 缓存参数
*/
#define UART3_BUFLEN_R							256
#define UART3_BUFLEN_S							256
#define DMA3_BUFLEN								256

#define USART1_DR_ADDRESS                 			((uint32)USART1 + 0x04)
#define USART2_DR_ADDRESS                 			((uint32)USART2 + 0x04)
#define USART3_DR_ADDRESS               				((uint32)USART3 + 0x04)
#define UART4Tx_DR_ADDRESS                			((uint32)UART4 + 0x04)

#define BSP_ComType_USART             				(uint8)0x01

#define BSP_ComUpperLayer_232					(uint8)0x01
#define BSP_ComUpperLayer						(uint8)0x02
#define BSP_ComDW_Usart							(uint8)0x03

#define BSP_COM_NO									0			
#define BSP_COM_EVEN								1			
#define BSP_COM_ODD								2			

#define BSP_COM_STOP1							1
#define BSP_COM_STOP2							2

#define UpperLayer_Rx       							(GPIOG->BSRRH = GPIO_Pin_14)
#define UpperLayer_Tx       							(GPIOG->BSRRL = GPIO_Pin_14)
#define LowerLayer_Rx       							(GPIOC->BSRRH = GPIO_Pin_8)
#define LowerLayer_Tx       							(GPIOC->BSRRL = GPIO_Pin_8)

typedef struct {
  bool  *  Busy;    
  Queue *  TxQueue;   
  Queue *  RxQueue;   
}UartInfo;

void com_write(uint8 *buf, uint16 len);
int16 com_read(uint8 *buf, int16 len);
bool com_open(int8 port, int32 baud_rate, int32 dataBit, int8 parity, int8 stopBit);
void com_init(void);
void Com_Init(void);
void BSP_SmSend(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen);
void BSP_SmRecv(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen);
/*---------------------------------串口1函数调用----------------------------------------------------------------*/
/*
** 串口初始化
*/
void Usart1_Init(void);
/*
** DMA 配置
*/
void Usart1_DmaConfig(void);
/*
** 初始化缓冲区
*/
void Usart1_BufferInit(void);
/*
** 从缓冲区读数据
*/
int Usart1_Rcv(uint8 *rxBuf, int rxNum);
/*
** DMA Write
*/
bool Usart1_DmaWrite(uint8 * txBuf,uint32 txNum);
/*--------------------------------------------------------------------------------------------------------------*/
void Usart2_Init(void);
void Usart2_DmaConfig(void);
void Usart2_BufferInit(void);
int Usart2_Rcv(uint8 *rxBuf, int rxNum);
bool Usart2_DmaWrite(uint8 * txBuf,uint32 txNum);
void Usart3_Init(void);
void Usart3_DmaConfig(void);
void Usart3_BufferInit(void);
int Usart3_Rcv(uint8 *rxBuf, int rxNum);
bool Usart3_DmaWrite(uint8 * txBuf,uint32 txNum);
#endif

