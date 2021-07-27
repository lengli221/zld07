#ifndef __BINFILEPARAM_H_
#define __BINFILEPARAM_H_

/*
** 远程升级之文件相关参数
*/
#pragma pack(1)
typedef struct{
	uint32 len;
	uint16 crc16;
}BinFileItemPara;
#pragma pack()

#endif

