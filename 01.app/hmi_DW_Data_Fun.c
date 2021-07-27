#include "includes.h"

/*
** 使用externd调用文件"hmi_DW_Data_Fun.c"变量
*/
extern HMI_DW_PAGE thispage;
extern const HMI_DW_PAGE hmi_DW_PageTable[];
extern uint8 hmi_DW_PageTableNum;

/*
** 页面切换接口函数
*/
void HMI_DW_PageDataTableFun(void){
	uint8 pageId = 0;
	uint8 i=0;	
	
	if(LCD_GetScreenID() != thispage.id){
		pageId = LCD_GetScreenID();
		for(i=0;i<hmi_DW_PageTableNum;i++){
			if((pageId == hmi_DW_PageTable[i].id)&&(pageId != 0xFF)){
				memcpy((uint8*)&thispage.id,(uint8*)&hmi_DW_PageTable[i].id,sizeof(HMI_DW_PAGE));
				break;
			}
		}
	}
	thispage.handle();
}

