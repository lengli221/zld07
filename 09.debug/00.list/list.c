#include "includes.h"
#if Debug_List == 1
/*
** 内联关键字 inline
** 编译报错
*/
// static /*inline*/  void __list_add(struct list_head *_new,struct list_head *prev,struct list_head *next){
// 								  next->prev = _new;
// 								  _new->next = next;
// 								  _new->prev = prev;
// 								  prev->next = _new;
// }

/*
** 链表头和尾增加结点--函数指向链表的head结点后插入new结点
*/
/*static*/ void list_add(struct list_head *_new,struct list_head *head){
	__list_add(_new,head,head->next);
}

/*
** 栈--函数向指定链表的head结点前插入new结点
*/
/*static*/ void list_add_tail(struct list_head *_new,struct list_head *head){
	__list_add(_new,head->prev,head);
}

/*
** 调试list
*/
LIST_HEAD(myList_Head);
/*
** struct list_head _new = {NULL,NULL};
** struct list_head _new1 = {NULL,NULL};
*/

LIST_HEAD(_new);
LIST_HEAD(_new1);
LIST_HEAD(_new2);
LIST_HEAD(_new3);

void Debug_ListParam(void){
// 	struct list_head _new;
// 	struct list_head _new1;
// 	struct list_head _new2;
// 	struct list_head _new3;
	
	myList_Head.prev = &myList_Head;
	myList_Head.next = &myList_Head;

// 	_new.prev = &_new;
// 	_new.next = &_new;	

// 	_new1.prev = &_new1;
// 	_new1.next = &_new1;	
// 	
// 	_new2.prev = &_new2;
// 	_new2.next = &_new2;
// 	
// 	_new3.prev = &_new3;
// 	_new3.next = &_new3;	

	
// 	list_add((struct list_head *)&_new, (struct list_head *)&myList_Head);
// 	list_add((struct list_head *)&_new1, (struct list_head *)&myList_Head);

// 	list_add((struct list_head *)&myList_Head, (struct list_head *)&_new);
// 	list_add((struct list_head *)&myList_Head, (struct list_head *)&_new1);	
	
// 	list_add((struct list_head *)&myList_Head, (struct list_head *)&_new);
// 	list_add((struct list_head *)&_new1, (struct list_head *)&_new);		
	
	
	//=============================================================================
//  	list_add((struct list_head *)&_new, (struct list_head *)&myList_Head);	
//  	list_add((struct list_head *)&_new1, (struct list_head *)&myList_Head);	
//  	list_add((struct list_head *)&_new2, (struct list_head *)&myList_Head);		
//  	list_add((struct list_head *)&_new3, (struct list_head *)&myList_Head);	
	
	//--------------------------------------------------------------------------------
//  	list_add((struct list_head *)&myList_Head, (struct list_head *)&_new);
//  	list_add((struct list_head *)&_new, (struct list_head *)&_new1);	
//  	list_add((struct list_head *)&_new1, (struct list_head *)&_new2);	
//  	list_add((struct list_head *)&_new2, (struct list_head *)&_new3);	

// 	list_add_tail((struct list_head *)&_new, (struct list_head *)&myList_Head);
// 	list_add_tail((struct list_head *)&_new1, (struct list_head *)&myList_Head);	

	//================================================================================
 	list_add_tail((struct list_head *)&_new, (struct list_head *)&myList_Head);	
 	list_add_tail((struct list_head *)&_new1, (struct list_head *)&myList_Head);	
 	list_add_tail((struct list_head *)&_new2, (struct list_head *)&myList_Head);		
 	list_add_tail((struct list_head *)&_new3, (struct list_head *)&myList_Head);		
}

// #define N = 10;

// struct numlist{
// 	int num = 0;
// 	struct
// };

#endif


