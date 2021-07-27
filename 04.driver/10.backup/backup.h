#ifndef __BACKUP_
#define __BACKUP_

/*
** 使能备份区时钟
*/
void PWR_BackupInit(void);
void BKP_ReadBackupRegDat(uint16_t Addr, uint8_t *Data,uint16_t Len);
void BKP_WriteBackupDat(uint16_t Addr, uint8_t *Data,uint16_t Len);

#endif

