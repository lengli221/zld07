#ifndef __COMMON_H_
#define __COMMON_H_

uint16 CRC16(uint8 *puchMsg, uint16 usDataLen);
/*
** 非连续数据的CRC16校验
*/
uint16 CRC16_Upgrade(uint8 *puchMsg, uint16 usDataLen,uint16 uchInit,uint8* indexInit);
bool TickOut(uint32 *tick,uint32 timeout);
void Sleep(uint32 milliSec);
void uint16Tou16Ascii(uint16 tmp,uint16 *u16Ascii);
bool Compare_Data(uint8*Src,uint8*Dst,uint16 len);
void  Uint16Reverse(uint16 *buf, uint8 len);
uint8 CalcBcc(uint8 *buf,uint16 len);
uint8 calcSum(uint8 *buf,uint16 len);
/*
** 换行符 0x0D 0x0A
*/
uint8 lineBreak(uint8* space);
/*
** 整数<-->字符串
** @param: num->整型值 str->转换后字符串 radix->进制
** @return: 字符串字符个数
*/
uint8 itoa(int num,char* str,int radix);
/*
** 指定(进制:10)整数类型-->字符串
** @param: num->整型值 str->转换后字符串
** @return: 字符串字符个数
*/
uint8 _itoa(int num,char* str);
/*
** 整型字符-->字符串 添加倍率参数
** @param: num->整型值 str->转换后字符串 rate-->倍率
*/
uint8 strAddRate(int num,char* str,uint32 rate);
/*
** BCD转uint32
*/
uint32 bcdToUInt(const uint8 *byBCDCode, uint16 iBCDCodeLen);
#endif

