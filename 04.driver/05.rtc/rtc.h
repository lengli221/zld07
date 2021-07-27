#ifndef RTCTIME_H
#define RTCTIME_H

#include "stm32f2xx.h"
#include "stm32f2xx_conf.h"
#include "stdio.h"
#include "type.h"

/* 
** TIME STRUCT 
*/
typedef  struct _TIME_INFO_{
    uint32 iYear;
    uint8  ucMonth;
    uint8  ucDay;
    uint8  ucWeek;
    uint8  uchour;
    uint8  ucMin;
    uint8  ucSec;
}Time_Info;  

/* 
** 时间结构定义 
*/
typedef struct _COMMON_TIME_{
	uint16 iYear;     									
	uint8 ucMonth;    								
	uint8 ucDay;      									
	uint8 ucHour;     								
	uint8 ucMin;      									
	uint8 ucSec;      									
}Common_Time;


/*
** RTC 时钟配置
*/
void RTC_Config(void);
/*
** 时钟初始化
*/
void rtc_Init(void);
/*
** 设置时间
*/
void RTC_Set_time(Common_Time st_Time);
/*
** 获取当前时间
*/
void GetCurrentTime(Common_Time *curTime);
/*
** 时间转换成秒
*/
uint32 TimeToSeconds(Common_Time cdzTime);
/*
** 获取当前秒数,从2000年起
*/
uint32 GetCurSecond(void);
/*
** 获取系统滴答时钟节拍
*/
uint32 GetSysTick(void);

#endif

