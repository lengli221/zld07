#include "includes.h"

/*
** 队列初始化 
*/
void init_FIFOQueue(FIFOQueue* q){
	uint16 i = 0;

	for(i=0;i < FIFOQueue_DepthLimit;i++){
		q->data[i] = NULL;
	}

	q->size = 0;
}

/*
** 入队
*/
void push_FIFOQueue(FIFOQueue* q,void* data){
	
	if(q == NULL || data == NULL){
		return;
	}

	q->data[q->size] = data;
	q->size++;
}

/*
** 出队
** 20210227--新增临时变量
*/
UpgrFail upgrFailTempPara = {0};
void* pop_FIFOQueue(FIFOQueue* q){
	uint16 i = 0;
	void* data = NULL;

	if(q == NULL || q->size == 0){
		return NULL;
	}

	/*data = q->data[0];*/
	/*拷贝上报数据*/
	memcpy((UpgrFail*)&upgrFailTempPara,(UpgrFail*)q->data[0],sizeof(UpgrFail));
	data = (UpgrFail*)&upgrFailTempPara;

	for(i=0;i < q->size - 1;i++){
		/*q->data[i] = q->data[i+1];*//*拷贝地址不对--应修改成拷贝数据*/
		memcpy(q->data[i],q->data[i+1],sizeof(UpgrFail));/*拷贝数据--是否可以优化不使用sizeof(UpgrFail)--增加代码复用性*/
		/*清已拷贝数据--2021027*/
		memset(q->data[i+1],0,sizeof(UpgrFail));
	}
	/*清指针指向空间位置*/
	q->data[q->size-1] = NULL;
	q->size--;

	return data;
}

/*
** 返回队列大小
*/
uint16 size_FIFOQueue(FIFOQueue* q){
	return(q->size);
}

