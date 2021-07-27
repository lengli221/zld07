#include "includes.h"

extern SM_TaskPara st_SMTaskPara[D_SM_TaskNum];

/*
** 用于升级清空通讯机
*/
void clear_SmCmd(uint8 ucSmNo){
	uint32 iParam[3] = {0};
	uint8 i = 0;

	for(i=0;i<3;i++){
		st_SMTaskPara[ucSmNo].iParam[i] = iParam[0];
	}
}

/*
** 设置通讯机命令
*/
bool SM_SetCmd(uint8 ucSmNo,uint32* piParam){
	uint8 j = 0;
	uint16 cnt = 0;	
	
	if (piParam == null || ucSmNo >= D_SM_TaskNum) 
		return false;
	
	st_SMTaskPara[ucSmNo].cmdNum++;

	do{
		
		cnt++;
		Sleep(10);
	}while((st_SMTaskPara[ucSmNo].iParam[0] != 0) && (cnt < 100));/*等待之前的指令执行完毕*/

	for(j =0; j < 3;j++){
		st_SMTaskPara[ucSmNo].iParam[2-j] = piParam[2-j];
	}
		
	st_SMTaskPara[ucSmNo].cmdNum--;
	
	if(st_SMTaskPara[ucSmNo].cmdNum < 0){
		st_SMTaskPara[ucSmNo].cmdNum = 0;
	}
	return true;
}

/*
** 获取通讯机命令
*/
bool SM_GetCmd(uint8 ucSmNo,uint32* piParam){
	if(piParam != null){
		memcpy(piParam,st_SMTaskPara[ucSmNo].iParam,sizeof(st_SMTaskPara[ucSmNo].iParam));
	}
	
	if(st_SMTaskPara[ucSmNo].iParam[0]){
		return true;/*当前有命令需要发送*/
	}else{
		return false;/*当前通讯空闲*/
	}
}

/*
** 通讯机发送函数
*/
uint8 ucComBuf[256];
bool SM_DisposeComUnit(SM_TaskPara *pst_TaskPara, int i,uint8 ucAddr)
{
	//uint8 ucComBuf[256];
	uint16 ucLen = 0;
	uint16 ucRecvLen = 0;
	uint16 ucRet = 0;
	uint16 rcv = D_RCV_CNT;
	int j = 0;
	
	memset(ucComBuf,0,sizeof(ucComBuf));

    	/*打包命令帧*/
    	pst_TaskPara->pst_LogicTable[i].PackageFrame(pst_TaskPara->pst_LogicTable[i].pFrame, ucComBuf, &ucLen);

	/*发送命令帧*/
    	BSP_SmSend(pst_TaskPara->ucComType, pst_TaskPara->ucComID,ucComBuf, &ucLen );
	
    	/*判断该通信单元是否需要做解析，不需要的话直接返回*/
   	 if(pst_TaskPara->pst_LogicTable[i].ParseFrame == null) 
        	return true;
	 
    	ucLen = 0;
    	/*等待返回数据10ms*/	
	rcv = 100;
	Sleep(10);
	
	/*接收响应帧*/
	for(j = 0;j < rcv;j++){    
		BSP_SmRecv(pst_TaskPara->ucComType, pst_TaskPara->ucComID,ucComBuf + ucLen,&ucRecvLen);
		ucLen += ucRecvLen;
		
		if(ucLen >( sizeof(ucComBuf))){
			break;
		}
		
		/*解析响应帧*/
		ucRet = pst_TaskPara->pst_LogicTable[i].ParseFrame(pst_TaskPara->pst_LogicTable[i].pFrame,ucComBuf,ucLen);
		if(ucRet)/*解析成功*/{
			if(pst_TaskPara->ucComID == BSP_ComDW_Usart){
				*pst_TaskPara->pst_LogicTable[i].ucComState = 0;
			}else{
				*pst_TaskPara->pst_LogicTable[i].ucComState = (*(pst_TaskPara->pst_LogicTable[i].ucComState)) &(~(1<<ucAddr));
			}
			break;
		}
		/*等待150ms 继续接收数据 */
		Sleep(10);
	}
	
	/*解析失败*/
	if((ucRet == false) || (j >= rcv)){
		if(pst_TaskPara->ucComID == BSP_ComDW_Usart) {
			*pst_TaskPara->pst_LogicTable[i].ucComState = 1;
		}else{
			*pst_TaskPara->pst_LogicTable[i].ucComState = (*(pst_TaskPara->pst_LogicTable[i].ucComState))|(1<<ucAddr);
		}
	}
	
    /*将解析出的数据填入设备数据*/
    pst_TaskPara->pst_LogicTable[i].SetDataCmd(pst_TaskPara->pst_LogicTable[i].pFrame,&ucAddr);
    return true;
}

