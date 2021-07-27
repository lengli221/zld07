#ifndef __LCDDATA_VOICE_H_
#define __LCDDATA_VOICE_H_

/*
** voice loc 
*/
#define _DoorBase_VoiceLoc				(uint8)140/*仓门基础标号*/
#define _SmokeWaring						(uint8)188/*烟雾报警*/
#define _ChgDoorTempOT					(uint8)190/*充电器仓温度过高*/
#define _ChgTempOT							(uint8)192/*充电器温度过高*/
#define _BatTempOT							(uint8)194/*电池温度过高*/
#define _ChgTimeTimeOut					(uint8)196/*充电时间超过10小时*/
#define _BatUpgr								(uint8)198/*电池升级*/
#define _ComUpgr								(uint8)200/*分控升级*/
#define _BatErr									(uint8)202/*电池故障*/
#define _comErr								(uint8)204/*分控故障*/
#define _ChgErr								(uint8)206/*充电器故障*/
#define _ComIsLose							(uint8)208/*分控失联*/
#define _AllBatUpgr							(uint8)210/*所有电池升级中*/
#define _AllComUpgr							(uint8)212/*所有分控升级中*/
#define _FileDownload						(uint8)214/*文件下载中*/
#define _PhaseOverV							(uint8)216/*过压保护*/
#define _PhaseLV								(uint8)218/*欠压保护*/
#define _PhaseOC								(uint8)220/*过流保护*/
#define _BatUpgrFinsh								(uint8)222/*电池升级完成*/

/*
** 音频控制
*/
bool lcd_WriteVoiceCtr(void *pData,uint8 dev);
#endif

