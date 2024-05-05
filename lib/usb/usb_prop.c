/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : All processing related to Virtual Com Port Demo
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct  _VideoControl
{
	uint8_t    bmHint[2];                      // 0x02
	uint8_t    bFormatIndex[1];                // 0x03
	uint8_t    bFrameIndex[1];                 // 0x04
	uint8_t    dwFrameInterval[4];             // 0x08
	uint8_t    wKeyFrameRate[2];               // 0x0A
	uint8_t    wPFrameRate[2];                 // 0x0C
	uint8_t    wCompQuality[2];                // 0x0E
	uint8_t    wCompWindowSize[2];             // 0x10
	uint8_t    wDelay[2];                      // 0x12
	uint8_t    dwMaxVideoFrameSize[4];         // 0x16
	uint8_t    dwMaxPayloadTransferSize[4];    // 0x1A
	uint8_t    dwClockFrequency[4];            // 0x1C
	uint8_t    bmFramingInfo[1];
	uint8_t    bPreferedVersion[1];
	uint8_t    bMinVersion[1];
	uint8_t    bMaxVersion[1];
}   VideoControl;

//VideoStreaming Requests应答，参考USB_Video_Class_1_1.pdf p103~
VideoControl    videoCommitControl =
{
	{ 0x01, 0x00 },                      // bmHint
	{ 0x01 },                           // bFormatIndex
	{ 0x01 },                           // bFrameIndex
	{ MAKE_DWORD(FRAME_INTERVEL) },		// dwFrameInterval
	{ 0x00, 0x00, },                     // wKeyFrameRate
	{ 0x00, 0x00, },                     // wPFrameRate
	{ 0x00, 0x00, },                     // wCompQuality
	{ 0x00, 0x00, },                     // wCompWindowSize
	{ 0x00, 0x00 },                      // wDelay
	{ MAKE_DWORD(MAX_FRAME_SIZE) },     // dwMaxVideoFrameSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwMaxPayloadTransferSize
	{ 0x80, 0x8d, 0x5b, 0x00 },         // dwClockFrequency
	{ 0x00 },                           // bmFramingInfo
	{ 0x00 },                           // bPreferedVersion
	{ 0x00 },                           // bMinVersion
	{ 0x00 },                           // bMaxVersion
};

VideoControl    videoProbeControl =
{
	{ 0x01, 0x00 },                      // bmHint
	{ 0x01 },                           // bFormatIndex
	{ 0x01 },                           // bFrameIndex
	{ MAKE_DWORD(FRAME_INTERVEL) },          // dwFrameInterval
	{ 0x00, 0x00, },                     // wKeyFrameRate
	{ 0x00, 0x00, },                     // wPFrameRate
	{ 0x00, 0x00, },                     // wCompQuality
	{ 0x00, 0x00, },                     // wCompWindowSize
	{ 0x00, 0x00 },                      // wDelay
	{ MAKE_DWORD(MAX_FRAME_SIZE) },    // dwMaxVideoFrameSize
	{ 0x00, 0x00, 0x00, 0x00 },         // dwMaxPayloadTransferSize
	{ 0x80, 0x8d, 0x5b, 0x00 },         // dwClockFrequency
	{ 0x00 },                           // bmFramingInfo
	{ 0x00 },                           // bPreferedVersion
	{ 0x00 },                           // bMinVersion
	{ 0x00 },                           // bMaxVersion
};

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
	UsbCamera_init,
	UsbCamera_Reset,
	UsbCamera_Status_In,
	UsbCamera_Status_Out,
	UsbCamera_Data_Setup,
	UsbCamera_NoData_Setup,
	UsbCamera_Get_Interface_Setting,
	UsbCamera_GetDeviceDescriptor,
	UsbCamera_GetConfigDescriptor,
	UsbCamera_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
	UsbCamera_GetConfiguration,
	UsbCamera_SetConfiguration,
	UsbCamera_GetInterface,
	UsbCamera_SetInterface,
	UsbCamera_GetStatus,
	UsbCamera_ClearFeature,
	UsbCamera_SetEndPointFeature,
	UsbCamera_SetDeviceFeature,
	UsbCamera_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(uint8_t*)Camera_DeviceDescriptor,
	CAMERA_SIZ_DEVICE_DESC
};
ONE_DESCRIPTOR Config_Descriptor =
{
	(uint8_t*)Camera_ConfigDescriptor,
	CAMERA_SIZ_CONFIG_DESC
};
ONE_DESCRIPTOR String_Descriptor[4] =
{
	{ (uint8_t*)Camera_StringLangID, CAMERA_SIZ_STRING_LANGID },
	{ (uint8_t*)Camera_StringVendor, CAMERA_SIZ_STRING_VENDOR },
	{ (uint8_t*)Camera_StringProduct, CAMERA_SIZ_STRING_PRODUCT },
	{ (uint8_t*)Camera_StringSerial, CAMERA_SIZ_STRING_SERIAL }
};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  :
* Description    : init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_init(void)
{

	/* Update the serial number string descriptor with the data from the unique
	ID*/
	// Get_SerialNum();

	pInformation->Current_Configuration = 0;

	/* Connect the device */
	PowerOn();
	/* USB interrupts initialization */
	wInterrupt_Mask = IMR_MSK;
	/* set interrupts mask */
	_SetCNTR(wInterrupt_Mask);

	bDeviceState = UNCONNECTED;
	printf("USB INIT\n");
}

