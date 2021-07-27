#include "includes.h"

/*
** 使能备份区时钟
*/
void PWR_BackupInit(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM,ENABLE);
	PWR_BackupRegulatorCmd(ENABLE);
	while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
	{

	}
}

#define BKSRAM_BASE ((__IO uint8_t * )(BKPSRAM_BASE))
void BKP_WriteBackupDat(uint16_t Addr, uint8_t *Data,uint16_t Len)
{
	  __IO uint8_t *BkpAddr = 0;
	  uint16_t i = 0;

	 /* if(Addr > 3072) return;*/
	  
	  BkpAddr = BKSRAM_BASE + Addr;
		
	  for (i=0; i< Len; i++)
	  {
		*BkpAddr++ = Data[i];
	  }
}

void BKP_ReadBackupRegDat(uint16_t Addr, uint8_t *Data,uint16_t Len)
{
	  __IO uint8_t *BkpAddr = 0;
	  uint16_t i = 0;

	  /*if(Addr > 3072) return;*/

	  BkpAddr = BKSRAM_BASE + Addr;
		
	  for (i=0; i< Len; i++)
	  {
		Data[i] =  *BkpAddr++;
	  }
}

