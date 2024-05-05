#ifndef __SCCB_H
#define __SCCB_H

void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_SendByte(uint8_t Byte);
uint8_t SCCB_ReceiveByte(void);
void SCCB_SendNA(void);
uint8_t SCCB_ReceiveAck(void);

#endif