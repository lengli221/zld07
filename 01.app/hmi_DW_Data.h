#ifndef __HMI_DW_DATA_H_
#define __HMI_DW_DATA_H_

#define HMI_DW_PAGE_MAIN									(uint8)0x00/*主页面*/
#define HMI_DW_PAGE_BatInfo								(uint8)0x01/*电池信息页面*/
#define HMI_DW_PAGE_Waring									(uint8)0x02/*一级报警页面*/
#define HMI_DW_PAGE_SysInfo								(uint8)0x03/*系统信息*/
#define HMI_DW_PAGE_CtrUpgr								(uint8)0x04/*控制板升级页面*/
#define HMI_DW_PAGE_InputPw								(uint8)0x06/*输入管理员密码*/
#define HMI_DW_PAGE_PwNoCorrect							(uint8)0x08/*密码不正确*/
#define HMI_DW_PAGE_WifiNamePw										(uint8)0x11/*WIFI 用户名和密码*/


typedef union{
	uint8 flag;
	struct{
		uint8 hv:1;/*过压标志*/
		uint8 lv:1;/*欠压标志*/
		uint8 oc:1;/*过流标志*/
		uint8 res:5;/*保留*/
	}bits;
}Hmi_PhaseVC;


typedef struct
{
	uint8 id;
	uint8 state;
	void (*handle)(void);
}HMI_DW_PAGE;

/*
** 页面切换接口函数
*/
void HMI_DW_PageDataTableFun(void);
/*
** 页面操作任务初始化
*/
void HMI_Init(void);

#endif

