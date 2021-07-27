#ifndef __FIFOQUEUE_H_
#define __FIFOQUEUE_H_

/*
** 配置FIFO队列最大深度
*/
#define FIFOQueue_DepthLimit						(uint16)0x03

/*
** FIFO队列结构
*/
typedef struct{
	void* data[FIFOQueue_DepthLimit];
	uint16 size;
}FIFOQueue;


/*
** 队列初始化 
*/
void init_FIFOQueue(FIFOQueue* q);
/*
** 入队
*/
void push_FIFOQueue(FIFOQueue* q,void* data);
/*
** 出队
*/
void* pop_FIFOQueue(FIFOQueue* q);
/*
** 返回队列大小
*/
uint16 size_FIFOQueue(FIFOQueue* q);

#endif

