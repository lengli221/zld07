#include "includes.h"

static bool DP_PackageWriteControlFrame(DP_FramePara *pPara, uint8 *pBuf, uint16 *ucDatalen){/*指定地址写寄存器数据0x80*/
	uint8 iCount = 0;
	uint16 crc = 0;

	if ((pPara == null) || (pBuf == null) || (ucDatalen == null)) 
		return false;
	/*打包站号功能码*/
	pBuf[iCount++] = D_DP_FrameStart1;
	pBuf[iCount++] = D_DP_FrameStart2;
	pBuf[iCount++] = pPara->ucRegLen + 4;
	pBuf[iCount++] = D_DP_CtrlWrite;
	pBuf[iCount++] = pPara->iAddress;
	memcpy(pBuf + iCount, pPara->ucDataSpace, pPara->ucRegLen);
	iCount = iCount + pPara->ucRegLen;
	/*计算CRC*/
	crc = CRC16(pBuf+3, iCount-3);
	/*打包CRC*/
	pBuf[iCount++] = (crc >> 8) & 0xff;
	pBuf[iCount++] = crc & 0xff ;
	*ucDatalen = iCount;
	
	return true;
}

static bool DP_PackageReadControlFrame(DP_FramePara *pPara, uint8 *pBuf, uint16 *ucDatalen){/*指定地址读RD_LEN字节寄存器数据0x81*/
	/*打包读数据*/
	uint8 iCount = 0;
	uint16 crc = 0;
	
	if ((pPara == null) || (pBuf == null) || (ucDatalen == null)) 
		return false;

	/*打包站号功能码*/
	pBuf[iCount++] = D_DP_FrameStart1;
	pBuf[iCount++] = D_DP_FrameStart2;
	pBuf[iCount++] = 5;
	pBuf[iCount++] = D_DP_CtrlRead;
	pBuf[iCount++] = pPara->iAddress;
	pBuf[iCount++] = pPara->ucRegLen;
	
	crc = CRC16(pBuf+3, iCount-3);
	/*打包CRC*/
	pBuf[iCount++] = (crc >> 8) & 0xff;
	pBuf[iCount++] = crc & 0xff ;
	*ucDatalen = iCount;
	return true;
}

static bool DP_PackageWriteDataFrame(DP_FramePara *pPara, uint8 *pBuf, uint16 *ucDatalen){/*指定地址开始写入数据串(字数据)到变量存储器区0x82*/
	uint8 iCount = 0;
	uint16 crc = 0;
// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		uint16 tmpAddress = 0;
// 	#endif
	
	if ((pPara == null) || (pBuf == null) || (ucDatalen == null)) 
		return false;
	/*
	** 打包站号功能码
	*/
	pBuf[iCount++] = D_DP_FrameStart1;
	pBuf[iCount++] = D_DP_FrameStart2;
	pBuf[iCount++] = pPara->ucRegLen*2 + 5;
	pBuf[iCount++] = D_DP_DataWirte;
// 	#if DW_GENERATION_SCREEN == DW_ONE_SCREEN
		pBuf[iCount++] = pPara->iAddress>>8;
		pBuf[iCount++] = pPara->iAddress & 0xff;
		memcpy(pBuf + iCount, pPara->ucDataSpace, pPara->ucRegLen*2);
		iCount = iCount + pPara->ucRegLen*2;
// 	#elif DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		tmpAddress = pPara->iAddress;
// 		if(pPara->iAddress != 0x0084)
// 		{
// 			tmpAddress += 0x1000;
// 		}
// 		pBuf[iCount++] = tmpAddress>>8;
// 		pBuf[iCount++] = tmpAddress & 0xff;
// 		memcpy(pBuf + iCount, pPara->ucDataSpace, pPara->ucRegLen*2);
// 		iCount = iCount + pPara->ucRegLen*2;
// 	#endif
	/*计算CRC*/
	crc = CRC16(pBuf+3, iCount-3);
	/*打包CRC*/
	pBuf[iCount++] = (crc >> 8) & 0xff;
	pBuf[iCount++] = crc & 0xff ;
	*ucDatalen = iCount;
	
	return true;
}

