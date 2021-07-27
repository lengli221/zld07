#ifndef __UPGRADEFLASHADDRALLOT_H
#define __UPGRADEFLASHADDRALLOT_H

/*
** 远程升级之文件大小限制参数
*/
#define UpgrFileSizeLimit											(uint32)0x00032000/*0x00019000*//*100K -- 修改成:200K*/

/*
** 远程升级之外部Flash地址分配
** 1.通讯小板地址分配
** 2.电池包地址分配
*/
#define Upgrade_FlashAddrInit									(uint32)0x001F4000/*2048000/1024 = 2000K*/
#define Upgr_FlashAddr_Bat(loc)										(uint32)(Upgrade_FlashAddrInit + UpgrFileSizeLimit + loc*UpgrFileSizeLimit)/*(Upgrade_FlashAddrInit + loc*UpgrFileSizeLimit)*/


/*
** get Flash 升级存储地址
*/
uint32 getUpgrFlashAddr(UpgrFilePara upgr);

#endif

