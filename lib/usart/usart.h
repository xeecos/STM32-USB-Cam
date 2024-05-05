#ifndef __USART_H
#define __USART_H
#include <stdint.h>
#include "stdio.h"	 
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			64  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART_RX_STA;         		//接收状态标记	
void Usart_Init(uint32_t bound);
#endif	   
















