#ifndef __TEMP_H_
#define __TEMP_H_

typedef struct
{
	int8 temp;
	int16 AD;
}TEMP_RELATION;
int8 Temp_CalTempByAd(uint16 adVal);
bool CC_Control_Fan(uint16 base,uint16 diff,int16* cur);
#endif

