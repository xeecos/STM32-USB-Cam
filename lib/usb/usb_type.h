/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_type.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Type definitions used by the USB Library
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TYPE_H
#define __USB_TYPE_H

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __STM32F10x_H

// typedef enum
// {
// 	FALSE = 0, TRUE = !FALSE
// }
// bool;
// typedef enum { RESET = 0, SET = !RESET } FlagStatus, ITStatus;

// typedef enum { DISABLE = 0, ENABLE = !DISABLE } FunctionalState;

// typedef enum { ERROR = 0, SUCCESS = !ERROR } ErrorStatus;
#endif

#define USB_TRUE   		1
#define USB_FALSE  		0




/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

#endif /* __USB_TYPE_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
