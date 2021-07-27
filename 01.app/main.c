/**
**bin文件生成说明*****************************
**1.Run User Programs After Build/Rebuild
	勾选 Run #1:E:\00.ApplicationSoftware\00.keil\01.Install\ARM\ARMCC\bin\fromelf.exe --bin --output ./02.Output/chenfu.bin F:\00.CurrentSourceCode\01.ChangeCabinet\05.Okay\02.Project\00.prj\project\keil\output\chenfu.axf
**2.Read/Only Memory Areas
			Start		 	Size
	IROM1: 0x8040000   	0x5FFFF
**3.Create Exectable
	以下三项均不勾选
	Debug Information
	Create HEX File
	Browse Information
**4.修改文件system_stm32f2xx.c
	#define VECT_TAB_OFFSET  0x40000
***/

#include "includes.h"

int stk[1024] = {0}; 

void hard_init(void){	
	BspCPUClkFreq();
	BspInit();
}

void Thread(void *p){	
	/*
	** 提供系统软件创建上层协议解析接口函数
	*/
	UpperLayerParse_Init();
	/*
	** 提供系统软件创建上层协议回复接口函数
	*/	
	UpperLayerReply_Init();
	/*
	** 提供系统软件创建系统控制接口函数
	*/
	SysCtrlLogic_Init();
	/*
	** 通讯机任务接口函数
	*/
	SM_InitSmComManagement();	
	/*
	** 页面操作任务初始化
	*/
	HMI_Init();
	/*
	** 提供系统软件创建下层协议解析接口函数
	*/
	LowerLayerParse_Init();
	/*
	** 提供系统软件创建下层协议查询接口函数
	*/
	LowerLayerReply_Init();
	/*
	** 提供系统软件之远程升级接口函数之上层协议
	*/
	UgradeULParse_Init();	
	/*
	** 提供系统软件之远程升级接口函数之下层协议之回复函数
	*/
	UpgradeLLReply_Init();
	/*
	** 提供系统软件之远程升级接口函数之下层协议之解析函数
	*/
	UpgradeLLParse_Init();
	
	while(1)
	{
		Sleep(50);
	}
}

int main()
{
	#if OS_CRITICAL_METHOD == 3u           		
		OS_CPU_SR  cpu_sr = 0u;
	#endif
	OS_ENTER_CRITICAL();
	
	hard_init();
	os_init();

	Thread_create(Thread,0,&stk[1024 - 1],MainTask_Prio);

	OS_EXIT_CRITICAL();

	Thread_start();

	while(1)
	{
		Sleep(10);
	}
}

void App_TaskIdleHook(void)
{

}

