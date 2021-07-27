#ifndef __WATCHDOG_H
#define __WATCHDOG_H

/*
** 看门狗初始化
*/
void watchdogInit(void);
/*
** 更新看门狗寄存器--喂狗
*/
void watchdogUpdate(void);

#endif

