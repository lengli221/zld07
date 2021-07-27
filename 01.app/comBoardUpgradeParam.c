#include "includes.h"

SeqQueue comQueue = {0};
ComBup comBup[SeqQueue_DepthLimit] = {0};
ComBup curComBup = {0};

/*
** 通讯板升级参数初始化
*/
void ComBoardUpgrPara_Init(void){
	/*
	** 队列初始化
	*/
	Init_SeQueue((SeqQueue *)&comQueue);
	/*
	** 清除当前通讯升级任务参数
	*/
	Clear_CurComBupPara();
}

/*
** 清除当前通讯升级任务参数
*/
void Clear_CurComBupPara(void){
	memset((uint8*)&curComBup.upgradeWay,0x00,sizeof(ComBup));
	/*
	** 清除升级计数次数
	*/	
	clear_UpgrLLParamCnt();
}

/*
** get 当前升级任务相关参数
*/
ComBup getCurComBup(void){
	return curComBup;
}

/*
** 更新通讯小板队列参数
*/
void UpdataComBoardQueuePara(ComBup com){
	uint16 depth = Size_SeqQueue((SeqQueue *)&comQueue);

	memset((uint8*)&comBup[depth].binFileType.flag,0x00,sizeof(ComBup));
	comBup[depth] = com;
	Push_SeqQueue((SeqQueue *)&comQueue, (void *)&comBup[depth]);
}

/*
** 通讯板任务更新+任务相关参数更新
*/
void ComBoard_TaskSwitch(void){
// 	static bool resumeFlag = true;
// 	static bool resFlag = true;
	
	/*
	**	状态检查
	*/
	if(curComBup.binFileType.flag == 0){/*通讯小板无任何类型文件更新*/
		if(Size_SeqQueue((SeqQueue *)&comQueue) > 0){
			curComBup = *(ComBup*)Pop_SeqQueue((SeqQueue *)&comQueue);
			/*
			** 升级之参数结构体变量初始化
			*/
			UpgradeLLParam_Init();
			/*
			** 清除升级计数次数
			*/	
			clear_UpgrLLParamCnt();			
			/*
			** 升级次数自加
			*/
			add_UpgrCnt();
		}		
	}

// 	/*
// 	** 任务切换
// 	*/
// 	if(curComBup.binFileType.flag != 0){/*通讯小板有文件需要下载--执行升级任务*/
// 		if(resumeFlag == true){/*挂起App接口任务*/
// 			resumeFlag = false;
// 			OSTaskSuspend(LowerLayerParseTask_Prio);
// 			OSTaskSuspend(LowerLayerReplyTask_Prio);
// 		}
// 		
// 		if(resFlag == false){/*恢复升级接口任务*/
// 			resFlag = true;
// 			OSTaskResume(UgradeLLReplyTask_Prio);
// 			OSTaskResume(UgradeLLParseTask_Prio);
// 		}		
// 	}else{/*执行App接口函数任务*/
// 		if(resumeFlag == false){/*恢复App接口任务*/
// 			resumeFlag = true;
// 			OSTaskResume(LowerLayerParseTask_Prio);
// 			OSTaskResume(LowerLayerReplyTask_Prio);			
// 		}

// 		if(resFlag == true){/*挂起升级接口*/
// 			resFlag = false;
// 			OSTaskSuspend(UgradeLLReplyTask_Prio);
// 			OSTaskSuspend(UgradeLLParseTask_Prio);
// 		}
// 	}
}

