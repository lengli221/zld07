#ifndef __INTERFACE_SW_H_
#define __INTERFACE_SW_H_

/*
** �л��ӿ�EEPROM��׼�����ӿ�
*/
#define Interface_Sw_BaseAddr						0xA0
#define Interface_Sw_BaseLen							(sizeof(SaveIF_Label) + sizeof(uint16))

#pragma pack(1)
/*
** �л���־�Ͷ�ʱ��
*/
typedef struct{
	bool sw_Flag;/*�ӿ��л���־*/
	uint32 itick;/*��ʱ��*/
	uint8 temp_Label;/*��ʱ���*/
}SwFlag_Tick;

/*
** save Interface Label 
*/
typedef struct{
	uint8 head;
	uint8 label;
	uint16 runLedBaseTime;/*���л�׼ʱ��*/
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

