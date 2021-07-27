#ifndef __INTERFACE_SW_H_
#define __INTERFACE_SW_H_

/*
** 切换接口EEPROM基准参数接口
*/
#define Interface_Sw_BaseAddr						0xA0
#define Interface_Sw_BaseLen							(sizeof(SaveIF_Label) + sizeof(uint16))

#pragma pack(1)
/*
** 切换标志和定时器
*/
typedef struct{
	bool sw_Flag;/*接口切换标志*/
	uint32 itick;/*定时器*/
	uint8 temp_Label;/*临时标号*/
}SwFlag_Tick;

/*
** save Interface Label 
*/
typedef struct{
	uint8 head;
	uint8 label;
	uint16 runLedBaseTime;/*运行基准时间*/
	uint8 end;
}SaveIF_Label;

/*
** Interface Sw typedef
*/
typedef struct{
	SwFlag_Tick swFlag_Tick;
	SaveIF_Label saveIF_Label;
}Interface_SwDef;
#pragma pack()


/*
** get Interface
*/
SaveIF_Label get_Interface(void);

/*
** set Interface 
*/
void set_Interface(uint8 label);

/*
** reset Interface Init 
*/
void reset_InterfaceInit(void);

/*
** chk Interface Sw Logic
*/
void chk_Interface_SwLogic(void);

/*
** update Interface Sw Flag
*/
void update_Interface_SwFlag(uint8 temp);


#endif

