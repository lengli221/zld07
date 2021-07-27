#include "includes.h"

/*
** 写音频参数
*/
void write_VoicePara(uint8* space,uint8 dev){
	uint8 voiceLoc = 0;
	uint8 num = 0;
	
	if(dev < SysCtr_AllDoorNum){
		voiceLoc = _DoorBase_VoiceLoc + dev;
		num = 1;
	}else{
		voiceLoc = dev;
		num = 2;
	}	
	/*
	** 音乐播放设置
	**	D3：本次播放的起始段，0x00-0xFF 
	**	D2：本次播放段数，0x01-0xFF，DGUS 处理后清零；蜂鸣器模式下是鸣叫时间，单位 8ms
	**	D1：播放音量，单位为 1/256。
	**	D0：实时返回的播放进度（剩余播放段数目 0x00-0xFF）
	**	D3 和 D2 同时写入 0x00 将停止当前播放
	*/
	space[0] = voiceLoc;
	space[1] = num;
	space[2] = 0xFF;
	space[3] = 0xFF;
}

/*
** 音频控制
*/
bool lcd_WriteVoiceCtr(void *pData,uint8 dev){
	DP_FramePara *st_FramePara = (DP_FramePara *)pData;

	if(pData == null){
		return false;
	}

	st_FramePara->iAddress = LCD_ADDR_WriteVoice;
	st_FramePara->ucFunCode = D_DP_DataWirte;
	st_FramePara->ucRegLen = LCD_ADDR_Len_WriteVoice;

	/*
	** 写音频参数
	*/	
	write_VoicePara((uint8 *)&st_FramePara->ucDataSpace[0], dev);

	return true;
}

