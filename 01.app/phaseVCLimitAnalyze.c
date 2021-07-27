#include "includes.h"

extern Hmi_PhaseVC hmi_PhaseVc;
extern UpperLayerPara upperLayerPara;

/*
** 分析相电压/相电流是否存在异常		
*/
extern bool oneWaringResetSetFlag;
PhaseVCAnalyze get_PhaseVCAnalyze(void){
	CalcPhaseVC cpvs = get_CalcPhaseVC();
	static PhaseVCAnalyze  pvca = {0};
	uint8 i = 0;
	static uint32 itickPV[3] = {0};
	static uint32 itickPC[3] = {0};
	static bool updateFlag = false;
	static bool powerOnFlag = false;/*上电标志*/
	static uint32 itick = 0;/*上电检测定时器*/
	static bool l_Flag = false;
	static bool h_Flag = false;
	static uint32 severityItick[3] = {0};/*严重过压定时器--V1.5*/
	static uint8 phaseOverVFaultRecoverFlag = 0;/*三相电压严重过压恢复标志*/
	static uint8 phaseLVRecoverFlag = 0;/*三相电压欠压恢复标志*/
	static uint8 phaseOverVWaringRecoverFlag = 0;/*三相电压过压恢复标志*/
	static uint8 phaseOCRecoverFlag = 0;/*三相电压过流恢复标志*/
	
	/*
	** 软件模拟检测空开是否断开
	*/
	static uint32 chkTimCnt[3] = {0};
	union __VoltChk{
		uint8 flag;
		struct {
			uint8 a;
			uint8 b;
			uint8 c;
		}bits;
	}voltChk;
	/*
	** 欠压连续检测2S以上
	*/
	static uint32 itickLVContinuous[3] = {0};

/*屏蔽过压过流检测*/
#if MaskOVCChk == 0
	if(powerOnFlag == false){
		TickOut((uint32 *)&itick, 0);/*电压电流检测延时6S检测*/
		powerOnFlag = true;
		TickOut((uint32 *)&severityItick[i], 0);/*严重过压定时器--V1.5*/
	}
	
	if(TickOut((uint32 *)&itick, 6000) == true){
		/*
		** 电压状态更新
		*/	
		if(updateFlag == false){
			for(i=0;i<3;i++){
				if(cpvs.abcPV[i] >= 270*1.414){
					/*清欠压检测定时器*/
					TickOut((uint32 *)&itickLVContinuous[i], 0);
					
					#if VerCtr == VerCtr_Normal
						pvca.pVol.flag |= (1<<i);
						/*TickOut((uint32 *)&itickPV[i], 0);*/
						
						/*updateFlag = true;*//*变更检测电压标志,超阈值不再恢复*/
						
						/*根据美团最新需求,相电压>270V可自动恢复*/
						TickOut((uint32 *)&itickPV[i], 0);
						l_Flag = false;/*低压标志*/
						h_Flag = true;/*高压标志*/

						TickOut((uint32 *)&severityItick[i], 0);/*严重过压定时器--根据V1.5版本*/
						phaseOverVFaultRecoverFlag = 0;
						
						hmi_PhaseVc.bits.hv = true;
						hmi_PhaseVc.bits.lv = false;
						/*增加上传服务器相电压过压之后,需下电操作才能恢复*/
						upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault = true;
						/*20210128--严重过压之后,清除过压标志*/
						upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = false;
						upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseLV = false;
					#endif
				}else{
					/*检测已离开严重过压--清除相应严重过压标志*/
					if(cpvs.abcPV[i]  < 264*1.414 &&TickOut((uint32 *)&severityItick[i], tickTimeBase_V) == true){
						phaseOverVFaultRecoverFlag |= ((uint8)(1<<i));
						TickOut((uint32 *)&severityItick[i], 0);
						if(phaseOverVFaultRecoverFlag == 0x07){/*三相电压已恢复*/
							phaseOverVFaultRecoverFlag = 0;
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault = false;
						}
					}else{
						if(cpvs.abcPV[i]  > 266*1.414){/*清定时器预留阈值2V*/
							TickOut((uint32 *)&severityItick[i], 0);/*新增:恢复之后连续检测1min正常再上报清除标志位*/
						}
					}
				
					if(cpvs.abcPV[i]  <= 166*1.414){
						/*欠压检测连续检测2S以上才判定欠压*/
						if(TickOut((uint32 *)&itickLVContinuous[i], 2000) == true){
							#if VerCtr == VerCtr_Normal
								if(upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == false){/*判定开关已检测拉下,故不检测欠压*/
									pvca.pVol.flag |= (1<<i);
									TickOut((uint32 *)&itickPV[i], 0);
									l_Flag = true;/*低压标志*/
									h_Flag = false;/*高压标志*/
									hmi_PhaseVc.bits.hv = false;
									hmi_PhaseVc.bits.lv = true;
									/*增加上传服务器相电压欠压,可恢复*/
									upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = false;
									upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseLV = true;
									/*Clear--三相电压欠压恢复标志*/
									phaseLVRecoverFlag = 0;
								}
							#endif
						}
					}
					else if(cpvs.abcPV[i]  >= 264*1.414){
						/*清欠压检测定时器*/
						TickOut((uint32 *)&itickLVContinuous[i], 0);
						
						#if VerCtr == VerCtr_Normal
							pvca.pVol.flag |= (1<<i);
							TickOut((uint32 *)&itickPV[i], 0);
							l_Flag = false;/*低压标志*/
							h_Flag = true;/*高压标志*/
							hmi_PhaseVc.bits.hv = true;
							hmi_PhaseVc.bits.lv = false;
							/*过压可恢复--修改:20210128--严重过压标志存在,不检测过压*/
							if(upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault == true/*严重过压标志*/){
								upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = false;
							}else{
								upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = true;
							}
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseLV = false;
							/*clear--三相电压过压恢复标志*/
							phaseOverVWaringRecoverFlag = 0;
						#endif
					}else{
						/*清欠压检测定时器*/
						TickOut((uint32 *)&itickLVContinuous[i], 0);
					
						if(cpvs.abcPV[i] >= 180*1.414 && TickOut((uint32 *)&itickPV[i], tickTimeBase_V) == true && l_Flag == true){
							pvca.pVol.flag &= ~(1<<i);
							phaseLVRecoverFlag |= ((uint8)(1<<i));
							if(phaseLVRecoverFlag == 0x07){
								phaseLVRecoverFlag = 0;
								l_Flag = false;
								hmi_PhaseVc.bits.lv = false;
								/*欠压恢复*/
								upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseLV = false;
							}
						}else{
							if(l_Flag == true&&cpvs.abcPV[i] < 178*1.414){/*清定时器预留阈值2V*/
								TickOut((uint32 *)&itickPV[i], 0);/*新增:20210201--回复时间以恢复到执行标准之后开始计时*/
							}
						}
						
						if(cpvs.abcPV[i] <= 255*1.414 && TickOut((uint32 *)&itickPV[i], tickTimeBase_V) == true && h_Flag == true){/*回差和时间限制,防止抖动*/
							pvca.pVol.flag &= ~(1<<i);
							phaseOverVWaringRecoverFlag |= ((uint8)(1<<i));
							if(phaseOverVWaringRecoverFlag == 0x07){
								phaseOverVWaringRecoverFlag = 0;
								h_Flag = false;
								hmi_PhaseVc.bits.hv = false;
								/*过压恢复*/
								upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = false;
								/*清除严重过压标志*/
								upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault = false;
							}
						}else{
							if(h_Flag == true && cpvs.abcPV[i] > 257*1.414){/*清定时器预留阈值2V*/
								TickOut((uint32 *)&itickPV[i], 0);/*新增:20210201--回复时间以恢复到执行标准之后开始计时*/
							}
						}
					}	
				}

				if(cpvs.abcPV[i]  <= (float)(150*1.414)){/*判定开关已按下*/
					if(TickOut((uint32 *)&chkTimCnt[i], 100) == true){
						voltChk.flag |= (1<<i);
					}
				}else{
					TickOut((uint32 *)&chkTimCnt[i], 0);
					voltChk.flag &= ~(1<<i);
				}
			}
		}

		/*开关按下上报服务器*/
		if((voltChk.flag & 0x07) == 0x07){/*开关按下一定是三相电压均低于50V,不会存在缺相bug*/
			upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose = true;

			for(i=0;i<3;i++){
				/*清欠压检测定时器*/
				TickOut((uint32 *)&itickLVContinuous[i], 0);
			}

			/*掉电清除过压,欠压标志*/
			pvca.pVol.flag &= ~(1<<i);
			l_Flag = false;/*低压标志*/
			h_Flag = false;/*高压标志*/
			hmi_PhaseVc.bits.hv = false;
			hmi_PhaseVc.bits.lv = false;			
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVFault = false;
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOverVWaring = false;
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseLV = false;
			/*短路之后(空开自动跳闸),清过流标志*/
			upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = false;
			upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO &= ~(1<<i); 			
		}else{
			/*重新合上空开之后检测,清短路标志*/
			if(upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose == true){
				/*清短路标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = false;
				/*清相短路标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort &= ~((uint16)((uint16)1<<i));		
				/*清硬件开关标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.hardSwitchClose = false;
				/*优化逻辑处理--升级未拿到分控复位状态信息强制置位*/
				if(oneWaringResetSetFlag == true){
					oneWaringResetSetFlag = false;
					upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.comRecoverFlag = true;
				}
			}
		}

		/*电流状态更新*/	
		for(i=0;i<3;i++){
			#if MaskOCur_ShortFun == 1/*清零--屏蔽过流检测*/
			if(cpvs.abcPC[i] >= /*150**/80*1.414){
				upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = true;
				/*置位--相短路标志*/
				upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort |= (uint16)((uint16)1<<i);
				/*清检测恢复定时器*/
				TickOut((uint32 *)&itickPC[i], 0);
			}else
			#endif
			if(cpvs.abcPC[i] >= 50*1.414){
				#if VerCtr == VerCtr_Normal
					#if CtrHardware == CtrHardware_Andriod/*注明:过流检测仅针对充电柜,充电架不再更新过流保护值*/
						#if MaskOCur_ShortFun == 1/*屏蔽过流,充电架充电柜均屏蔽过流检测--201014--赵耀辉现场需求*/
						pvca.pCur.flag |= (1<<i);
						hmi_PhaseVc.bits.oc = true;
						
						if(upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort == false){/*在短路条件下不检测过流*/
							/*过流*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = true;
							/*相电流*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO |= (uint16)((uint16)1<<i); 
							/*清短路标志*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = false;
							/*清相短路标志*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort &= ~((uint16)((uint16)1<<i));
						}
						/*clear--三相电压过流恢复标志*/
						phaseOCRecoverFlag = 0;
						#endif
					#endif
				#endif
				TickOut((uint32 *)&itickPC[i], 0);
			}else{
				if(cpvs.abcPC[i] <= 30*1.414 && TickOut((uint32 *)&itickPC[i], tickTimeBase_V) == true){/*回差和时间限制,防止抖动*/
					if(upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort == false){
						pvca.pCur.flag &= ~(1<<i);
						phaseOCRecoverFlag |= ((uint8)(1<<i));
						if(phaseOCRecoverFlag == 0x07){
							phaseOCRecoverFlag = 0;
							hmi_PhaseVc.bits.oc = false;
							/*过流恢复*/
							upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseOC = false;
						}
						/*相电流*/
						upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurO &= ~(1<<i); 
						/*清短路标志*/
						upperLayerPara.stateInfoChange.sysModuleStateInfo.bits.phaseCShort = false;	
						/*清相短路标志*/
						upperLayerPara.stateInfoChange.sysModuleStateInfo_2.bits.abcPhaseCurShort &= ~((uint16)((uint16)1<<i));	
					}
				}else{
					if(cpvs.abcPC[i] > 32*1.414){/*清定时器预留阈值2A*/
						TickOut((uint32 *)&itickPC[i], 0);/*新增:20210201--回复时间以恢复到执行标准之后开始计时*/
					}
				}
			}
		}	
	}
#endif
	return pvca;
}   

/*
** 相电压/相电流异常处理策略
** 		策略一:关闭所有充电器
*/
bool PhaseVCAbnCloseAllCharger(void){
	bool ret = false;
	PhaseVCAnalyze pvsa = get_PhaseVCAnalyze();

	if(pvsa.pVol.flag != 0 || pvsa.pCur.flag != 0){
		ret = true;
	}else{
		ret = false;
	}

	return ret;
}