/*
** 通讯机任务函数
*/
void SM_DisposeSmCom(void *pPara)
{    
	uint8 i=0,j=0,k=0;
	SM_TaskPara *pst_TaskPara = (SM_TaskPara*)pPara;

	while(1){        	
		if(pst_TaskPara->iParam[0] == D_SM_NewCmd)/*是否是循环指令*/{/*任务切换已决定任务参数的传递*/
			for (j = 0; j < pst_TaskPara->ucTableLen; j++){                                                
				if(pst_TaskPara->pst_LogicTable[j].ucCmdCode == pst_TaskPara->iParam[1])/*新的指令指令是什么*/{                 
					if(pst_TaskPara->pst_LogicTable[j].GetDataCmd != null)/*判断需要读取的数据区域是否为空*/{
						pst_TaskPara->pst_LogicTable[j].GetDataCmd(pst_TaskPara->pst_LogicTable[j].pFrame,
							pst_TaskPara->iParam[2]);
						if(pst_TaskPara->ucComID == BSP_ComDW_Usart){
							if((((DP_FramePara *)(pst_TaskPara->pst_LogicTable[j].pFrame))->iAddress == 0)||
							(((DP_FramePara *)(pst_TaskPara->pst_LogicTable[j].pFrame))->ucRegLen == 0)){
								break;
							}
						}
					}

					if(pst_TaskPara->ucComType == D_ComType_COM) {
						SM_DisposeComUnit(pst_TaskPara,j,pst_TaskPara->iParam[2]);
						if(pst_TaskPara->ucComID == BSP_ComDW_Usart) {
							if((*(pst_TaskPara->pst_LogicTable[j].ucComState)) == 1){
								pst_TaskPara->pst_LogicTable[j].ComFailCnt[0]++;
								if( pst_TaskPara->pst_LogicTable[j].ComFailCnt[0]>=10)
								{
									pst_TaskPara->pst_LogicTable[j].ComFailCnt[0]=11;  
									*pst_TaskPara->pst_LogicTable[j].ucComFlag = 1;
								}
							}else{
								pst_TaskPara->pst_LogicTable[j].ComFailCnt[0] = 0;
								*pst_TaskPara->pst_LogicTable[j].ucComFlag = 0;
							}
						}else{
							if((*(pst_TaskPara->pst_LogicTable[j].ucComState))&(1<<pst_TaskPara->iParam[2])){
								pst_TaskPara->pst_LogicTable[j].ComFailCnt[pst_TaskPara->iParam[2]]++;
								if( pst_TaskPara->pst_LogicTable[j].ComFailCnt[pst_TaskPara->iParam[2]]>=10){
									pst_TaskPara->pst_LogicTable[j].ComFailCnt[pst_TaskPara->iParam[2]]=11;  
									*pst_TaskPara->pst_LogicTable[j].ucComFlag|=(1<<pst_TaskPara->iParam[2]);
								}
							}else{  
								pst_TaskPara->pst_LogicTable[j].ComFailCnt[pst_TaskPara->iParam[2]]=0;  
								*pst_TaskPara->pst_LogicTable[j].ucComFlag&=(~(1<<pst_TaskPara->iParam[2]));	
							}
						}
					}
					break;
				}    
			}
			pst_TaskPara->iParam[0] = D_SM_NoCmd;/*是否有新指令的参数清零*/
		}else{
			if(i >= pst_TaskPara->ucTableLen)
				i=0;
			
			for(; i < pst_TaskPara->ucTableLen; i++){
				if(pst_TaskPara->pst_LogicTable[i].ucCmdType == D_SM_CycleCmd){    
					if(k >= (*pst_TaskPara->pst_LogicTable[i].ucDevNum)){
						k = 0;
					}
					
					for(; k < (*pst_TaskPara->pst_LogicTable[i].ucDevNum); k++){
						if(pst_TaskPara->iParam[0] == D_SM_NewCmd)
							break;
						
						if(pst_TaskPara->pst_LogicTable[i].GetDataCmd != null)/*判断需要读取的数据区域是否为空*/{
							pst_TaskPara->pst_LogicTable[i].GetDataCmd(pst_TaskPara->pst_LogicTable[i].pFrame,k);
						}
						
						if(pst_TaskPara->ucComType == D_ComType_COM) {
							SM_DisposeComUnit(pst_TaskPara,i,k);
							if((*(pst_TaskPara->pst_LogicTable[i].ucComState))&(1<<k)){
								pst_TaskPara->pst_LogicTable[i].ComFailCnt[k]++;
								
								if( pst_TaskPara->pst_LogicTable[i].ComFailCnt[k]>=10){
									pst_TaskPara->pst_LogicTable[i].ComFailCnt[k]=11;  
									*pst_TaskPara->pst_LogicTable[i].ucComFlag|=(1<<k);	
								}
							}else{  
								pst_TaskPara->pst_LogicTable[i].ComFailCnt[k]=0;  
								*pst_TaskPara->pst_LogicTable[i].ucComFlag&=(~(1<<k));	
							}
						}
					}
				} 
			}
		}

		/*
		** 更新看门狗寄存器--喂狗
		*/
		watchdogUpdate();
		
		Sleep(20);	
    	}
}

