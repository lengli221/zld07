#include "includes.h"

extern LCD_CTRLFRAME LCD_CtrlAddrSheets[];
extern uint8 const  LCD_CtrlAddrSheetsNum;
extern LCD_DATAFRAME LCD_DataAddrSheets[];
extern uint16 const LCD_DataAddrSheetsNum;


/*
** 迪文屏之从屏幕获取参数
*/
static LcdGetData lcdGetDataTable[] = {
	{LCD_ADDR_DomainNameLocal,getLocalDomainInput},
	{LCD_ADDR_DomainNameRemote,getRemoteDomainInput},
	{LCD_ADDR_DeviceID,get_FromDeinDeviceId},
	{LCD_Addr_WIFIName,get_FromDwinWifiName},
	{LCD_Addr_WIFIPw,get_FromDwinWifiPw},
};
static uint8 lcdGetDataTableNum = sizeof(lcdGetDataTable)/sizeof(LcdGetData);

/*
** 迪文屏交互之下传参数
*/
bool LCD_SetData(void *pData,void *temp){
	uint16 i;
	uint8 j;
	uint16 Vual=0 ;
	DP_FramePara *st_FramePara = (DP_FramePara *)pData;

	if (st_FramePara == null) 
		return false;
	
	if((st_FramePara->ucFunCode == D_DP_CtrlRead)||(st_FramePara->ucFunCode == D_DP_CtrlWrite)){
		for (i = 0; i < LCD_CtrlAddrSheetsNum; i++){
			if(st_FramePara->iAddress == LCD_CtrlAddrSheets[i].addr){/*查找对应寄存器地址*/
				if((LCD_CtrlAddrSheetsNum - i) < (st_FramePara->ucRegLen))/*防止指针越界*/
					return false;
				
				for (j = 0; j < (st_FramePara->ucRegLen); j++){
					*(LCD_CtrlAddrSheets[i + j].pData) = st_FramePara->ucDataSpace[j];
				}
				break;
			}
		}
	}else{
		if(st_FramePara->iAddress < 0x3000){
			for (i = 0; i < LCD_DataAddrSheetsNum; i++){
				if(st_FramePara->iAddress == LCD_DataAddrSheets[i].addr){/*查找对应寄存器地址*/
					if((LCD_DataAddrSheetsNum - i) < (st_FramePara->ucRegLen))/*防止指针越界*/ 
						return false;
					
					for (j = 0; j < (st_FramePara->ucRegLen); j++){
						Vual=st_FramePara->ucDataSpace[j * 2];
						Vual=Vual<<8;
						Vual=Vual+st_FramePara->ucDataSpace[j *2+1];
						*(LCD_DataAddrSheets[i + j].pData)=Vual;
					}		
					break;
				}
			}
		}else{
			for(i=0;i<lcdGetDataTableNum;i++){
				if(st_FramePara->iAddress == lcdGetDataTable[i].addr){
					lcdGetDataTable[i].handle(pData,temp);
					break;
				}
			}
		}
	}
	return true;
}

/*
** 迪文屏交互之上传参数
*/
bool LCD_GetData(void *pData)
{
	uint16 i; 
	uint8 j;
	DP_FramePara *st_FramePara = (DP_FramePara *)pData;
	
	if (pData == null) 
		return false;
	
	/*(D_DP_CtrlRead)0x81指定地址读RD_LEN字节寄存器数据 (D_DP_CtrlWrite)0x80指定地址写寄存器数据*/
	if((st_FramePara->ucFunCode == D_DP_CtrlRead)||(st_FramePara->ucFunCode == D_DP_CtrlWrite))/*指定地址数据以字节为单位*/{
		for(i = 0; i < LCD_CtrlAddrSheetsNum; i++){
			if(st_FramePara->iAddress == LCD_CtrlAddrSheets[i].addr){
				if((LCD_CtrlAddrSheetsNum - i) < (st_FramePara->ucRegLen)) 
					return false;
				for(j = 0; j < (st_FramePara->ucRegLen); j++){
					st_FramePara->ucDataSpace[j] = *(LCD_CtrlAddrSheets[i + j].pData) & 0xff;
				}
				break;
			}
		}
	}else{/*(D_DP_DataWirte)0x82指定地址开始写入数据串(字数据)到变量存储区*/
		for(i = 0; i < LCD_DataAddrSheetsNum; i++){/*说明:查表,查询表格地址是否匹配,找到匹配地址*/
			if(st_FramePara->iAddress == LCD_DataAddrSheets[i].addr){/*判断查询到匹配地址之后的数据是否大于需要写入屏数据,否则直接返回*/
				if((LCD_DataAddrSheetsNum - i) < (st_FramePara->ucRegLen)) 
					return false;
				for(j = 0; j < (st_FramePara->ucRegLen); j++){
					st_FramePara->ucDataSpace[j * 2] = *(LCD_DataAddrSheets[i + j].pData) >> 8;
					st_FramePara->ucDataSpace[j * 2 + 1] = *(LCD_DataAddrSheets[i + j].pData) & 0xff;
				}
				break;
			}
		}
	}
	return true;
}

