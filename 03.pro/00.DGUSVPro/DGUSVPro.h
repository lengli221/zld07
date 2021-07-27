#ifndef __DGUSVPRO_H_
#define __DGUSVPRO_H_

/*
** 协议参数定义
*/
#define D_DP_FrameStart1				0x5A		                   		
#define D_DP_FrameStart2				0xA5		                   		

/*
** 功能参数说明
** 1.指定地址写寄存器数据
** 2.指定地址读寄存器数据
** 3.指定地址开始写入数据串到变量存储区
** 4.从变量存储区指定地址开始读入RD_LEN长度数据
** 5.写曲线缓冲区数据
*/
#define D_DP_CtrlWrite					0x80	                     		
#define D_DP_CtrlRead					0x81		                    		
#define D_DP_DataWirte					0x82	                     		
#define D_DP_DataRead					0x83		                    		
#define D_DP_CurveWirte				0x84	                     	

typedef struct __DP_FramePara__
{
	uint16 iAddress;        	 										
	uint8 ucFunCode;         										
	uint8 ucRegLen;         										  	
	uint8 ucDataSpace[256];  									
} DP_FramePara;

bool  DP_PackageFrame(void *pPara, void *pBuf1, uint16 *ucDatalen);
bool  DP_ParseFrame(void *pPara, void *pBuf1, uint16 ucDatalen);

#endif

