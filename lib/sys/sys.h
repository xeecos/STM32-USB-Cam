#ifndef __SYS_H
#define __SYS_H	  
#include "stm32f10x.h"   

void SetSysClockToHSE(void);
void MY_NVIC_PriorityGroupConfig(uint8_t NVIC_Group);  
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group);
#endif











