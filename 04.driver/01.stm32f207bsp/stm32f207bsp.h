#ifndef __STM32F207BSP_H_
#define __STM32F207BSP_H_

uint32 BspCPUClkFreq(void);
void BspInit(void);
int8 eeprom_write(int32 addr, uint8 *buf, int32 len);
int8 eeprom_read(int32 addr, uint8 *buf, int32 len);

#endif

