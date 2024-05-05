/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_pwr.h
* Author             : MCD Application Team
* Version            : V2.2.0
* Date               : 06/13/2008
* Description        : Connection/disconnection & power management header
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PWR_H
#define __USB_PWR_H
#include "usb_type.h"
#include "usb_core.h"

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _RESUME_STATE
{
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
} RESUME_STATE;


//1、接入态（Attached）：全 / 高速设备D + 引脚外接1.5k上拉电阻，低速设备D - 引脚外接1.5k上拉电阻，设备接入主机后，主机通过检测信号线上的电平变化来发现设备的接入，并获取设备速度；
//2、供电态（Powered）：就是给设备供电，分为设备接入时的默认供电值，配置阶段后的供电值（按数据中要求的最大值，可通过编程设置）
//3、缺省态（Default）：USB在被配置之前，通过缺省地址0与主机进行通信；
//4、地址态（Address）：经过了配置，USB设备被复位后，就可以按主机分配给它的唯一地址来与主机通信，这种状态就是地址态；
//5、配置态（Configured）：通过各种标准的USB请求命令来获取设备的各种信息，并对设备的某此信息进行改变或设置。
//6、挂起态（Suspended）：总线供电设备在3ms内没有总线动作，即USB总线处于空闲状态的话，该设备就要自动进入挂起状态，在进入挂起状态后，总的电流功耗不超过280UA。
typedef enum _DEVICE_STATE
{
	UNCONNECTED,	// 
	ATTACHED,		// 接入态
	POWERED,		// 供电态
	SUSPENDED,		// 挂起态
	ADDRESSED,		// 地址态
	CONFIGURED		// 配置态
} DEVICE_STATE;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Suspend (void);
void Resume_Init (void);
void Resume (RESUME_STATE eResumeSetVal);
RESULT PowerOn (void);
RESULT PowerOff (void);
/* External variables --------------------------------------------------------*/
extern  volatile uint32_t bDeviceState; /* USB device status */
extern volatile uint8_t fSuspendEnabled;  /* true when suspend is possible */

#endif  /*__USB_PWR_H*/

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