static bool DP_PackageReadDataFrame(DP_FramePara *pPara, uint8 *pBuf, uint16 *ucDatalen){/*从变量存储区指定地址开始读入RD_LEN长度字数据0x83*/
	/*打包读数据*/
	uint8 iCount = 0;
	uint16 crc = 0;
// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		uint16 tmpAddress = 0;
// 	#endif
	
	if ((pPara == null) || (pBuf == null) || (ucDatalen == null)) 
		return false;
	/*打包站号功能码*/
	pBuf[iCount++] = D_DP_FrameStart1;
	pBuf[iCount++] = D_DP_FrameStart2;
	pBuf[iCount++] = 6;
	pBuf[iCount++] = D_DP_DataRead;
// 	#if DW_GENERATION_SCREEN == DW_ONE_SCREEN
		pBuf[iCount++] = pPara->iAddress >> 8;
		pBuf[iCount++] = pPara->iAddress & 0xff;
		pBuf[iCount++] = pPara->ucRegLen;
// 	#elif DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		tmpAddress = pPara->iAddress;
// 		if(tmpAddress != 0x0014){
// 			tmpAddress += 0x1000;
// 		}
// 		pBuf[iCount++] = tmpAddress >> 8;
// 		pBuf[iCount++] = tmpAddress & 0xff;
// 		pBuf[iCount++] = pPara->ucRegLen;
// 	#endif
	crc = CRC16(pBuf+3, iCount-3);
	/*打包CRC*/
	pBuf[iCount++] = (crc >> 8) & 0xff;
	pBuf[iCount++] = crc & 0xff ;
	*ucDatalen = iCount;
	
	return true;
}

bool  DP_PackageFrame(void *pPara, void *pBuf1, uint16 *ucDatalen)
{
	DP_FramePara *st_FramePara = (DP_FramePara *)(pPara);
	uint8 * pBuf = pBuf1;

	if ((pPara == null) || (pBuf == null) || (ucDatalen == null)) 
	return false;

	switch(st_FramePara->ucFunCode)
	{
		case D_DP_CtrlWrite:
			return DP_PackageWriteControlFrame(st_FramePara, pBuf, ucDatalen);
		case D_DP_CtrlRead:
			return DP_PackageReadControlFrame(st_FramePara, pBuf, ucDatalen);
		case D_DP_DataWirte:
			return DP_PackageWriteDataFrame(st_FramePara, pBuf, ucDatalen);
		case D_DP_DataRead:
			return DP_PackageReadDataFrame(st_FramePara,pBuf,ucDatalen);
		default:
			break;
	}
	return false;
}

bool  DP_ParseReadControlFrame(void *pPara, uint8 *pBuf, uint8 ucDatalen){/*读寄存器的DGUS屏应答0x81*/
	uint8 iCount = 0;
	int16 crc = 0;
	DP_FramePara *st_FramePara = (DP_FramePara *)pPara;

	if (pBuf[iCount++] != D_DP_FrameStart1|| pBuf[iCount++] != D_DP_FrameStart2)
		return  false;
	iCount++;
	if((pBuf[iCount++] != D_DP_CtrlRead) ||(pBuf[iCount++] != st_FramePara->iAddress))
	{
		return false;
	}
	if (pBuf[iCount++] != ((st_FramePara->ucRegLen) & 0xFF))
		return false;
	crc = CRC16(pBuf+3, (st_FramePara->ucRegLen + 3));
	if(((crc & 0xFF) != pBuf[st_FramePara->ucRegLen + 7]) ||
		(((crc >> 8) & 0xFF) != pBuf[st_FramePara->ucRegLen + 6]))
	return false;
	memcpy(st_FramePara->ucDataSpace, pBuf + 6, st_FramePara->ucRegLen);
	return true;
}

