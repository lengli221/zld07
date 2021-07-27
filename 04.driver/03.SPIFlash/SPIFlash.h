#ifndef SPI_FLASH_H
#define SPI_PLASH_H
#include "stm32f2xx.h"
#include "stm32f2xx_spi.h"
#include "type.h"

#define sFLASH_CMD_WRITE          					0x02/*Write to Memory instruction*/
#define sFLASH_CMD_WRSR          	 				0x01/*Write Status Register instruction*/
#define sFLASH_CMD_WREN           					0x06/*Write enable instruction*/
#define sFLASH_CMD_READ           					0x03/*Read from Memory instruction*/
#define sFLASH_CMD_RDSR           					0x05/*Read Status Register instruction*/
#define sFLASH_CMD_RDID           					0x9F/*Read identification*/
#define sFLASH_CMD_SE             						0x20/*//0xD8Sector Erase instruction*/
#define sFLASH_CMD_BE             						0xC7/*Bulk Erase instruction*/

#define sFLASH_WIP_FLAG           					0x01/*Write In Progress (WIP) flag*/

#define sFLASH_DUMMY_BYTE         					0xA5
#define sFLASH_SPI_PAGESIZE       					0x100
#define SPI_FLASH_ID              						0xEF7119/*W25Q256*//*0xef4016*//*W25Q32*/

#define sFLASH_SPI_SECTORSIZE	  				4096			

#define FLASH_DUMMY_BYTE    						0x55

unsigned int SPI_flash_ReadID(void);

void sFLASH_ReadBuffer(uint8 *pBuffer, uint32 ReadAddr, uint16 NumByteToRead);
void sFLASH_EraseSector(uint32 SectorAddr);
void sFLASH_WriteBuffer(uint8 *pBuffer, uint32 WriteAddr, uint16 NumByteToWrite);
void sFLASH_WriteData(uint8 *pBuffer, uint32 WriteAddr, uint16 NumByteToWrite);
void sFLASH_WriteDataWithClr(uint8 *pBuffer, uint32 WriteAddr, uint16 NumByteToWrite);

#define SPI_flash_read(a,b,c) sFLASH_ReadBuffer(b,a,c)/*¶ÁFlash*/
#define SPI_flash_write(a,b,c) sFLASH_WriteBuffer(b,a,c)/*Ð´Flash*/
#if Debug_Flash_BugAnalysis == 1
/*
** Flash Bugµ÷ÊÔËµÃ÷
*/
void sFlash_BugAnalysis(void);
#endif
#define SPI_flash_erase(a) sFLASH_EraseSector(a)
#define SPI_flash_writeData(a,b,c) sFLASH_WriteData(b,a,c)
#define SPI_flash_writewithclr(a,b,c) sFLASH_WriteDataWithClr(b,a,c)

uint8 spi_flash_init(void);

#endif

