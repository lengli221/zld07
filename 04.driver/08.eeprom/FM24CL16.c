#include <stm32f2xx.h>
#include "string.h"
#include "FM24CL16.h"

#define I2C_ERROR_TIMES     		20

#define FM24WP_ON					(GPIOE->BSRRL = GPIO_Pin_15)
#define FM24WP_OFF					(GPIOE->BSRRH = GPIO_Pin_15)

#define SCL_H							(GPIOB->BSRRL = GPIO_Pin_10)   
#define SCL_L	    						(GPIOB->BSRRH = GPIO_Pin_10)

#define SDA_H							(GPIOB->BSRRL = GPIO_Pin_11)
#define SDA_L							(GPIOB->BSRRH  = GPIO_Pin_11)

#define SCL_read						(GPIOB->IDR  & GPIO_Pin_10)
#define SDA_read						(GPIOB->IDR  & GPIO_Pin_11)

#define I2C1_SLAVE_ADDRESS7    	0xA0
#define I2C_PageSize           			256

static void I2C_delay(void)
{
	u32 i = 200;
	while(i)
	{
		i--;
	}
}

static bool I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	I2C_delay();
	I2C_delay();

	if(!SDA_read)
	{
		return false; //SDA线为低电平则总线忙,退出
	}
	SDA_L;
	I2C_delay();
	if(SDA_read) 
		return false; //SDA线为高电平则总线出错,退出
	SDA_L;
	I2C_delay();
	return true;
}

void I2C_ReStart(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_L;
	I2C_delay();

}

static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

static void I2C_Ack(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static void I2C_NoAck(void)
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

static bool I2C_WaitAck(void) //返回为:=1有ACK,=0无ACK
{
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read)
	{
		SCL_L;
		return false;
	}
	SCL_L;
	return true;
}

void I2C_SendByte(uint8 SendByte) //数据从高位到低位
{
	uint8 i = 8;
	while(i--)
	{
		SCL_L;
		I2C_delay();
		if(SendByte & 0x80)
			SDA_H;
		else
			SDA_L;

		SendByte <<= 1;
		I2C_delay();
		SCL_H;
		I2C_delay();
	}
	SCL_L;
}

static uint8 I2C_ReceiveByte(void)  //数据从高位到低位
{
	uint8 i = 8;
	uint8 ReceiveByte = 0;

	SDA_H;
	while(i--)
	{
		ReceiveByte <<= 1;
		SCL_L;
		I2C_delay();
		SCL_H;
		I2C_delay();
		if(SDA_read)
		{
			ReceiveByte |= 0x01;
		}
	}
	SCL_L;
	return ReceiveByte;
}

#define FMWRITECMD	0xA0
#define FMREADCMD	0xA1
uint8 FMRead(uint8 *readdata, uint32 addr, uint16 num)
{
	unsigned char addr_h, addr_l;
	unsigned char dat;
	unsigned short i;
	addr = addr & 0x07ff;
	addr_h = addr >> 8;
	addr_h = addr_h << 1;
	addr_l = addr & 0xff;

	I2C_Start();
	I2C_SendByte(FMWRITECMD + addr_h);
	if(!I2C_WaitAck())
	{
		return 0;
	}

	I2C_SendByte(addr_l);
	if(!I2C_WaitAck())
	{
		return 0;
	}

	I2C_ReStart();
	I2C_SendByte(FMREADCMD + addr_h);	//发送读命令
	if(!I2C_WaitAck())
	{
		return 0;
	}
		for(i = 0; i < num; i++)
		{
			dat = I2C_ReceiveByte();
			if(i < num - 1)
				I2C_Ack();
			*readdata = dat;
			readdata++;
		}
	I2C_NoAck();
	I2C_Stop();
	return true;
}

uint8 FMWrite(uint8 *writedata, uint32 addr, uint16 num)
{
	unsigned char addr_h, addr_l;
	unsigned short i;
	
	FM24WP_OFF;
	for(i = 0; i < 50; i++)
		I2C_delay();
	
	addr = addr & 0x07ff;
	addr_h = addr >> 8;
	addr_h = addr_h << 1;
	addr_l = addr & 0xff;

	I2C_Start();
	I2C_SendByte(FMWRITECMD + addr_h);
	if(!I2C_WaitAck())
	{
		return 0;
	}

	I2C_SendByte(addr_l);
	if(!I2C_WaitAck())
	{
		return 0;
	}
	for(i = 0; i < num; i++)
	{
		I2C_SendByte(*writedata);
		writedata++;
		if(!I2C_WaitAck())
		{
			return 0;
		}
	}
	I2C_Stop();
	for(i = 0; i < 50; i++)
		I2C_delay();
	FM24WP_ON;
	return true;
}

void FM24_Init( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


