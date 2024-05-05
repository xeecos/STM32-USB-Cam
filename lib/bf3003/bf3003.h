#ifndef __BF3003_H
#define __BF3003_H

#define BF3003_PID_BME              0xfc
#define BF3003_VER_BME              0xfd
#define BF3003_VHREF                0x03
#define BF3003_OFFSET_4x_DOWN       0x04
#define BF3003_LOFFN1E              0x05
#define BF3003_LOFFN0E              0x06
#define BF3003_OFFSET_2X            0x07
#define BF3003_COM2                 0x09
#define BF3003_COM3                 0x0C
#define BF3003_CLKRC                0x11
#define BF3003_COM7                 0x12
#define BF3003_COM8                 0x13
#define BF3003_COM10                0x15
#define BF3003_HSTART               0x17
#define BF3003_HSTOP                0x18
#define BF3003_VSTART               0x19
#define BF3003_VSTOP                0x1A
#define BF3003_PLLCTL               0x1B
#define BF3003_AVER_LOCK1           0x1C
#define BF3003_AVER_LOCK2           0x1D
#define BF3003_MVFP                 0x1E
#define BF3003_DBLK_TARO            0x1F
#define BF3003_DBLK_TARE            0x22
#define BF3003_BLUE_GAIN            0x01
#define BF3003_RED_GAIN             0x02
#define BF3003_GREEN_GAIN           0x23
#define BF3003_GN_GAIN              0x6A
#define BF3003_BLUE_GAIN_LOW        0xA2
#define BF3003_BLUE_GAIN_HIGH       0xA3
#define BF3003_RED_GAIN_LOW         0xA4
#define BF3003_REDE_GAIN_HIGH       0xA5
#define BF3003_GLB_GAIN             0x87
#define BF3003_GLB_GAIN_MIN         0x82
#define BF3003_GLB_GAIN_MAX         0x86
#define BF3003_STEPO                0x27
#define BF3003_DBLK_CNTL            0x28
#define BF3003_EXHCH                0x2A
#define BF3003_EXHCL                0x2B
#define BF3003_HREF_CNTL            0xE2
#define BF3003_DM_LNL               0x92
#define BF3003_DM_LNH               0x93
#define BF3003_DM_ROWL              0xE4
#define BF3003_DM_ROWH              0xE3
#define BF3003_TSLB                 0x3A
#define BF3003_AE_MODE              0x80
#define BF3003_AE_SPEED             0x81
#define BF3003_TEST_MODE            0xB9
#define BF3003_MODE_SEL             0xf0
#define BF3003_SUBSAMPLE            0x4a
#define BF3003_DICOM1               0x69
#define BF3003_INT_MEAN_H           0X89
#define BF3003_INT_MEAN_L           0X8A
#define BF3003_INT_TIM_MIN          0X8B
#define BF3003_INT_TIM_HI           0X8C
#define BF3003_INT_TIM_LO           0X8D
#define BF3003_INT_TIM_MAX_HI       0X8E
#define BF3003_INT_TIM_MAX_LO       0X8F
#define BF3003_LINE_CTR             0xF5 
void BF3003_Init(void);
void BF3003_Handle(void);
void BF3003_Start();
void BF3003_Stop();
void BF3003_SetWindow(uint16_t x,uint16_t y,uint16_t w,uint16_t h);
void BF3003_SetMode(uint8_t gain, uint8_t whitebalance, uint8_t exposure);
void BF3003_SetExposure(uint16_t exposure);
void BF3003_SetGain(uint8_t r,uint8_t g,uint8_t b);
void BF3003_SetDummy(uint16_t dummy);
void BF3003_SetFrequency(uint16_t freqDiv, uint16_t skipDiv);
void _BF3003_SetFrequency(uint16_t freqDiv);
void _BF3003_UpdateFrequency(uint16_t freqDiv);
void BF3003_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t BF3003_ReadReg(uint8_t RegAddress);
void BF3003_LineBegin();
void BF3003_FrameBegin();
void BF3003_ReadPixel();
#endif