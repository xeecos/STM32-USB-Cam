/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.h
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : All processing related to Virtual COM Port Demo (Endpoint 0)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usb_prop_H
#define __usb_prop_H

/* Includes ------------------------------------------------------------------*/
#include "usb_type.h"
#include "usb_core.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint32_t bitrate;
	uint8_t format;
	uint8_t paritytype;
	uint8_t datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define UsbCamera_GetConfiguration          NOP_Process
//#define UsbCamera_SetConfiguration          NOP_Process
#define UsbCamera_GetInterface              NOP_Process
#define UsbCamera_SetInterface              NOP_Process
#define UsbCamera_GetStatus                 NOP_Process
#define UsbCamera_ClearFeature              NOP_Process
#define UsbCamera_SetEndPointFeature        NOP_Process
#define UsbCamera_SetDeviceFeature          NOP_Process
//#define UsbCamera_SetDeviceAddress          NOP_Process

#define GET_CUR                     0x81
#define SET_CUR                     0x01
#define SET_INTERFACE               0x0b
#define REPORT_DESCRIPTOR           0x22

/* Exported functions ------------------------------------------------------- */
void UsbCamera_init(void);
void UsbCamera_Reset(void);
void UsbCamera_SetConfiguration(void);
void UsbCamera_SetDeviceAddress(void);
void UsbCamera_Status_In(void);
void UsbCamera_Status_Out(void);
RESULT UsbCamera_Data_Setup(uint8_t);
RESULT UsbCamera_NoData_Setup(uint8_t);
RESULT UsbCamera_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *UsbCamera_GetDeviceDescriptor(uint16_t);
uint8_t *UsbCamera_GetConfigDescriptor(uint16_t);
uint8_t *UsbCamera_GetStringDescriptor(uint16_t);

uint8_t* VideoCommitControl_Command(uint16_t Length);
uint8_t* VideoProbeControl_Command(uint16_t Length);


#endif /* __usb_prop_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

