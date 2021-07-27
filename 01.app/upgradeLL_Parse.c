#include "includes.h"

static int32 upgradeLLParse_TaskStk[512] = {0};

/*
** 使用Extern调用文件UpgradeLL_Reply.c变量
*/
extern UpgradeLLParam upgradeLLParam;

/*
** 升级之查询通讯小板运行区域
*/
void UpgradeLLParse_ChkComRunArea(uint8 len,uint8* item,uint8 addr){
	uint8 rxlen = 0;
	
	/*
	** 运行区域回复
	*/
	switch(item[rxlen]){
		case 0x01:/*BootLoader*/
			upgradeLLParam.proCtr.areaFlag |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			/*
			** 增加允许升级兼容性 -- 20210221 -- 升级失败之后可能存在无判断允许升级--因为查询运行区域和查询版本号是同一函数包下发
			*/
			//upgradeLLParam.proCtr.upgradePermit |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			break;
// 		case 0x02:/*App*/
// 			upgradeLLParam.proCtr.areaFlag &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);
// 			/*
// 			** 增加允许升级兼容性
// 			*/
// 			upgradeLLParam.proCtr.upgradePermit &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);	
// 			break;
	}
}

/*
** 升级之升级文件版本号 -- 0xF1
*/
void UpgradeLLParse_FileVer(uint8 len,uint8* item,uint8 addr){
	uint8 rxlen = 0;

	/*
	** 升级文件版本号回复 是否允许 升级文件类型 系统版本号
	*/
	switch(item[rxlen]){/*是否允许*/
		case 0x01:/*允许升级*/
			upgradeLLParam.proCtr.upgradePermit |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			/*
			** 增加升级状态变更
			*/
			
			break;
		case 0x2:/*拒绝升级*/
			upgradeLLParam.proCtr.upgradePermit &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);	
			break;
	}
}

/*
** 升级之文件字节数+文件内容校验 -- 0xF2
*/
void UpgradeLLParse_FileByteItemCrc(uint8 len,uint8* item,uint8 addr){
	uint8 rxlen = 0;
	/*20210221--规避分控,发送完检验失败之后,还会响应F2--字段:0x02*/
	static DoorNumDefine sub_ChkFailFlag = 0;
	
	/*
	** 更新帧接收标志
	*/
	upgradeLLParam.proCtr.fileByteCrcRecFlag |= (DoorNumDefine)((DoorNumDefine)1<<addr);
	
	/*
	** 状态回复分析
	*/
	switch(item[rxlen]){
		case 0x01:/*等待数据项传输*/
			/*
			** 更新数据传输标志
			*/
			upgradeLLParam.proCtr.binDataItemTransmit |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			/*清除优化标志*/
			sub_ChkFailFlag = 0;
			break;
		case 0x02:/*完成数据接收,进行数据校验等待更新App区*/
			/*
			** 更新结束数据传输
			*/
			upgradeLLParam.proCtr.binDataItemEndFlag |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			/*规避:分控在校验失败之后会回复F2--0x02字段*/
			upgradeLLParam.proCtr.binDataItemEndFlag &= (DoorNumDefine)(~sub_ChkFailFlag);			
			break;
		case 0xF6:/*更新App软件版本号失败*/
		case 0xF7:/*更新App区编码失败*/
		case 0xF8:/*擦除App区不失败*/
		case 0xF9:/*擦除备份区失败*/
		case 0xFA:/*文件内容字节数超空间设计*/
		case 0xFB:/*文件内容校验合法*/
		case 0xFC:/*接收文件数据校验不合法*/
		case 0xFD:/*恢复运行APP区*/
			sub_ChkFailFlag |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			UpgradeLL_AssignCanelUpgrade(addr);
			break;
	}

	/*
	** 对比进入bootloader标志位与接收标志位等同时进入下一流程--修改时间:20210226--需要连续查询3S确保一次置位"开始升级标志"-->屏蔽下述检测直接跳转条件
	*/
	//if(upgradeLLParam.proCtr.areaFlag == upgradeLLParam.proCtr.fileByteCrcRecFlag
	//	&& upgradeLLParam.fileByteItemCrc.cmd == 0x01){/*在开始传输数据时才进入内容传输流程*/
	//	upgradeLLParam.proCtr.step = UpgradeLL_Proc_DataItemTra;
	//	/*
	//	** 升级之更新升级文件内容参数
	//	*/
	//	Upgrade_UpdateFileItemPara(1);			
	//}

	/*
	** 对比进入数据更新项地址和结束更新项地址一致性--20210226中途退出的由于标志位:binDataItemTransmi并未清除,等待3S之后查询所有的数据后从查询内进入结束流程
	** 为中途退出的:从此处进入结束流程
	*/
	if(upgradeLLParam.proCtr.binDataItemTransmit == upgradeLLParam.proCtr.binDataItemEndFlag
		&& upgradeLLParam.fileByteItemCrc.cmd == 0x02){/*结束数据帧传输*/
		upgradeLLParam.proCtr.step  = UpgradeLL_Proc_Finsh;/*结束进程*/	
	}
}

