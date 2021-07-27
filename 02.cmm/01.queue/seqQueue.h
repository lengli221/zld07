#ifndef __SEQQUEUE_H_
#define __SEQQUEUE_H_

/*
** 队列最大深度参数配置
*/
#define SeqQueue_DepthLimit						(uint16)30

/*
** 顺序队列结构
*/
typedef struct{
	void* data[SeqQueue_DepthLimit];
	uint16 size;
}SeqQueue;

/*
** 队列初始化
*/
void Init_SeQueue(SeqQueue* q);
/*
** 入队
*/
void Push_SeqQueue(SeqQueue* q,void* data);
/*
** 出队
*/
void* Pop_SeqQueue(SeqQueue* q);
/*
** 返回队列大小
*/
uint16 Size_SeqQueue(SeqQueue* q);
#endif

