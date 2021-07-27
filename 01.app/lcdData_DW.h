#ifndef __LCDDATA_DW_H_
#define __LCDDATA_DW_H_

/*
** II迪文屏页面控制
*/
#define LCD_ADDR_WriteId								(uint16)(0x0084)
#define LCD_ADDR_Len_WriteId						(uint8)(2)
#define LCD_ADDR_ReadId								(uint16)(0x0014)
#define LCD_ADDR_Len_ReadId						(uint8)(1)
#define LCD_ADDR_WriteVoice							(uint16)(0x00A0)
#define LCD_ADDR_Len_WriteVoice					(uint8)(2)


/*
** 注意事项:
**			1.避免二代屏地址冲突 底层已处理,地址确认值不同于上述两个具体地址即可
** 迪文地址:
**			1.II代迪文屏底层已处理好,偏移地址:0x1000
**			2.地址编写设置余量
*/
#define LCD_Addr_WriteMainState								(uint16)(0x1000)/*主页面状态地址*/
#define LCD_Addr_WriteMainGroup0Icon						(uint16)(0x2100)/*主页面图标+电压参数地址*/
#define LCD_Addr_WriteMainGroup1Icon						(uint16)(0x2200)
#define LCD_Addr_WriteMainGroup2Icon						(uint16)(0x2300)
#define LCD_Addr_WriteMainGroup3Icon						(uint16)(0x2400)
#define LCD_Addr_WriteMainGroup4Icon						(uint16)(0x2500)
#define LCD_Addr_WriteMainGroup5Icon						(uint16)(0x2600)
#define LCD_Addr_JumpBatInfo								(uint16)(0x1500)/*读/写电池信息跳转参数*/
#define LCD_Addr_JumpInputPW								(uint16)(0x1510)/*读/写跳转至密码输入页面*/
#define LCD_Addr_WriteBatInfoBaseAddr					(uint16)(0x1700)/*写电池页面信息*/
#define LCD_Addr_WriteBatWorkStateAddr					(uint16)(0x1800)
#define LCD_Addr_WriteBatRunStateAddr					(uint16)(0x1900)
#define LCD_Addr_WriteBatVolAddr							(uint16)(0x1A00)
#define LCD_Addr_WriteBatRefAddr							(uint16)(0x1B00)
#define LCD_Addr_WriteFirstWaringAddr						(uint16)(0x1C00)
#define LCD_Addr_WriteShowPwAddr							(uint16)(0x1D00)/*写密码显示控件*/
#define LCD_ADDR_UpdatePwInputAddr						(uint16)(0x1D10)/*更新密码输入控件*/
#define LCD_ADDR_UpdateOkBtnAddr						(uint16)(0x1D40)/*更新密码按键控件*/

#define LCD_Addr_RegisterInfoAddr							(uint16)(0x2A00)/*注册地址信息*/
#define LCD_Addr_LogoutInfoAddr								(uint16)(0x2A10)/*注销地址信息*/

#define LCD_Addr_WifiIsOkBtn									(uint16)(0x2A20)/*WIFI确认按钮*/

#define LCD_ADDR_DomainNameLocal						(uint16)(0x3000)/*本地域名*/
#define LCD_ADDR_DomainNameRemote						(uint16)(0x3100)/*远程域名*/

#define LCD_ADDR_DeviceID									(uint16)(0x3400)/*设备ID*/

#define LCD_Addr_WIFIName									(uint16)(0x3500)/*WIFI名称*/
#define LCD_Addr_WIFIPw										(uint16)(0x3600)/*WIFI密码*/

/*
** 页面ID参数
*/
typedef struct
{
	uint16 id;
}LCD_SCREENID;

/*
** 数据参数控制
*/
typedef struct
{
	uint16 addr;
	uint16 *pData;
}LCD_DATAFRAME;

/*
** 页面地址控制
*/
typedef struct
{
	uint8 addr;
	uint8 *pData;
}LCD_CTRLFRAME;

/*
** DWIN页面参数结构定义
*/
typedef struct{
	uint16 idle;/*空闲*/
	uint16 ing;/*充电*/
	uint16 full;/*满电*/
}LcdMainSysPara;

/*
** 主页面跳转参数
*/
typedef struct{
	uint16 doorAddr;/*仓门地址*/
	uint16 inputPw;/*输入密码*/
}LcdMainJumpPara;

/*
** 密码页面参数
*/
typedef struct{
	uint16 show[4];/*实现密码*/
	uint16 ascii[4];/*ASCII输入*/
	uint16 okBtn;/*确认键*/
}LcdAdminPw;

