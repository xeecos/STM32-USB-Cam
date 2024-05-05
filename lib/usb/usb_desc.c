/******************** xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx ********************
* File Name          : usb_desc.c
* Author             :
* Version            :
* Date               :
* Description        : Usb Camera Descriptors
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t Camera_DeviceDescriptor[CAMERA_SIZ_DEVICE_DESC] =
{
	CAMERA_SIZ_DEVICE_DESC,					/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,				/* bDescriptorType */
	0x00, 0x02,								/* bcdUSB 2.10 */
	0x00,									/* bDeviceClass */
	0x00,									/* bDeviceSubClass */
	0x01,									/* bDeviceProtocol */
	0x40,									/* bMaxPacketSize 40 */
	0xB1, 0x20,								/* idVendor = 0x20B1*/
	0xE0, 0x1D,								/* idProduct  = 0x1DE0*/
	0x00, 0x02,								/* bcdDevice */
	1,										/* iManufacturer */
	2,										/* iProduct */
	0,										/* iSerialNumber */
	0x01									/* bNumConfigurations */
};

const uint8_t Camera_ConfigDescriptor[CAMERA_SIZ_CONFIG_DESC] =
{
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,      /* bDescriptorType: Configuration */
    CAMERA_SIZ_CONFIG_DESC,                /* wTotalLength:no of returned bytes */
    0x00,
    0x02,   /* bNumInterfaces: 1 interface for Game IO */
    0x01,   /* bConfigurationValue: Configuration value */
    USBD_IDX_CONFIG_STR,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xE0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 0 mA */
    
    /*---------------------------------------------------------------------------*/
    
    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface, zero based index of this interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: Two endpoints used */
    0x00,   /* bInterfaceClass: vendor */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */
    
    /*Endpoint IN Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType: Endpoint */
    0x81,                               /* bEndpointAddress */
    0x02,                               /* bmAttributes: Bulk */
    0x40,                               /* wMaxPacketSize: */
    0x00,
    0x00,                                /* bInterval: ignore for Bulk transfer */

    
    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x01,   /* bInterfaceNumber: Number of Interface, zero based index of this interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: Two endpoints used */
    0x00,   /* bInterfaceClass: vendor */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */

    /*Endpoint OUT Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType: Endpoint */
    0x02,                               /* bEndpointAddress */
    0x02,                               /* bmAttributes: Control 0x0 Isochronous 0x1 Bulk 0x2 Interrupt 0x3*/
    0x40,                               /* wMaxPacketSize: */
    0x00,
    0x00,                                /* bInterval: ignore for Bulk transfer */
    
};
/* USB String Descriptors */
const uint8_t Camera_StringLangID[CAMERA_SIZ_STRING_LANGID] =
{
	CAMERA_SIZ_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04 /* LangID = 0x0409: U.S. English */
};

const uint8_t Camera_StringVendor[CAMERA_SIZ_STRING_VENDOR] =
{
	CAMERA_SIZ_STRING_VENDOR,				/* Size of Vendor string */
	USB_STRING_DESCRIPTOR_TYPE,				/* bDescriptorType*/
	/* Manufacturer:  */
	'x', 0, 
	'e', 0, 
	'e', 0, 
	'c', 0, 
	'o', 0, 
	's', 0
};

const uint8_t Camera_StringProduct[CAMERA_SIZ_STRING_PRODUCT] =
{
	CAMERA_SIZ_STRING_PRODUCT,				/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,				/* bDescriptorType */
	/* Product name:  */
	'E', 0,
	'z', 0,
	'C', 0,
	'a', 0,
	'm', 0,
	' ', 0,
	'C', 0,
	'a', 0,
	'm', 0,
	'e', 0,
	'r', 0,
	'a', 0,

}; 

uint8_t Camera_StringSerial[CAMERA_SIZ_STRING_SERIAL] =
{
	CAMERA_SIZ_STRING_SERIAL,				/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,				/* bDescriptorType */
	'1', 0,
	'.', 0,
	'1', 0
};
#define USB_REQ_MS_VENDOR_CODE	 	0xA0
const uint8_t USBD_OS_STRING[8] = { 
   'M',
   'S',
   'F',
   'T',
   '1',
   '0',
   '0',
   USB_REQ_MS_VENDOR_CODE, 
}; 

const uint8_t USBD_WINUSB_OSFeatureDesc[USB_LEN_OS_FEATURE_DESC] =
{
	0x28, 0, 0, 0, // length
	0, 1,          // bcd version 1.0
	4, 0,          // windex: extended compat ID descritor
	1,             // no of function
	0, 0, 0, 0, 0, 0, 0, // reserve 7 bytes
	// function
	0,             // interface no
	0,             // reserved
	'W', 'I', 'N', 'U', 'S', 'B', 0, 0, //  first ID
    0, 0, 0, 0, 0, 0, 0, 0,  // second ID
	0, 0, 0, 0, 0, 0 // reserved 6 bytes      
};
const uint8_t USBD_WINUSB_OSPropertyDesc[USB_LEN_OS_PROPERTY_DESC] =
{
      0x8E, 0, 0, 0,  // length 246 byte
      0x00, 0x01,   // BCD version 1.0
      0x05, 0x00,   // Extended Property Descriptor Index(5)
      0x01, 0x00,   // number of section (1)
//; property section        
      0x84, 0x00, 0x00, 0x00,   // size of property section
      0x1, 0, 0, 0,   //; property data type (1)
      0x28, 0,        //; property name length (42)
      'D', 0,
      'e', 0,
      'v', 0,
      'i', 0,
      'c', 0,
      'e', 0,
      'I', 0,
      'n', 0,
      't', 0,
      'e', 0,
      'r', 0,
      'f', 0,
      'a', 0,
      'c', 0,
      'e', 0,
      'G', 0,
      'U', 0,
      'I', 0,
      'D', 0,
      0, 0,
      // D6805E56-0447-4049-9848-46D6B2AC5D28
      0x4E, 0, 0, 0,  // ; property data length
      '{', 0,
      '1', 0,
      '3', 0,
      'E', 0,
      'B', 0,
      '3', 0,
      '6', 0,
      '0', 0,
      'B', 0,
      '-', 0,
      'B', 0,
      'C', 0,
      '1', 0,
      'E', 0,
      '-', 0,
      '4', 0,
      '6', 0,
      'C', 0,
      'B', 0,
      '-', 0,
      'A', 0,
      'C', 0,
      '8', 0,
      'B', 0,
      '-', 0,
      'E', 0,
      'F', 0,
      '3', 0,
      'D', 0,
      'A', 0,
      '4', 0,
      '7', 0,
      'B', 0,
      '4', 0,
      '0', 0,
      '6', 0,
      '2', 0,
      '}', 0,
      0, 0,
};
/************************END OF FILE***************************************/
