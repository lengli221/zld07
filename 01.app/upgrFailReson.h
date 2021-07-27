#ifndef __UPGRFAILRESON_H_
#define __UPGRFAILRESON_H_

#pragma pack(1)
typedef struct{
	uint8 reson;/*失败原因*/
	uint16 hardVer;/*硬件版本*/
	uint8 fireId[16];/*固件ID*/
	uint16 softVer;/*升级前电池版本号*/
	uint8 id[16];/*电池ID*/
	uint16 fireSoftVer;/*固件软件版本号*//*新增V1.8*/
	uint8 upgrModel;/*升级模式*//*分控协议:V1.7*/
}Bat;/*同一结构体用于电池升级失败原因和充电器失败原因结构体*/

typedef struct{
	Bat bat;
}UpgrFail;/*升级失败原因--仅针对电池处理,为节约空间*/

/*
** Door Exit Upgr Reson No Read Flag
*/
typedef struct{
	DoorNumDefine bat;/*电池*/

}DoorEUResonNoRead;
#pragma pack()

/*
** 升级失败原因参数初始化--电池失败原因存在3(FIFOQueue_DepthLimit)级缓存
*/
void init_UpgrFailPara(void);
/*
** set--升级失败原因
*/
void set_UpgrFailPara(uint8 type,UpgrFail u,uint8 loc);
/*
** get--升级失败原因
*/
void* get_UpgrFailPara(uint8 type,uint8 loc);
/*
** 20210518--检测是否存在升级原因未读取
*/
void runtimeChk_UpgrResonFlag(void);
/*
** 20210518--获取原因上报
*/
DoorEUResonNoRead get_DoorEUResonNoRead(void);
#endif

