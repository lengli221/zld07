#ifndef __SYSCTRLOGIC_H_
#define __SYSCTRLOGIC_H_

/*������*/
#define SysCtr_AllDoorNum						(uint8)0x30//ϵͳ������--����:��ز���--48

/*
** ϵͳ���Ʋ����ṹ�嶨��
*/
#pragma pack(1)

#pragma pack()


/*
** ���һ��ҳ�澯��
*/
bool checkFirstPageWaring(void);
/*
** ����һ��������ֹ�����Լ������ļ�
*/
bool oneWaring_ForbidUpgrDownFile(void);

/*
**�ṩϵͳ��������ϵͳ���ƽӿں���
*/
void SysCtrlLogic_Init(void);

#endif