bool  DP_ParseReadDataFrame(void *pPara, uint8 *pBuf, uint8 ucDatalen){/*读数据存储器的DGUS屏应答0x83*/
	uint8 iCount = 0;
	int16 crc = 0;
// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		uint16 tmpAddress = 0;
// 	#endif	
	DP_FramePara *st_FramePara = (DP_FramePara *)pPara;
// 	volatile uint8 temp = 0;
	
	if (pBuf[iCount++] != D_DP_FrameStart1|| pBuf[iCount++] != D_DP_FrameStart2)
		return  false;
	iCount++;
	/*判断地址与功能码是否正确*/
	if((pBuf[iCount++] != D_DP_DataRead)){
		return false;
	}

// 	if(st_FramePara->iAddress == 0x0014){
// 		temp = 8;
// 	}
	
// 	#if DW_GENERATION_SCREEN == DW_ONE_SCREEN
		if (pBuf[iCount++] != ((st_FramePara->iAddress >> 8) & 0xFF) || pBuf[iCount++] != (st_FramePara ->iAddress & 0xFF))
			return false;
// 	#elif DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		tmpAddress = st_FramePara->iAddress;
// 		if(tmpAddress != 0x0014){
// 			tmpAddress += 0x1000;
// 		}
// 		if(pBuf[iCount++] != ((tmpAddress >> 8) & 0xFF) || pBuf[iCount++] != (tmpAddress & 0xFF))
// 			return false;
// 	#endif
	
	if (pBuf[iCount++] != ((st_FramePara->ucRegLen) & 0xFF))
			return false;
	if(iCount > ucDatalen){
		return false;
	}	

// 	#if  DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		DW_SecondScreenAddrAlteration(pPara, GetScreenCurId(), pBuf+iCount);
// 	#endif
	
	crc = CRC16(pBuf+3, (st_FramePara->ucRegLen*2 + 4));
	if(((crc & 0xFF) != pBuf[st_FramePara->ucRegLen*2 + 8]) ||
	(((crc >> 8) & 0xFF) != pBuf[st_FramePara->ucRegLen*2 + 7]))
		return false;

// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		if(st_FramePara->iAddress == 0x0014){
// 			st_FramePara->ucRegLen = 0x00;
// 			st_FramePara->iAddress = 0x00;
// 		}
// 	#endif
	
	/*解析数据区*/
	memcpy(st_FramePara->ucDataSpace, pBuf + 7, st_FramePara->ucRegLen*2);

// 	if(st_FramePara->iAddress == 0x0014){
// 		temp = 8;
// 	}
	
	return true;
}

bool  DP_ParseFrame(void *pPara, void *pBuf1, uint16 ucDatalen)
{
	DP_FramePara *st_FramePara = (DP_FramePara *)pPara;
	uint8 * pBuf = pBuf1;
// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		static uint32 timeout_LCD = 0;
// 	#endif
	
	if ((pPara == null) || (pBuf == null) || (ucDatalen == 0)) 
		return false;

// 	/*若返回帧功能码和询问时指令的不一致，判定失败*/
// 	#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN
// 		do{
// 			if(pBuf[3] == D_DP_DataWirte){
// 				pBuf += 8;/*解决0x82:变量空间地址,回复问题 理论上:SM_DisposeComUnit内存在判断条件直接返回,串口3采用232,接收队列在下一次接收解决数据问题*/
// 				ucDatalen -= 8;/*参数8来源分析:帧头(2Byte)+数据长度(1Byte)+指令(1字节)+数据(2Byte:0x4F 0x4B)+CRC校验(2Byte)*/
// 			}
// 			if(TickOut(&timeout_LCD,1000)){
// 				TickOut(&timeout_LCD,0x00);
// 				Sleep(40);
// 			}
// 		}while(pBuf[3] == D_DP_DataWirte);
// 		TickOut(&timeout_LCD,0x00);
// 	#endif	
	
	if (pBuf[3] != st_FramePara->ucFunCode) 
		return false;
	
	/*判断无误进入相关解析函数*/
	switch(st_FramePara->ucFunCode)
	{
		case D_DP_CtrlRead:
			return DP_ParseReadControlFrame(st_FramePara, pBuf, ucDatalen);
		case D_DP_DataRead:
			return DP_ParseReadDataFrame(st_FramePara, pBuf, ucDatalen);
		//#if DW_GENERATION_SCREEN == DW_SECOND_SCREEN	
		case D_DP_DataWirte:
			return true;
		//#endif
		default:
			break;
	}
	return false;
}

