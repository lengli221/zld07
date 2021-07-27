#include "includes.h"

int8 Thread_create(void (*fun)(void *p_arg),void *p_arg,int32 *stk,int8 prio)
{
	int8 ret = 0;
	ret = OSTaskCreate(fun, p_arg, (OS_STK *)stk, prio);
	if(OS_ERR_NONE == ret)
	{
		return BSP_OS_SUCCESS;
	}
	else
	{
		return BSP_OS_FAILURE;
	}
}

void Thread_start(void)
{
	OS_CPU_SysTickInit(); //操作系统滴答初始
	OSStart();
}

void os_init(void)
{
    OSInit();
}

