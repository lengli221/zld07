#ifndef __SMCOMMANAGER_H_
#define __SMCOMMANAGER_H_

#define D_SM_TASKID_LCD       								0/*显示屏*/
#define D_SM_TaskNum            								1/*通信任务最大数量*/  

#define D_SM_NewCmd             								0x0001/*有新指令消息*/       
#define D_SM_NoCmd              									0x0000/*没有新指令消息*/    

#define D_SM_CycleCmd           								0x0000/*循环指令信息*/      
#define D_SM_InitiativeCmd      								0x0001/*主动指令信息*/   

#define D_ComType_COM            				 				0x0001/*串口类型号*/ 

#define D_RCV_CNT               									100


/*
** 与显示屏通讯命令
*/
#define D_LCD_CMD_JumpScreen							0x00/*页面跳转*/
#define D_LCD_CMD_ReadScreenId						0x01/*获取当前页面*/
#define D_LCD_CMD_WriteStatePara						0x02/*写主页面状态参数*/
#define D_LCD_CMD_WriteIconPara						0x03/*写主页面icon相关参数*/
#define D_LCD_CMD_WriteJumpDeviceAddr				0x04/*写主页面跳转至电池信息地址*/
#define D_LCD_CMD_ReadJumpDeviceAddr				0x05/*读主页面跳转至电池信息地址*/
#define D_LCD_CMD_WriteInputPwAddr					0x06/*写主页面输入页面地址*/
#define D_LCD_CMD_ReadInputPwAddr					0x07/*读主页面输入页面地址*/
#define D_LCD_CMD_WriteBatInfoBase					0x08/*写电池信息*/
#define D_LCD_CMD_WriteBatInfoRun					0x09
#define D_LCD_CMD_WriteBatInfoUse					0x0A
#define D_LCD_CMD_WriteBatInfoVol						0x0B
#define D_LCD_CMD_WriteBatInfoRef						0x0C
#define D_LCD_CMD_WriteFirstWaring					0x0D/*写一级页面告警*/
#define D_LCD_CMD_WritePwShow						0x0E/*写密码显示控件*/
#define D_LCD_CMD_WritePwInput						0x0F/*写密码输入控件*/
#define D_LCD_CMD_ReadPwInput						0x10/*读密码输入控件*/
#define D_LCD_CMD_ReadOkBtn							0x11/*读密码Ok按键*/
#define D_LCD_CMD_WriteOkBtn							0x12/*写密码Ok按键*/

#define D_LCD_CMD_WriteLocalDomainShow			0x13/*写local域名显示*/
#define D_LCD_CMD_ReadLocalDomainInput			0x14/*读local域名输入*/		
#define D_LCD_CMD_WriteRemoteDomainShow		0x15/*写remote域名显示*/
#define D_LCD_CMD_ReadRemoteDomainInput			0x16/*读remote域名输入*/	

#define D_LCD_CMD_WriteRegisterBtn					0x17/*写注册信息*/
#define D_LCD_CMD_ReadRegisterBtn					0x18/*读注册信息*/
#define D_LCD_CMD_WriteLogoutBtn						0x19/*写注销信息*/
#define D_LCD_CMD_ReadLogoutBtn						0x1A/*读注销信息*/

#define D_LCD_CMD_CtrVoiceInfo							0x1B/*控制语音信息*/

#define D_LCD_CMD_WriteDeviceId						0x1C/*写设备ID*/
#define D_LCD_CMD_ReadDeviceId						0x1D/*读设备ID*/

#define D_LCD_CMD_WriteWifiName						0x1E/*写WIFI名称*/
#define D_LCD_CMD_ReadWifiName						0x1F/*读WIFI名称*/

#define D_LCD_CMD_WriteWifiPw							0x20/*写WIFI密码*/
#define D_LCD_CMD_ReadWifiPw							0x21/*读WIFI密码*/

/*
** 通信逻辑处理单元
*/
typedef struct __SM_LogicUnit__{
	uint8 ucCmdType;/*命令类型 是否循环*/ 
	uint32 ucCmdCode;/*命令编号*/
	void *pFrame;/*组解包帧*/  
	bool (*PackageFrame)(void *,void *,uint16 *);/*组包回调*/
	bool (*ParseFrame)(void *,void *,uint16);/*解包回调*/
	bool (*SetDataCmd)(void *,void *);/*设置数据回调(帧数据)服务与(*ParseFrame)解析函数*/
	bool (*GetDataCmd)(void *,uint8);/*获取数据回调（帧数据 设备号)服务与(*PackageFrame)打包函数*/
	void *pDevAddrs;/*从机地址*/
	uint8 *ucDevNum;
	uint32 *ucComState;/*通信状态(支持32个从设备)--通信异常置1*/
	uint16 *ComFailCnt;/*通讯失败数*/
	uint32 *ucComFlag;/*通讯最终状态	通信异常置1*/
}SM_LogicUnit;

/*
** 通信任务参数
*/
typedef struct __SM_TaskPara__{
	uint8 ucComType;/*通信端口类型*/
	uint8 ucComID;/*通信端口号*/
	SM_LogicUnit *pst_LogicTable;/*通信逻辑表*/
	uint16 ucTableLen;/*通信逻辑表单元数*/
	uint32 iParam[3];/*参数 1为是否新指令 2为指令内容 3为从设备号*/
	int16 cmdNum;
} SM_TaskPara;


/*
** 通讯机设置命令
*/
bool SM_SetCmd(uint8 ucSmNo,uint32* piParam);
/*
** 获取通讯机命令
*/
bool SM_GetCmd(uint8 ucSmNo,uint32* piParam);
/*
** 通讯机任务函数
*/
void SM_DisposeSmCom(void *pPara);
/*
** 通讯机任务接口函数
*/
void SM_InitSmComManagement(void);
#endif

