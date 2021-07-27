#ifndef OS_APP_CFG_H_
#define OS_APP_CFG_H_

/*
** 调试系统模块功能性#define 
** 1.CAN1 传输使能调试
*/
#define Debug_CAN1_TxFun 								1
#define MaskOCur_ShortFun									0/*置1--开启过流检测 清0--屏蔽过流检测*/

/*
** 清除所有固件包
*/
#define ClearAllFireware										0
/*
** 屏蔽过压过流检测
*/
#define MaskOVCChk											0

/*
** 客户端选择
*/
#define Client_Choice_MeiTuan								0x01/*美团*/
#define Client_Choice_PineCone							0x02/*快松果*/
#define Client_Choice_All										0xFE/*兼容客户端*/
#define Client_Choice											Client_Choice_MeiTuan

/*
** 控制板兼容迪文屏
*/
#define Screen_Compatibility								0x00/*安卓兼容迪文屏*/

/*
** 控制板硬件版本
*/
#define CtrHardware_Dwin									0x01/*迪文屏*/
#define CtrHardware_Andriod								0x02/*安卓*/
#define CtrHardware											CtrHardware_Andriod


/*
** VerCtr
*/
#define VerCtr_TestFrock									0x01/*测试工装*/
#define VerCtr_Normal										0x02/*正式出货版本*/
#define VerCtr													VerCtr_Normal	
												
///*
//** 迪文屏版本控制
//** 1.I代迪文屏
//** 2.II代迪文屏
//*/
//#define DW_ONE_SCREEN									0x01
//#define DW_SECOND_SCREEN								0x02

//#define DW_GENERATION_SCREEN						DW_SECOND_SCREEN

#define Debug_Flash_BugAnalysis  					0

/*
**任务优先级别处理
*/
#define MainTask_Prio										(int8)10
#define SysCtrlLogic_Prio										(int8)8
#define UpperLayerParseTask_Prio						(int8)6
#define UpperLayerReplyTask_Prio						(int8)7
#define LowerLayerParseTask_Prio						(int8)4
#define LowerLayerReplyTask_Prio						(int8)5

#define UgradeULParseTask_Prio							(int8)11
#define UgradeLLReplyTask_Prio							(int8)13
#define UgradeLLParseTask_Prio							(int8)12

#define SM_DW_LCDTask_Prio								(int8)15
#define HMI_DW_Task_Prio									(int8)16

#endif