/*
** 系统参数之域名参数
*/
typedef struct{
	uint8 localShow[64];
	uint8 remoteShow[64];
}GetDomain;

/*
** 注册/注销
*/
typedef struct{
	uint16 _register;/*注册*/
	uint16 logout;/*注销*/
	uint16 registerState;/*注册状态*/
}SysCtr;

/*
** 设备ID
*/
typedef struct{
	uint16 show[8];
}DeviceId;

/*
** WIFI 名称和密码
*/
typedef struct{
	uint8 name[32];
	uint8 pw[32];
	uint16 btnIsOk;/*确认按钮*/
}WIFIPara;

/*
** 系统参数
*/
typedef struct{
	GetDomain getDomain;/*域名获取*/
	SysCtr sysCtr;/*注册/注销*/
	DeviceId deviceId;/*设备ID*/
	WIFIPara wifiPara;/*WIFI名称和密码*/
}LcdNewSysPara;


/*
** 系统参数之回调函数结构体声明
*/
typedef struct{
	uint16 addr;/*地址信息*/
	void (*handle)(void*,void*);
}LcdGetData;

/*
** 初始化LCD变量参数
*/
void init_LcdPara(void);

/*
** 迪文屏交互之上传参数
*/
bool LCD_SetData(void *pData,void *temp);
/*
** 迪文屏交互之下传参数
*/
bool LCD_GetData(void *pData);
/*
** 获取当前页面ID参数
*/
uint16 LCD_GetScreenID(void);
/*
** 设置跳转页面ID参数
*/
bool LCD_SetScreenID(uint16 screenId);
/*
** 获取设置页面ID参数
*/
void LCD_GetScreenIdSet(uint8*pdata);
/*
** 设置页面跳转参数
*/
bool LCD_JumpScreen(void *pData, uint8 dev);
/*
** 获取当前页面ID
*/
bool LCD_ReadScreenID(void *pData, uint8 dev);

