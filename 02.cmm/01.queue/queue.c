#include "type.h"
#include "queue.h"

bool Enqueue(Queue *pQ, unsigned char e){          
	unsigned int rear;
	unsigned int front;

	rear = pQ->rear;
	front = pQ->front;

	if ((front != rear) || (!pQ->flag)){
		pQ->element[rear++] = e;
		rear %= pQ->total;
		if(front == rear){
		pQ->flag = 1;
	}
		pQ->rear = rear;
		return TRUE;                          
	}else{  
		return FALSE;                          
	}
}

bool Dequeue(Queue *pQ, unsigned char *pe)
{  
	unsigned int rear;
	unsigned int front;

	rear = pQ->rear;
	front = pQ->front;

	if ((front != rear) || (pQ->flag)){                                  
		*pe = pQ->element[front++];     
		front %= pQ->total;
		if (pQ->flag){  
			pQ->flag = 0;  
		}
		pQ->front = front;
		return TRUE;  
	}else{
		return FALSE;
	}
}

bool QueueIsFull(Queue *pQ){
	return  ((pQ->front == pQ->rear) && (pQ->flag));
}

bool QueueIsEmpty(Queue *pQ){
	return  ((pQ->front == pQ->rear) && (!pQ->flag));
}
