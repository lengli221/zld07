#include "includes.h"

/*
** 任务堆栈大小
*/
#define SIZE_STK 1536

static uint8 OtherComNum=1;

/*
** 通讯任务表
*/
SM_TaskPara st_SMTaskPara[D_SM_TaskNum];

/*
** 显示屏迪文类
*/
static uint32 stk_LCDTask[SIZE_STK];
DP_FramePara SM_LCD_InitiativeFrame; 
static uint32 uiLCDComState;
static uint16  uiLCDComCnt[32];
static uint32 uiLCDComFlag;
static SM_LogicUnit st_LogicSheetLCD[] = {
	{D_SM_InitiativeCmd,D_LCD_CMD_JumpScreen	,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,LCD_JumpScreen,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadScreenId,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,LCD_ReadScreenID,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteStatePara,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteMainStatePara,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteIconPara,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteMainIconPara,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteJumpDeviceAddr,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteMainJumpBatInfo,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadJumpDeviceAddr,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadMainJumpBatInfo,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteInputPwAddr,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteMainInputPw,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadInputPwAddr,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadMainInputPw,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteBatInfoBase,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteBatInfoBase,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteBatInfoRun,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteBatInfoRunState,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteBatInfoUse,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteBatInfoUseState,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteBatInfoVol,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteBatInfoVolState,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteBatInfoRef,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteBatInfoRefState,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteFirstWaring,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteFirstWaringInfo,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WritePwShow,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WritePwShow,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WritePwInput,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WritePwInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadPwInput,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadPwInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadOkBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadPwOkBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteOkBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteOkBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteLocalDomainShow,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteLocalDomainShow,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadLocalDomainInput,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadLocalDomainInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_WriteRemoteDomainShow,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteRemoteDomainShow,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadRemoteDomainInput,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadRemoteDomainInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteRegisterBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteRegisterBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadRegisterBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadRegisterBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteLogoutBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteLogoutBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadLogoutBtn,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadLogoutBtn,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},	

	{D_SM_InitiativeCmd,D_LCD_CMD_CtrVoiceInfo,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteVoiceCtr,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteDeviceId,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteDeviceInfo,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadDeviceId,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadDeviceInfo,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteWifiName,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteWifiNameShow,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadWifiName,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadWifiNameInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},	

	{D_SM_InitiativeCmd,D_LCD_CMD_WriteWifiPw,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_WriteWifiPwShow,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},
	{D_SM_InitiativeCmd,D_LCD_CMD_ReadWifiPw,&SM_LCD_InitiativeFrame,DP_PackageFrame,DP_ParseFrame,LCD_SetData,lcd_ReadWifiPwInput,null,&OtherComNum,&uiLCDComState,uiLCDComCnt,&uiLCDComFlag},	
};
static const uint16 iLogicSheetLCDUnitNum = sizeof(st_LogicSheetLCD)/sizeof(SM_LogicUnit);

/*
** 迪文屏显示任务参数初始化
*/
static void SM_InitLCDTask(void)
{
	st_SMTaskPara[D_SM_TASKID_LCD].ucComType = BSP_ComType_USART;
	st_SMTaskPara[D_SM_TASKID_LCD].ucComID = BSP_ComDW_Usart;
	st_SMTaskPara[D_SM_TASKID_LCD].ucTableLen = iLogicSheetLCDUnitNum;
	st_SMTaskPara[D_SM_TASKID_LCD].pst_LogicTable = st_LogicSheetLCD;
	st_SMTaskPara[D_SM_TASKID_LCD].cmdNum = 0;

	uiLCDComState = 0;    
}

/*
** 通讯机任务接口函数
*/
void SM_InitSmComManagement(void){
	/*
	** 迪文屏显示任务参数初始化
	*/	
	SM_InitLCDTask();
	
	/*显示屏*/
  	OSTaskCreate(SM_DisposeSmCom, (void *)(&st_SMTaskPara[D_SM_TASKID_LCD]), &stk_LCDTask[SIZE_STK - 1], SM_DW_LCDTask_Prio);	
}

