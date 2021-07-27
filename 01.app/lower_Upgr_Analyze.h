#ifndef __LOWER_UPGR_ANALYZE_H_
#define __LOWER_UPGR_ANALYZE_H_

typedef struct{
	uint8 funId;/*功能函数ID*/
	void (*handle)(uint8,uint8*,uint8)/*1.数据长度 2.数据项 3.设备地址*/;
}LowerUpgrFunTable;

/*
** 下层协议之数据解析之任务函数处理
*/
void lowerUpgrFunAnalyze(void);

#endif

