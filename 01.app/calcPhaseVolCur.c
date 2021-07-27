#include "includes.h"

/*--------------------------------------------三相电压校准------------------------------------------------------------*/
PhaseVoltAdjustLogic phaseVoltAdjustLogic = {0};
/*--------------------------------------------------------------------------------------------------------------------*/

/*
** get AD Convert Value -- 三相电压
*/
double get_ADConvertValue(uint8 ch){
	double tempp = ADC_GetADResultAD(ch);
	
	tempp *= 33;
	tempp /=10;
	tempp /=4096;
	tempp += 0.1;/*二极管调整*/
	/*tempp = (double)(ADC_GetADResultAD(ch)*33/10/4096);*/
	if(tempp<1.5){/*采样值优化处理*/
		tempp = 1.5;
	}

	return tempp;
}

/*
** get AD Convert C Value -- 三相电流
*/
double get_ADConvertCValue(uint8 ch){
	double tempp = ADC3_GetADResultAD(ch);
	
	tempp *= 33;
	tempp /=10;
	tempp /=4096;
	tempp += 0.085/*0*/;/*0.035*//*二极管调整*/
	if(tempp<1.5){/*采样值优化处理*/
		tempp = 1.5;
	}

	return tempp;
}

/*
** get 相电压采样转换ABC
*/
float get_PhaseV_AD(uint8 ch){
	double temp = 0;
	float tempp = 0;
	
	temp = get_ADConvertValue(ch);
	tempp = (float)(((double)((temp-1.5)*2*2006))/15);

	return(tempp);
}

/*
** get 相电流采样转换ABC
*/
float get_PhaseCur_AD(uint8 ch){
	double temp = 0;
	float tempp = 0;

	temp = get_ADConvertCValue(ch);
	tempp = (float)(((double)((temp - 1.5)*2*2000)));
	tempp /=(float)(1.18649635*50);

	return(tempp);
}

/*-------------------------------------------使用上位机矫正三相电压----------------------------------------------------------------*/
/*
** 获取低压侧AD值
*/
uint16 get_LvAdValue(uint8 ch){
	uint16 result = 0;
	if(get_PhaseV_AD(ch)>=160*1.414 && get_PhaseV_AD(ch) <= 180*1.414){
		result = ADC_GetADResultAD(ch);
	}else{
		result = 0xFFFF;
	}
	return result;
}

/*
** 获取高压侧AD值
*/
uint16 get_HvAdValue(uint8 ch){
	uint16 result = 0;
	if(get_PhaseV_AD(ch)>=260*1.414 && get_PhaseV_AD(ch) <= 280*1.414){
		result = ADC_GetADResultAD(ch);
	}else{
		result = 0xFFFF;
	}
	return result;
}

/*
** 设置校准系数
*/
void set_PhaseVoltAdjust(uint8 ch,bool flag,float k,float b){
	uint8 buf[AdjusrPhaseVoltLen] = {0};
	uint8 len = 0;
	uint16 crc = 0;

	/*更新全局变量参数*/
	phaseVoltAdjustLogic.flag[ch]  = flag;
	phaseVoltAdjustLogic.k[ch] = k;
	phaseVoltAdjustLogic.b[ch]= b;

	/*copy数据*/
	memcpy((uint8*)&buf[len],(uint8*)&phaseVoltAdjustLogic.flag[0],3*(sizeof(bool)+sizeof(float)+sizeof(float)));
	len += 3*(sizeof(bool)+sizeof(float)+sizeof(float));

	/*计算CRC校验*/
	crc = CRC16((uint8 *)&buf[0], len);
	memcpy((uint8*)&buf[len],(uint8*)&crc,sizeof(uint16));
	len += sizeof(uint16);

	/*保存数据*/
 	eeprom_write(AdjusrPhaseVoltAddr, (uint8 *)&buf[0], len);
 	BKP_WriteBackupDat(AdjusrPhaseVoltAddr, (uint8 *)&buf[0], len);	
}

/*
** 复位初始化校准系数
*/
void reset_PhaseVoltAdjust(void){
	uint8 buf[AdjusrPhaseVoltLen] = {0};
	uint8 buf_bkp[AdjusrPhaseVoltLen] = {0};
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;	
	uint8 i = 0; 

	/*读取存储参数*/
	eeprom_read(AdjusrPhaseVoltAddr, (uint8 *)&buf[len], AdjusrPhaseVoltLen);
	BKP_ReadBackupRegDat(AdjusrPhaseVoltAddr, (uint8*)&buf_bkp[len], AdjusrPhaseVoltLen);

	/*计算CRC校验*/
	crc = CRC16((uint8 *)&buf[len], (AdjusrPhaseVoltLen - sizeof(uint16)));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], (AdjusrPhaseVoltLen - sizeof(uint16)));
	/*设置偏移量*/
	len += (AdjusrPhaseVoltLen - sizeof(uint16));
	/*读取偏移量*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));

	if(crc == getCrc){
		memcpy((uint8*)&phaseVoltAdjustLogic.flag[0],(uint8*)&buf[0],(AdjusrPhaseVoltLen - sizeof(uint16)));
		BKP_WriteBackupDat(AdjusrPhaseVoltAddr, (uint8*)&buf[0], AdjusrPhaseVoltLen);
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&phaseVoltAdjustLogic.flag[0],(uint8*)&buf_bkp[0],(AdjusrPhaseVoltLen - sizeof(uint16)));
		eeprom_write(AdjusrPhaseVoltAddr, (uint8*)&buf_bkp[0], AdjusrPhaseVoltLen);
	}else{	
		/*初始化参数*/
		for(i=0;i<3;i++){
			set_PhaseVoltAdjust(i,false, 0, 0);
		}
	}
}

/*
** 计算三相校准电压
*/
float calc_PhaseAdjustVolt(uint8 ch){
	double temp = ADC_GetADResultAD(ch)*phaseVoltAdjustLogic.k[ch];
	temp +=  phaseVoltAdjustLogic.b[ch];
	phaseVoltAdjustLogic.volt[ch] = (uint16)(temp);

	return((float)(phaseVoltAdjustLogic.volt[ch]/100));
}
/*---------------------------------------------------------------------------------------------------------------------------------*/


/*
** get_计算三相电压电流
** 		策略:单点计算
*/
CalcPhaseVC get_CalcPhaseVC(void){
	CalcPhaseVC cpvc = {0}; 
	uint8 i = 0;

	/*
	** 计算三相电压
	*/
	for(i=0;i<3;i++){
		cpvc.abcPV[i]= get_PhaseV_AD(i);
		if(phaseVoltAdjustLogic.flag[i] == true){
			cpvc.abcPV[i] = calc_PhaseAdjustVolt(i);
		}
	}

	/*
	** 计算三相电流
	*/
	for(i=0;i<3;i++){
		cpvc.abcPC[i] = get_PhaseCur_AD(i);
	}

	return cpvc;
}