/*******************************************************************************
* Function Name  :
* Description    : reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Reset(void)
{
	printf("USB Reset\n");
	/* Set Usb device as not configured state */
	pInformation->Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = Camera_ConfigDescriptor[7];
	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_NAK);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPRxCount(ENDP0, PACKET_SIZE);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_BULK);
	#ifdef USB_DBUF_ENABLE
	SetEPDoubleBuff(ENDP1);
	SetEPDblBuffAddr(ENDP1, ENDP1_BUF0Addr, ENDP1_BUF1Addr);
	SetEPDblBuffCount(ENDP1, EP_DBUF_OUT, PACKET_SIZE);
	ClearDTOG_RX(ENDP1);
	ClearDTOG_TX(ENDP1);
    ToggleDTOG_RX(ENDP1);
	#else
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPTxCount(ENDP1, PACKET_SIZE);
	#endif
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	SetEPTxStatus(ENDP1, EP_TX_VALID);

	/* Initialize Endpoint 2 */
	SetEPType(ENDP2, EP_BULK);
	SetEPRxAddr(ENDP2, ENDP2_RXADDR);
	SetEPRxCount(ENDP2, PACKET_SIZE);
    SetEPTxStatus(ENDP2, EP_TX_DIS);
	SetEPRxValid(ENDP2);

	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;
}


/*******************************************************************************
* Function Name  :
* Description    : Udpade the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
}

/*******************************************************************************
* Function Name  :
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  :
* Description    : Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Status_In(void)
{
}

/*******************************************************************************
* Function Name  :
* Description    : Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void UsbCamera_Status_Out(void)
{
}

/*******************************************************************************
* Function Name  :
* Description    : handle the data class specific requests
*              对Class-specific Requests的应答
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT UsbCamera_Data_Setup(uint8_t RequestNo)
{
	uint8_t *(*CopyRoutine)(uint16_t);
	CopyRoutine = NULL;

	if ((RequestNo == GET_CUR) || (RequestNo == SET_CUR))
	{
		if (pInformation->USBwIndex == 0x0100)
		{
			if (pInformation->USBwValue == 0x0001)
			{
				// Probe Control
				CopyRoutine = VideoProbeControl_Command;
			} else if (pInformation->USBwValue == 0x0002)
			{
				// Commit control
				CopyRoutine = VideoCommitControl_Command;
			}
		}
	} else
	{
		return USB_UNSUPPORT;
	}
	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}



/*******************************************************************************
* Function Name  :
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT UsbCamera_NoData_Setup(uint8_t RequestNo)
{
	return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  :
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *UsbCamera_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  :
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *UsbCamera_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  :
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *UsbCamera_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 > 4)
	{
		if(wValue0==0xEE)
		{
			return Standard_GetDescriptorData(Length, &USBD_OS_STRING);
		}
		return 0;
	} else
	{
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*******************************************************************************
* Function Name  :
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT UsbCamera_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 1)
	{
		return USB_UNSUPPORT;
	} else if (Interface > 1)
	{
		return USB_UNSUPPORT;
	}
	return USB_SUCCESS;
}


/*******************************************************************************
* Function Name  :
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
uint8_t* VideoProbeControl_Command(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
		return NULL;
	} else
	{
		return((uint8_t*)(&videoProbeControl));
	}
}

/*******************************************************************************
* Function Name  :
* Description    :
* Input          :
* Output         :
* Return         :
*******************************************************************************/
uint8_t* VideoCommitControl_Command(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
		return NULL;
	} else
	{
		return((uint8_t*)(&videoCommitControl));
	}
}




/******************* (C) COPYRIGHT 2011 xxxxxxxxxxxxxxx *****END OF FILE****/

