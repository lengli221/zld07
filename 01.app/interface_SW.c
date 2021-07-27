#include "includes.h"

/*
** Interface Sw typedef
*/
Interface_SwDef interface_SwDef;

/*
** get Interface
*/
SaveIF_Label get_Interface(void){
	return interface_SwDef.saveIF_Label;
}

/*
** set Interface 
*/
void set_Interface(uint8 label){
	uint8 buf[Interface_Sw_BaseLen] = {0};
	uint16 crc = 0;
	uint8 len = 0;	

	interface_SwDef.saveIF_Label.head = 0x23;
	interface_SwDef.saveIF_Label.end = 0x45;
	interface_SwDef.saveIF_Label.label = label;/*接口标号*/
	switch(label){
		case 0x01:/*对接上层接口:232*/
			interface_SwDef.saveIF_Label.runLedBaseTime = 2000;/*运行灯以2S闪烁*/
			break;
		case 0x02:/*对接上层接口:485*/
			interface_SwDef.saveIF_Label.runLedBaseTime = 4000;/*运行灯以2S闪烁*/	
			break;
	}

	memcpy((uint8*)&buf[len],(uint8*)&interface_SwDef.saveIF_Label.head,sizeof(SaveIF_Label));
	len += sizeof(SaveIF_Label);
	crc = CRC16((uint8 *)&buf[0], sizeof(SaveIF_Label));
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);	

	eeprom_write(Interface_Sw_BaseAddr, (uint8 *)&buf[0], Interface_Sw_BaseLen);
	BKP_WriteBackupDat(Interface_Sw_BaseAddr, (uint8*)&buf[0], Interface_Sw_BaseLen);		
}

/*
** reset Interface Init 
*/
void reset_InterfaceInit(void){
	uint8 buf[Interface_Sw_BaseLen] = {0};
	uint8 buf_bkp[Interface_Sw_BaseLen] = {0};	
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	SaveIF_Label saveIF_LabelTemp = {0};
	
	eeprom_read(Interface_Sw_BaseAddr, (uint8 *)&buf[len], Interface_Sw_BaseLen);
	BKP_ReadBackupRegDat(Interface_Sw_BaseAddr, (uint8*)&buf_bkp[len], Interface_Sw_BaseLen);	

	/*计算CRC16校验*/
	crc = CRC16((uint8 *)&buf[len], sizeof(SaveIF_Label));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(SaveIF_Label));
	len += sizeof(SaveIF_Label);	

	/*get CRC16校验*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));	

	if(crc == getCrc){
		memcpy((uint8*)&saveIF_LabelTemp.head,(uint8*)&buf[0],sizeof(SaveIF_Label));
		if(saveIF_LabelTemp.head == 0x23 && saveIF_LabelTemp.end == 0x45){
			memcpy((uint8*)&interface_SwDef.saveIF_Label.head,(uint8*)&saveIF_LabelTemp.head,sizeof(SaveIF_Label));
			BKP_WriteBackupDat(Interface_Sw_BaseAddr, (uint8*)&buf[0], Interface_Sw_BaseLen);
		}
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&saveIF_LabelTemp.head,(uint8*)&buf_bkp[0],sizeof(SaveIF_Label));
		if(saveIF_LabelTemp.head == 0x23 && saveIF_LabelTemp.end == 0x45){
			memcpy((uint8*)&interface_SwDef.saveIF_Label.head,(uint8*)&saveIF_LabelTemp.head,sizeof(SaveIF_Label));
			eeprom_write(Interface_Sw_BaseAddr, (uint8*)&buf_bkp[0], Interface_Sw_BaseLen);
		}
	}else{
		/*6A默认接口485*/
		interface_SwDef.swFlag_Tick.temp_Label = 0x02;
		set_Interface(interface_SwDef.swFlag_Tick.temp_Label);
	}
}

/*
** chk Interface Sw Logic
*/
void chk_Interface_SwLogic(void){
	if(interface_SwDef.swFlag_Tick.sw_Flag == true){
		if(TickOut((uint32 *)&interface_SwDef.swFlag_Tick.itick, 1000) == true){
			TickOut((uint32 *)&interface_SwDef.swFlag_Tick.itick, 0);
			interface_SwDef.swFlag_Tick.sw_Flag = false;
			set_Interface(interface_SwDef.swFlag_Tick.temp_Label);
			/*复位系统*/
			NVIC_SystemReset();
		}
	}
}

/*
** update Interface Sw Flag
*/
void update_Interface_SwFlag(uint8 temp){
	interface_SwDef.swFlag_Tick.sw_Flag = true;
	interface_SwDef.swFlag_Tick.temp_Label = temp;
	TickOut((uint32 *)&interface_SwDef.swFlag_Tick.itick, 0);
}