/*
** 写主页面系统参数
*/
bool lcd_WriteMainStatePara(void *pData,uint8 dev);
/*
** 写主页面ICON相关参数
** icon相关参数帧数据长度--分为4组处理
*/
bool lcd_WriteMainIconPara(void *pData,uint8 dev);
/*
** 获取电池信息页面地址
*/
uint16 getLcdBatInfoDoorAddr(void);
/*
** 清除电池信息页面地址
*/
void clearLcdBatInfoDoorAddr(void);
/*
** 写主页面跳转--设备地址参数
*/
bool lcd_WriteMainJumpBatInfo(void *pData,uint8 dev);
/*
** 读主页面跳转--设备地址参数
*/
bool lcd_ReadMainJumpBatInfo(void *pData,uint8 dev);
/*
** get 密码输入地址参数
*/
uint16 get_InputPwPara(void);
/*
** clear 密码输入地址参数
*/
void clear_InputPwPara(void);
/*
** 写页面跳转--密码输入地址
*/
bool lcd_WriteMainInputPw(void *pData,uint8 dev);
/*
** 读页面跳转--密码输入地址
*/
bool lcd_ReadMainInputPw(void *pData,uint8 dev);
/*
** 写电池信息之基础信息
*/
bool lcd_WriteBatInfoBase(void *pData,uint8 dev);
/*
** 写电池信息之电池工作状态
*/
bool lcd_WriteBatInfoRunState(void *pData,uint8 dev);
/*
** 写电池信息之电池使用状态
*/
bool lcd_WriteBatInfoUseState(void *pData,uint8 dev);
/*
** 写电池信息之电芯信息
*/
bool lcd_WriteBatInfoVolState(void *pData,uint8 dev);
/*
** 写电池信息之相关信息之MOS状态
*/
bool lcd_WriteBatInfoRefState(void *pData,uint8 dev);
/*
** 写一级告警页面之警告信息
*/
bool lcd_WriteFirstWaringInfo(void *pData,uint8 dev);
/*
** clear 清空密码显示文本控件
*/
void clear_PwShow(void);
/*
** set 密码显示文本控件
*/
void set_PwAscii(void);
/*
** 写密码显示文本控件
*/
bool lcd_WritePwShow(void *pData,uint8 dev);
/*
** clear 密码输入控件
*/
void clear_PwInput(void);
/*
** get 密码输入控件
*/
LcdAdminPw get_PwInput(void);
/*
** 写密码输入控件
*/
bool lcd_WritePwInput(void *pData,uint8 dev);
/*
** 读取密码输入控件
*/
bool lcd_ReadPwInput(void *pData,uint8 dev);
/*
** clear okBtn按键值
*/
void clear_OkBtn(void);
/*
** get OkBtn按键值
*/
uint16 get_OkBtn(void);
/*
** 写密码确认控件
*/
bool lcd_WriteOkBtn(void *pData,uint8 dev);
/*
** 读取密码确认控件
*/
bool lcd_ReadPwOkBtn(void *pData,uint8 dev);
/*
** update Temp Domain -->更新域名临时参数
*/
void updateTempDomain(uint8* local,uint8* remote);
/*
** get Domain Legal Len
*/
uint8 getDomainIsLegalLen(uint8* space);
/*
** update Domain
*/
void update_Domain(void);
/*
** 写Local域名显示控件
*/
bool lcd_WriteLocalDomainShow(void *pData,uint8 dev);
/*
** get Local Domain Input
*/
void getLocalDomainInput(void* pData,void* temp);
/*
** get_Local Domain Input
*/
void get_LocalDomainInput(uint8* rx);
/*
** 读取local域名input
*/
bool lcd_ReadLocalDomainInput(void *pData,uint8 dev);
/*
** update Remote Demain Show
*/
void update_RemoteDomainShow(uint8* space);
/*
** 写Remote域名显示控件
*/
bool lcd_WriteRemoteDomainShow(void *pData,uint8 dev);
/*
** get Remote Domain Input
*/
void getRemoteDomainInput(void* pData,void* temp);
/*
** get_Remote Domain Input
*/
void get_RemoteDomainInput(uint8* rx);
/*
** 读取Remote域名
*/
bool lcd_ReadRemoteDomainInput(void *pData,uint8 dev);
/*
** clear 注册信息按键值
*/
void clear_RegisterBtn(void);
/*
** set 设置注册状态
*/
void set_RegisterState(uint16 state);
/*
** get 注册信息按键值
*/
uint16 get_RegisterBtn(void);
/*
** 写注册信息确认控件
*/
bool lcd_WriteRegisterBtn(void *pData,uint8 dev);
/*
** 读取注册信息确认控件
*/
bool lcd_ReadRegisterBtn(void *pData,uint8 dev);
/*
** clear 注销信息按键值
*/
void clear_LogoutBtn(void);
/*
** get 注销信息按键值
*/
uint16 get_LogoutBtn(void);
/*
** 写注销信息确认控件
*/
bool lcd_WriteLogoutBtn(void *pData,uint8 dev);
/*
** 读取注销信息确认控件
*/
bool lcd_ReadLogoutBtn(void *pData,uint8 dev);
/*
** update 设备ID
*/
void update_DeviceId(DeviceId id);
/*
** get 设备ID
*/
DeviceId get_DeviceId(void);
/*
** 写设备地址 柜子型号 主控型号
*/
bool lcd_WriteDeviceInfo(void *pData,uint8 dev);
/*
** get From Dwin DeviceId
*/
void get_FromDeinDeviceId(void* pData,void* temp);
/*
** 读设备地址 柜子型号 主控型号
*/
bool lcd_ReadDeviceInfo(void *pData,uint8 dev);
/*
** get WifiNamePW Legal Len
*/
uint8 get_WifiNamePwIsLegalLen(uint8* space);
/*
** init WifiName To DWIN Show
*/
void init_WifiNameToDwinShow(void);
/*
** update wifi nane
*/
void update_WifiName(uint8* space);
/*
** 写WIFI名称显示
*/
bool lcd_WriteWifiNameShow(void *pData,uint8 dev);
/*
** get From DWIN WIFI Name
*/
void get_FromDwinWifiName(void* pData,void* temp);
/*
** 读WIFI名称输入
*/
bool lcd_ReadWifiNameInput(void *pData,uint8 dev);
/*
** init WifiPw To DWIN Show
*/
void init_WifiPwToDwinShow(void);
/*
** update wifi Pw
*/
void update_WifiPw(uint8* space);
/*
** 写WIFI密码显示
*/
bool lcd_WriteWifiPwShow(void *pData,uint8 dev);
/*
** get From DWIN WIFI Pw
*/
void get_FromDwinWifiPw(void* pData,void* temp);
/*
** 读WIFI密码输入
*/
bool lcd_ReadWifiPwInput(void *pData,uint8 dev);
/*
** 写WIFI确认按钮
*/
bool lcd_WriteWifiOkBtn(void *pData,uint8 dev);
/*
** 读WIFI确认按钮
*/
bool lcd_ReadWifiOkBtn(void *pData,uint8 dev);

#endif