/*
** 升级之文件内容--F3
*/
void UpgradeLLParse_FileItem(uint8 len,uint8* item,uint8 addr){
	uint8 rxlen = 0;
	uint16 frameLabel = 0;
	volatile uint8 byteNum = 0;
	uint8 replyAnalysis = 0;
	static bool recFlag = true;
	/*------------------20210226目前采取的方案是:整个进入文件内容下载(F3)环节,分控不再响应-------------*/
	/*
	** 帧标识 帧数据内容字节数 回复分析
	*/
	memcpy((uint8*)&frameLabel,(uint8*)&item[rxlen],sizeof(uint16));
	rxlen += sizeof(uint16);
	byteNum = item[rxlen];
	rxlen += sizeof(uint8);
	replyAnalysis = item[rxlen];
	rxlen += sizeof(uint8);

	switch(replyAnalysis){/*回复分析*/
		case 0x01:/*对应帧完成接收*/
		case 0x05:/*帧重复*/
			/*
			** 增加优化处理
			*/
			if(upgradeLLParam.proCtr.fileItemRecFlag == 0){
				recFlag = false;
			}
			upgradeLLParam.proCtr.fileItemRecFlag |= (DoorNumDefine)((DoorNumDefine)1<<addr);
			
			/*
			** 对比接收文件字节数+文件内容校验标志与文件内容接收标志
			*/
			if(upgradeLLParam.proCtr.binDataItemTransmit == upgradeLLParam.proCtr.fileItemRecFlag
				&& recFlag == false){
				recFlag = true;
				upgradeLLParam.fileItemCtr.cnt = 0;
				upgradeLLParam.fileItem.frameLabel++;
				
				/*
				** 升级之文件内容之数据帧完成发送标志位
				*/			
				if(Upgrade_FileItemTraFinshFlag() == true){
					upgradeLLParam.proCtr.step  = UpgradeLL_Proc_FileByteCrc;
					upgradeLLParam.fileByteItemCrc.circCnt = 0;
					/*
					** 更新升级命令之结束数据传输
					*/
					upgradeLLParam.fileByteItemCrc.cmd = 0x02;/*结束数据传输*/
					/*清结束传输定时器--分控可能最后一包未接收成功--修改时间:20210222--上午11:01*/
					TickOut((uint32 *)&upgradeLLParam.fileByteItemCrc.itick, 0);
				}
			}
			break;
		case 0x02:/*流程异常(未成功接收功能0xF2相关)*/			
		case 0x03:/*帧标识异常(备注:帧跨越)*/
		case 0x04:/*写Flash异常*/
			upgradeLLParam.proCtr.fileItemRecFlag &= ~(DoorNumDefine)((DoorNumDefine)1<<addr);	
			UpgradeLL_AssignCanelUpgrade(addr);
			break;			
	}
}

/*
** 提供系统软件之远程升级接口任务之下层协议之解析函数
*/
void SM_UpgradeLLParse_Task(void* p_arg){
	/*
	** 系统复位延时6S,防止任务先启动,导致通讯板程序进入Bootloader区
	*/
	Sleep(6000);
	
	for(;;){
		/*
		** 下层协议之数据解析之任务函数处理
		*/
		lowerUpgrFunAnalyze();

		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
		
		Sleep(4);
	}
}

/*
** 提供系统软件之远程升级接口函数之下层协议之解析函数
*/
void UpgradeLLParse_Init(void){
	Thread_create(SM_UpgradeLLParse_Task, 0,
		(int32 *)&upgradeLLParse_TaskStk[512-1], (int8)UgradeLLParseTask_Prio);
}

