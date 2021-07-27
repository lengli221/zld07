#ifndef __SYSCTRLOGIC_H_
#define __SYSCTRLOGIC_H_

/*仓门数*/
#define SysCtr_AllDoorNum						(uint8)0x30//系统仓门数--美团:电池仓数--48

/*
** 系统控制参数结构体定义
*/
#pragma pack(1)

#pragma pack()


/*
** 检测一级页面警报
*/
bool checkFirstPageWaring(void);
/*
** 增加一级报警禁止升级以及下载文件
*/
bool oneWaring_ForbidUpgrDownFile(void);
/*
** 新增:整柜一级告警标记判定--20210512
*/
bool cabinet_OneWaringFlagJudge(void);
/*
** 20210302 -- 一级报警中需手动恢复的逻辑--需手动上下电(下电AC)逻辑
*/
void oneWaring_NeedHandRecoverLogic(void);
/*
**提供系统软件创建系统控制接口函数
*/
void SysCtrlLogic_Init(void);

#endif

