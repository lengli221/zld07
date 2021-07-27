#include "includes.h"

/* Table of CRC values for high–order byte */
static uint8 auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
} ;

/* Table of CRC values for low–order byte */
static char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
} ;

uint16 CRC16(uint8 *puchMsg, uint16 usDataLen)
{
	uint8 uchCRCHi = 0xFF ; 
	uint8 uchCRCLo = 0xFF ; 
	uint8 uIndex  = 0; 
	while (usDataLen--) 
	{
		uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;
}

/*
** 非连续数据的CRC16校验
*/
uint16 CRC16_Upgrade(uint8 *puchMsg, uint16 usDataLen,uint16 uchInit,uint8* indexInit){
	uint8 uchCRCHi = (uint8)((uchInit>>8)&0xFF); 
	uint8 uchCRCLo = (uint8)(uchInit&0xFF); 
	uint8* uIndex  = indexInit; 
	
	while (usDataLen--) 
	{
		*uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[*uIndex] ;
		uchCRCLo = auchCRCLo[*uIndex] ;
	}
	return (uchCRCHi << 8 | uchCRCLo) ;	
}


uint8 CalcBcc(uint8 *buf,uint16 len)
{
	uint8 sum=0;
	uint8 *tx = buf;
	
	while(len--)
	{
		sum ^=*tx++;
	}
	return(sum);
}

uint8 calcSum(uint8 *buf,uint16 len)
{//和校验
	uint8 sum = 0xA5;
	while(len--)
	{
		sum += *buf++;
	}
	return sum;
}

int32 MSToTicks(uint32 ms)
{
	uint32 ticks = 0; 
	if(ms != 0)
	{
		ticks = (ms * OS_TICKS_PER_SEC)/1000;
		if(ticks < 1)
			ticks = 1;
	}
	return ticks;
}

bool TickOut(uint32 *tick,uint32 timeout)
{
	uint32 icount = 0;

	if(timeout == 0)
	{
		*tick = OSTimeGet();
		return true;
	}
	else
	{
		icount = MSToTicks(timeout);  
		if((OSTimeGet() - *tick) > icount)
			return true;
		else
			return false;
	}
}

void Sleep(uint32 milliSec)
{
	if(milliSec == 0)
	{
		return;
	}
	OSTimeDlyHMSM(0,0,(milliSec/1000),(milliSec%1000)); 
}

void uint16Tou16Ascii(uint16 tmp,uint16 *u16Ascii)
{
	u16Ascii[0] = (((tmp/1000 + '0')<<8)&0xFF00) + (tmp/100%10 + '0');
	u16Ascii[1] = (((tmp/10%10 + '0')<<8)&0xFF00) + (tmp%10 + '0'); 
}

bool Compare_Data(uint8*Src,uint8*Dst,uint16 len)
{
	uint16 i=0;
	bool ret = true;

	for(i=0;i<len;i++)
	{
		if(*Src++ != *Dst++)
		{
			ret = false;
			break;
		}
	}
	return(ret);
}

void  Uint16Reverse(uint16 *buf, uint8 len)
{
	uint8 i;
	for(i = 0; i < len; i++)
	{
		*buf = ((*buf)>>8)+((*buf)<<8);
		buf++;
	}
}

/*
** 换行符 0x0D 0x0A
*/
uint8 lineBreak(uint8* space){
	uint8 line[] = {0x0D,0x0A};

	memcpy((uint8*)&space[0],(uint8*)&line[0],sizeof(uint16));

	return(sizeof(uint16));
}

/*
** 整数<-->字符串
** @param: num->整型值 str->转换后字符串 radix->进制
** @return: 字符串字符个数
*/
uint8 itoa(int num,char* str,int radix){
	/*
	** 索引表
	*/
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	/*
	** 存放要转换的整数的绝对值,转换的整数可能是负数
	*/
	unsigned unum;
	/*
	** i用来指示设置字符串相应位,转换之后i其实就是字符串的长度;转换后顺序是逆序的;有正负的情况;
	** k用来指示调整顺序的开始位置;
	** j用来指示调整顺序时的交换
	*/
	int i=0,j,k;
	/*
	** 临时变量
	*/
	char temp;

	/*
	** 获取要转换的整数的绝对值
	*/
	if(radix == 10 && num < 0){
		unum = (unsigned)-num;
		str[i++] = '-';
	}else{
		unum = (unsigned)num;
	}


	do{
		str[i++] = index[unum%(unsigned)radix];
		unum /= radix;
	}while(unum);

	/*
	** 在字符串最后添加'\0'字符,c语言字符串以'\0'结束
	*/
	str[i]='\0';

	/*
	** 顺序调整
	*/
	if(str[0] == '-'){
		k = 1;
	}else{
		k = 0;
	}

	/*
	** 头尾一一对称交换
	*/
	for(j = k; j < i/2;j++){
		temp = str[j];
		str[j] = str[i-1+k-j];
		str[i-1+k-j] = temp;
	}

	return i;
}

/*
** 指定(进制:10)整数类型-->字符串
** @param: num->整型值 str->转换后字符串
** @return: 字符串字符个数
*/
uint8 _itoa(int num,char* str){
	return(itoa(num, str, 10));
}

/*
** 整型字符-->字符串 添加倍率参数
** @param: num->整型值 str->转换后字符串 rate-->倍率
*/
uint8 strAddRate(int num,char* str,uint32 rate){
	int8 index = -1;
	uint8 strnum = 0;
	char strTemp[3] = {0};
	char temp[8] = {0};
	uint32 numTemp = 0;

	memset((uint8*)&temp[0],'0',8);

	do{
		rate /=10; 
		index++;
	}while(rate);

	numTemp = (uint32)num;
	if(num < 0){/*将负数转换成正数处理*/
		
		numTemp = ~numTemp + 1;
	}

	strnum = itoa(numTemp, (char *)str, 10);

	if(strnum <= index){/*字符串装换值小于倍率*/
		memcpy((uint8*)&temp[index+1-strnum],(uint8*)str,strnum);
		memcpy((uint8*)str,(uint8*)&temp[0],index+1);
		strnum = index + 1;/*为增加.之前0提供数据空间*/
	}

	/*插入.空间*/
	memcpy((uint8*)&strTemp[0],(uint8*)(str+(strnum-index)),index);
	str[strnum - index] = '.';
	memcpy((uint8*)(str+(strnum-index)+1),(uint8*)&strTemp[0],index);

	/*负数增加'-'*/
	if(num < 0){
		memcpy((uint8*)&temp[0],(uint8*)&str[0],strnum + 1);
		str[0] = '-';
		memcpy((uint8*)&str[1],(uint8*)&temp[0],strnum+1);
		strnum++;
	}

	return (strnum + 1);
}

/*
** BCD转uint32
*/
#define   BCDToHex(b)       		((((b) & 0xF0) >> 4) * 10 + ((b) & 0x0F))
uint32 bcdToUInt(const uint8 *byBCDCode, uint16 iBCDCodeLen){
	uint32 iIntFromBCD = 0x00;
	uint16 i = 0;
	int val = 1;
	
	for (i = 0; i < iBCDCodeLen; i++)
	{
		iIntFromBCD += (uint16)(BCDToHex(*(byBCDCode + i)) * val);
		val *= 100;
	}
	return iIntFromBCD;
}


