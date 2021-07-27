#include "includes.h"

/*
** 采样基准电压 5V
*/
static const TEMP_RELATION tempTabel[] = {
	{-20,214},	
	{-19,224},
	{-18,236},	
	{-17,248},
	{-16,260},	
	{-15,273},
	{-14,288},	
	{-13,303},
	{-12,319},	
	{-11,336},
	{-10,353},	
	{-9,371},
	{-8,390},	
	{-7,409},
	{-6,430},	
	{-5,450},
	{-4,472},
	{-3,494},
	{-2,518},	
	{-1,542},
	{0,567},
	{1,592},
	{2,619},
	{3,647},
	{4,675},
	{5,705},
	{6,735},
	{7,766},
	{8,797},
	{9,830},
	{10,864},
	{11,899},
	{12,936},
	{13,973},
	{14,1011},	
	{15,1050},
	{16,1089},
	{17,1129},
	{18,1169},
	{19,1211},
	{20,1253},
	{21,1196},
	{22,1340},
	{23,1384},
	{24,1429},
	{25,1475},
	{26,1521},
	{27,1568},
	{28,1615},
	{29,1663},
	{30,1711},	
	{31,1759},
	{32,1809},
	{33,1858},
	{34,1908},
	{35,1959},
	{36,2009},
	{37,2060},
	{38,2111},
	{39,2163},
	{40,2214},
	{41,2266},
	{42,2317},
	{43,2369},
	{44,2421},
	{45,2472},
	{46,2523},	
	{47,2573},
	{48,2623},
	{49,2673},
	{50,2723},
	{51,2772},
	{52,2820},
	{53,2869},
	{54,2916},
	{55,2964},	
	{56,3012},
	{57,3060},	
	{58,3107},
	{59,3154},	
	{60,3201},	
	{61,3243},
	{62,3284},	
	{63,3324},
	{64,3364},	
	{65,3403},	
	{66,3444},
	{67,3484},	
	{68,3523},
	{69,3561},	
	{70,3599},	
	{71,3636},
	{72,3673},
	{73,3709},
	{74,3744},
	{75,3778},
	{76,3812},
	{77,3845},
	{78,3877},
	{79,3908},
	{80,3939},
	{81,3969},
	{82,3999},
	{83,4028},
	{84,4056},
	{85,4084},
	{86,4111},
	{87,4137},
	{88,4163},
	{89,4188},
	{90,4213},
	{91,4236},
	{92,4259},
	{93,4282},
	{94,4304},
	{95,4326},
	{96,4347},
	{97,4367},
	{98,4387},
	{99,4407},
	{100,4426},
};

int8 Temp_CalTempByAd(uint16 adVal)
{
	uint8 i=0;
	int8 temp = 0;
	for(i=0;i<sizeof(tempTabel)/sizeof(TEMP_RELATION);i++)
	{
		if(adVal<tempTabel[i].AD)
		{
			break;
		}
	}
	if(i==0)
	{
		temp = -20;
	}
	else if(i >= sizeof(tempTabel)/sizeof(TEMP_RELATION))
	{
		temp = 70;
	}
	else
	{
		temp = tempTabel[i-1].temp;
	}
	return(temp);
}

bool CC_Control_Fan(uint16 base,uint16 diff,int16* cur){
	static bool ret = false;
	uint16 adTemp = 0;
	uint16 adTemp2 = 0;
	int16 temp=0;/*目前暂时处理-20至100度*/
	int16 temp2 = 0;
	int16 tempValue = 0;
	static uint32 itick=0;
	
	adTemp = ADC_GetADResultAD(Temper1_ADResult);
	temp = Temp_CalTempByAd((uint16)adTemp);
	adTemp2 = ADC_GetADResultAD(Temper2_ADResult);
	temp2 = Temp_CalTempByAd((uint16)adTemp2);
	if((temp>=base || temp2 >= base)&& TickOut((uint32 *)itick,1015))
	{
		ret = true;
	}
	else if((temp <= (base-diff))&&(temp2 <= (base-diff)))
	{
		TickOut((uint32 *)&itick,0x00);
		ret = false;
	}
	else
	{
		TickOut((uint32 *)&itick,0x00);
	}
	/*
	**温度当前值
	*/
	tempValue = temp > temp2?temp:temp2;
	tempValue *= 10;
	memcpy((int8*)&cur[0],(int8*)&tempValue,sizeof(int16));
	return ret;
}

