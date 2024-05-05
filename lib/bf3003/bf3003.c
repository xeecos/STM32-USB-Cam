#include "stm32f10x.h" // Device header
// #include "stm32f1xx_hal.h"
#include "sccb.h"
#include "delay.h"
#include "usart.h"
#include "bf3003.h"
#include "hw_config.h"

/* 0xDC --- 写地址   ***   0XDD ---读地址 */
#define BF3003_ADDRESS      	    0xDC

#define BF3003_PID_BME              0xFC
#define BF3003_VER_BME              0xFD

#define REGS_COUNT					44
#define MAX_BUFFER_COUNT            2
uint16_t frameWidth = 640;
uint16_t frameHeight = 480;
uint8_t frame[MAX_BUFFER_COUNT][640];
uint16_t pixelIdx = 0;
uint32_t totalCount = 0;
uint16_t totalLines = 0;
int16_t lineIdx = 0;
uint16_t skipFreq = 15;
uint16_t baseFreq = 15;
uint16_t currentFreq = 0;
uint8_t regs[REGS_COUNT][2] = {
	{BF3003_COM7, 0b10000000},
	{BF3003_COM2, 0b11001111},
	/*
	Common control 2
	Bit[7:6]: vclk output drive capability
		00:1x 01:1.5x 10:2.5x 11:3x
	Bit[5]:Tri-state option for output data at power down period
		0:tri-state at this period
		1:No tri-state at this period
	Bit[4]:Tri-state option for output clock at power down period
		0:tri-state at this period
		1:No tri-state at this period
	Bit[3:2]: hsync output drive capability
		00:1x 01:1.5x 10:2.5x 11:3x
	when drivesel=0
	Bit[1:0]: data&clk&Hsync output drive capability
		00:1x 01:1.5x 10:2.5x 11:3x
	when drivesel=1
		Bit[1:0]: data output drive capability
		00:1x 01:1.5x 10:2.5x 11:3x
	*/
	{BF3003_COM3, 0b00000000},
	/*
	Bit[7]:PROCRSS RAW selection
		0: process raw from ycbcr to rgb conversion in datformat
		1: process raw from color interpolation(deniose,gamma,lsc is selectable)
	Bit[6]:Output data MSB and LSB swap
	Bit[5:4]:PROCESS RAW sequence(when 0x0c[7]=0):
		00: (LINE0:BGBG/LINE1:GRGR)
		01: (LINE0:GBGB/LINE1:RGRG)
		10: (LINE0:GRGR/LINE1:BGBG)
		11: (LINE0:RGRG/LINE1:GBGB)
	Bit[3]:
		0:no HREF when VSYNC_DAT=0;
		1:always has HREF no matter VSYNC_DAT=0 or not;
	Bit[2]:DATA ahead 1 clk(YUV MCLK,RawData PCLK) or not
	Bit[1]:HREF ahead 1 clk(YUV MCLK,RawData PCLK) or not
	Bit[0]:HREF ahead 0.5 clk(YUV MCLK,RawData PCLK) or not
	0x0c[1:0]: Internal use only
	*/
	{BF3003_CLKRC, 0b1001},
	/*
	Mclk_div control
	Bit[7：2]: Internal use only
	Bit[1:0]:Internal MCLK pre-scalar
		00:divided by 1 F(MCLK)=F(pll output clock)
		01:divided by 2 F(MCLK)=F(pll output clock)/2
		10:divided by 4 F(MCLK)=F(pll output clock)/4
		11: no clocking, digital stand by mode(all clocks freeze)
	*/
	{BF3003_COM7, 0b00000001},
	/*
		Bit[7]: SCCB Register Reset
			0: No change
			1: Resets all registers to default values 
		Bit[6]: Reserved
		Bit[5]: (when 0x4a =03h)0: row 1/2 sub,1: output input image.
		Bit[4]: 1/2 digital subsample Selection(only for YUV422/RGB565/RGB555/RGB444 output).
		Bit[3]: data selection
			0:normal(YUV422/RGB565/RGB555/RGB444/BAYER RAW/PRO RAW)
			1:CCIR656 output enable(for TV)
		Bit[2]: YUV422/RGB565/RGB555/RGB444 Selection.
		Bit[1]: Reserved.
		Bit[0]: Raw RGB Selection.
			{0x12[2],0x12[0]}
			00: YUV422
			01: Bayer RAW
			10: RGB565/RGB555/RGB444(use with 0x3a) 
			11: Process RAW (use with 0x0c[7]) 
 	*/
	{BF3003_TSLB, 0b00000001},
	/*
		if YUV422 is selected,the Sequence is:
		Bit[1:0]:Output YUV422 Sequence
		00: YUYV, 01: YVYU
		10: UYVY, 11: VYUY
		if RGB565/RGB555/RGB444 is selected,the Sequence is:
		Bit[4:0]:Output RGB565/RGB555/RGB444 Sequence
		RGB565:
		00h: R5G3H,G3LB5 01h: B5G3H,G3LR5
		02h: B5R3H,R2LG6 03h: R5B3H,B2LG6
		04h: G3HB5,R5G3L 05h: G3LB5,R5G3H
		06h: G3HR5,B5G3L 07h: G3LR5,B5G3H
		08h: G6B2H,B3LR5 09h: G6R2H,R3LB5
	*/
	{BF3003_COM8, 0b00010000},
	/*
		Auto mode Contrl
		Bit[7:6] reserved
		Bit[5:4]：Sensitivity enable,
		Bit[5]: 0:manual adjust , 1 :auto adjust
		Bit[4]: when manual adjust,write 1, high sensitivity
		write 0, low sensitivity
		Bit[4]: select which gain to be used,when short int_tim
		adjust:
		0: use glb_gain_short
		1: use glb_gain
		Bit[3]: Reserved.
		Bit[2]: AGC Enable. 0:OFF , 1: ON.
		Bit[1]: AWB Enable. 0:OFF , 1: ON.
		Bit[0]: AEC Enable. 0:OFF , 1: ON. 
	*/
	{BF3003_COM10, 0b00000010},
	/* BF3003_COM10
		Bit[7]: Reserved
		Bit[6]: 0:HREF, 1:HSYNC
		Bit[5]: 0:VSYNC_IMAGE, 1:VSYNC_DAT
		Bit[4]: VCLK reverse
		Bit[3]: HREF option, 0:active high, 1:active low.
		Bit[2]: Reserved
		Bit[1]: VSYNC option, 0:active low, 1:active high.
		Bit[0]: HSYNC option, 0:active high, 1:active low.
	*/
	{BF3003_VHREF, 0b0100},
	{BF3003_HSTART, 0x1},
	{BF3003_HSTOP, 0xA0},
	{BF3003_VSTART, 0x0},
	{BF3003_VSTOP, 0x78},
	{BF3003_PLLCTL, 0b00101010},
	/*
	PLLCTL[7]: PLL Enable
		0:enable
		1:disable
	PLLCTL[6:0]: Reserved
	*/
	{BF3003_HREF_CNTL,0b111},
	/*
	HREF_CNTL[2:0]: 000:delay third,delay two pclk;
		001:delay fourth,delay three pclk;
		010:delay fifth,delay four pclk;
		011:delay sixth,delay five pclk;
		100:delay seventh,delay six pclk;
		101:delay eighth,delay seven pclk;
		110:delay ninth,delay eight pclk;
		111:delay tenth,delay nine pclk;
	*/
	{BF3003_EXHCH,0x00},
	{BF3003_EXHCL,0x80},
	/*
	Dummy Pixel Insert MSB
		Bit[7:4]: 4MSB for dummy pixel insert in horizontal direction
	Dummy Pixel Insert LSB
		8 LSB for dummy pixel insert in horizontal direction
	*/
	{BF3003_DM_ROWH, 0x00},
	/*
		Dummy line insert before active line low 8 bits 
	*/
	{BF3003_DM_ROWL, 0x00},
	/*
		Dummy line insert before active line high 8 bits 
	*/
	{BF3003_DM_LNL, 0x00},
	/*
		insert the dummy line after active line(Dummy line low 8bits)
		it's default value is 0x28;
	*/
	{BF3003_DM_LNH, 0x00},
	/*
		insert the dummy line after active line(Dummy line high 8bits) 
	*/
	{BF3003_AE_MODE, 0b11000000},
	/*
	Bit[7]: AE mode select:
		0: use Y (from color space module).
		1: use rawdata (from gamma module), (when special effect in color interpolation module is selected,0x80[7] must set
	to be 1'b1)
	Bit[6]: INT_TIM lower than INT_STEP_5060 or not:
		0: limit int_tim>=step(no flicker)
		1: int_tim can be less than 1*int_step(existing flicker).
	Bit[5:4]: center window select:
	vga and ntsc mode 00: 512*384(full) ,256*192(1/2sub when normal mode)
	and ntsc have no sub 
		01: 384*288(full) ,192*144(1/2sub when normal mode)
		10 : 288*216(full) ,144*108(1/2sub when normal mode)
		11: 216*160(full) ,108*80 (1/2sub when normal mode)
	pal mode 00: 512*448
		01: 384*336
		10: 288*256
		11: 216*192
	Bit[3:1]: weight select: weight_sel region1 region2 region3 region4
		000: 1/4 1/4 1/4 1/4
		001: 1/2 1/4 1/8 1/8
		010: 5/8 1/8 1/8 1/8
		011: 3/8 3/8 1/8 1/8
		100: 3/4 1/4 0 0
		101: 5/8 3/8 0 0
		110: 1/2 1/2 0 0
		111: 1 0 0 0
	Bit[0]: Banding filter value select
		0: Select {0x89[5],0x9E[7:0]} as Banding Filter Value.
		1: Select {0x89[4],0x9D[7:0]} as Banding Filter Value
	*/
	{BF3003_TEST_MODE, 0b00000000},
	/*
	BIT[7] : 
		1: test pattern enable
		0: bypass test pattern
	BIT[6:5]: 
		00: output color bar pattern
		01: output gradual pattern
		1x: output manual write R/G/B
	BIT[4] : 0:vertical pattern, 1:horizontal pattern
	BIT[3:0]: gradual gray pattern mode control 
	*/
	{BF3003_MODE_SEL, 0b0},
	{BF3003_SUBSAMPLE, 0b0},
	{BF3003_BLUE_GAIN, 0x1},
	{BF3003_RED_GAIN, 0x1},
	{BF3003_GREEN_GAIN, 0x1},
	{BF3003_DICOM1, 0x80},
	/*Bit[7]: YCBCR RANGE select
		0: YCBCR 0~255
		1: Y 16~235, CBCR 16~240
	Bit[6]: Negative image enable
		0: Normal image, 1: Negative image
	Bit[5]: UV output value select.
		0: output normal value
		1: output fixed value set in MANU and MANV
	Bit[4]:U、V dither when ycbcr mode/R、B dither when rgb
	mode:
		0: low 2 bits, 1: low 3bits
	Bit[3]:Y dither when ycbcr mode/G dither when rgb mode:
		0: low 2 bits, 1: low 3bits
	Bit[2]:Y dither enable
	Bit[1]:U、V dither enable
	Bit[0]:RGB dither enable 
	*/
	{BF3003_INT_MEAN_H,  0x32},
	{BF3003_INT_MEAN_L,  0xAA},
	{BF3003_INT_TIM_MIN, 0x00},
	{BF3003_INT_TIM_HI,  0x00},
	{BF3003_INT_TIM_LO,  0x00},
	{BF3003_INT_TIM_MAX_HI, 0xFF},
	{BF3003_INT_TIM_MAX_LO, 0xFF},
	{BF3003_LINE_CTR, 0x1},
	{BF3003_GLB_GAIN_MIN, 0x00},
	{BF3003_GLB_GAIN_MAX, 0xFF},
	{BF3003_GLB_GAIN, 0x10},
	{0, 0},
};
/*
 * @brief    BF3003引脚初始化函数，在BF3003_Init() 里触发，不需外部调用
 * @param  无
 * @retval 无
 */


EXTI_InitTypeDef   EXTI_InitStructurePCLK;
EXTI_InitTypeDef   EXTI_InitStructureHREF;
EXTI_InitTypeDef   EXTI_InitStructureVSYNC;
void BF3003_Pin_Init()
{
    GPIO_InitTypeDef GPIO_InitStruct;  
    /* XCLK初始化 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* PIXCLK VSYNC HREF 初始化 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
    

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);//pclk
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);//vsync
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7);//href

	EXTI_InitStructureVSYNC.EXTI_Line = EXTI_Line6;//vsync
	EXTI_InitStructureVSYNC.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructureVSYNC.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_InitStructureHREF.EXTI_Line = EXTI_Line7;//href
	EXTI_InitStructureHREF.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructureHREF.EXTI_Trigger = EXTI_Trigger_Rising;

	// NVIC_InitTypeDef nvic;
    // nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
    // nvic.NVIC_IRQChannelPreemptionPriority = 0;
    // nvic.NVIC_IRQChannelSubPriority = 0;
    // nvic.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&nvic); 	 
	MY_NVIC_Init(0, 0, EXTI9_5_IRQn, 2);
    /* D0-D7 IO口初始化 */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

	TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	#ifdef DMA_ENABLE
	DMA_InitTypeDef    DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)((uint8_t*)(&GPIOB->IDR)+1);// 设置发送缓冲区首地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)frame[0];         
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 // 设置外设寄存器 -> 内存缓冲区
    DMA_InitStructure.DMA_BufferSize = 640;                     		// 需要发送的字节数，这里其实可以设置为0，因为在实际要发送的时候，会重新设置次值
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不做增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存缓冲区地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位，1个字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位，1个字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                 // 优先级设置
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;         
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);    
	#else
	EXTI_InitStructurePCLK.EXTI_Line = EXTI_Line5;//pclk
	EXTI_InitStructurePCLK.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructurePCLK.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructurePCLK.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructurePCLK);
	EXTI_InitStructurePCLK.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructurePCLK);       
	#endif
}
/*
 * @brief    BF3003写寄存器
 * @param  写入寄存器的地址
 * @param  写入的内容（一个字节数据）
 * @retval 无
 */
void BF3003_WriteReg(uint8_t RegAddress, uint8_t Data)
{
    SCCB_Start();
    SCCB_SendByte(BF3003_ADDRESS);
    SCCB_ReceiveAck(); 

    SCCB_SendByte(RegAddress);
    SCCB_ReceiveAck();

    SCCB_SendByte(Data);
    SCCB_ReceiveAck();

    SCCB_Stop();
}
/*
 * @brief    BF3003读取寄存器
 * @param  读取寄存器的地址
 * @retval 相应地址的寄存器的数据
 */
uint8_t BF3003_ReadReg(uint8_t RegAddress)
{
    uint8_t Data;

    SCCB_Start();
    SCCB_SendByte(BF3003_ADDRESS);
    SCCB_ReceiveAck(); 
    SCCB_SendByte(RegAddress);
    SCCB_ReceiveAck();
    SCCB_Stop(); 

    SCCB_Start();
    SCCB_SendByte(BF3003_ADDRESS | 0x01);
    SCCB_ReceiveAck();
    Data = SCCB_ReceiveByte();
    SCCB_SendNA();
    SCCB_Stop();

    return Data;
}

/*
 * @brief    寄存器初始化
 */
void BF3003_Configure(void)
{
    SCCB_Init();        // SCCB初始化
    uint8_t ver = 0;
	while (1)
	{
		ver = BF3003_ReadReg(BF3003_PID_BME);
		printf("BF3003 Probe:%x\n", ver);
		if (ver == 0x30)
		{
			break;
		}
		Delay_Ms(100);
	}
	for (int i = 0; i < REGS_COUNT; i++)
	{
		if (regs[i][0] == 0)
			break;
		BF3003_WriteReg(regs[i][0], regs[i][1]);
		Delay_Ms(1);
	}
}
/*
 * @brief    BF3003初始化
 * @param  无
 * @retval 无
 */
void BF3003_Init(void)
{
    BF3003_Pin_Init();  // 引脚初始化
    BF3003_Configure(); // 寄存器预设
}

void BF3003_Handle(void)
{
	
}
void BF3003_Start()
{
	printf("bf3003 start\n");
	EXTI_InitStructureVSYNC.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructureVSYNC);
	EXTI_InitStructureHREF.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructureHREF);
	_BF3003_SetFrequency(baseFreq);
}
void BF3003_Stop()
{
	EXTI_InitStructureVSYNC.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructureVSYNC);
	EXTI_InitStructureHREF.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructureHREF);
	
}
int bufIdx = 0;
void BF3003_FrameBegin()
{
	// printf("frame:%d\n",lineIdx);
	totalLines = lineIdx;
	if(frameWidth<640)_BF3003_SetFrequency(skipFreq);
	lineIdx = 0;
	pixelIdx = 0;
	bufIdx = MAX_BUFFER_COUNT-1;
}
void BF3003_LineBegin()
{
	pixelIdx = 0;
	bufIdx++;
	if(bufIdx>MAX_BUFFER_COUNT-1)bufIdx=0;
	#ifdef DMA_ENABLE
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	// __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	DMA1_Channel2->CCR &= ~DMA_CCR1_EN;
	DMA1_Channel2->CMAR = (uint32_t)frame[bufIdx]; 
	DMA1_Channel2->CNDTR = frameWidth;
	DMA1_Channel2->CCR |= DMA_CCR1_EN;
	#else
	uint32_t tmp = (uint32_t)EXTI_BASE;
    *(__IO uint32_t *) tmp |= EXTI_Line5;
	_BF3003_SetFrequency(baseFreq);
	#endif
	lineIdx++;
}
void BF3003_ReadPixel()
{
	frame[bufIdx][pixelIdx] = GPIOB->IDR>>8;//(pixelIdx>>5)<<3;//
	pixelIdx++;
	if(pixelIdx>frameWidth-1)
	{
		uint32_t tmp = (uint32_t)EXTI_BASE;
		*(__IO uint32_t *) tmp &= ~EXTI_Line5;
		pixelIdx = 0;
		_BF3003_SetFrequency(skipFreq);
	}
}

void BF3003_SetWindow(uint16_t x,uint16_t y,uint16_t w,uint16_t h)
{
	printf("set window:%d, %d, %d, %d\n",x, y, w, h);
	BF3003_WriteReg(BF3003_VHREF, (((x+w)&0b11)<<6)+(((x)&0b11)<<4)+(((y+h)&0b11)<<2)+(y&0b11));
	BF3003_WriteReg(BF3003_HSTART, x>>2);
	BF3003_WriteReg(BF3003_HSTOP, (x+w)>>2);
	BF3003_WriteReg(BF3003_VSTART, y>>2);
	BF3003_WriteReg(BF3003_VSTOP, (y+h)>>2);
	frameWidth = w;
	frameHeight = h;
}
void BF3003_SetDummy(uint16_t dummy)
{
	printf("set dummy:%d\n",dummy);
	BF3003_WriteReg(BF3003_EXHCH, dummy>>8);
	BF3003_WriteReg(BF3003_EXHCL, dummy&0xff);
}

void BF3003_SetMode(uint8_t gain, uint8_t whitebalance, uint8_t exposure)
{
	printf("set mode:%d, %d, %d\n",gain, whitebalance, exposure);
	uint8_t value = 0b00010000;
	value |= (gain?0b100:0b0);
	value |= (whitebalance?0b10:0b0);
	value |= (exposure?0b1:0b0);
	BF3003_WriteReg(BF3003_COM8, value);
}
void BF3003_SetExposure(uint16_t exposure)
{
	printf("set exp:%d\n",exposure);
    uint8_t exposure_l = exposure & 0xff;
    uint8_t exposure_h = exposure >> 8;
    BF3003_WriteReg(BF3003_INT_TIM_LO, exposure_l);
    BF3003_WriteReg(BF3003_INT_TIM_HI, exposure_h);
}
void BF3003_SetGain(uint8_t r,uint8_t g,uint8_t b)
{
	printf("set gain:%d %d %d\n",r, g, b);
	BF3003_WriteReg(BF3003_RED_GAIN, r);
	BF3003_WriteReg(BF3003_GREEN_GAIN, (g&0b111)+((g&0b111)<<4));
	/*GreenGain[2:0]:
		bit[2:0]: for odd column (used as GreenOgain[2:0])
		bit[6:4]: for even column (used as GreenEgain[2:0])
		bit[7]:choose RGB from colorcorrection or
		colorinterpolation module
	*/
	BF3003_WriteReg(BF3003_GN_GAIN, g>>3);
	/*
		bit[2:0]G channel Gain (bit2~bit0 is used as GreenGain[5:3]). 
	*/
	BF3003_WriteReg(BF3003_BLUE_GAIN, b);
}
void BF3003_SetGlobalGain(uint8_t gain)
{
	printf("global gain:%d\n",gain);
	BF3003_WriteReg(BF3003_GLB_GAIN, gain);
}
void BF3003_SetFrequency(uint16_t freqDiv, uint16_t skipDiv)
{
	printf("set freq:%d\n",freqDiv);
	baseFreq = freqDiv;
	skipFreq = skipDiv;
}
void _BF3003_SetFrequency(uint16_t freqDiv)
{
	if(currentFreq!=freqDiv)
	{
		currentFreq = freqDiv;
		TIM_Cmd(TIM3, DISABLE); 
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		TIM_TimeBaseStructure.TIM_Prescaler = freqDiv - 1;
		TIM_TimeBaseStructure.TIM_Period = 2 - 1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		// 72000000  / (TIM_Period + 1) / (TIM_Prescaler + 1)
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		TIM_Cmd(TIM3, ENABLE); 
		#ifdef DMA_ENABLE
		TIM_TimeBaseStructure.TIM_Prescaler = freqDiv*4 - 1;
		TIM_TimeBaseStructure.TIM_Period = 2 - 1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE); 
		TIM_Cmd(TIM2, ENABLE);    
		#endif   
	}
}
void _BF3003_UpdateFrequency(uint16_t freqDiv)
{
	if(currentFreq!=freqDiv)
	{
		currentFreq = freqDiv;
		TIM_PrescalerConfig(TIM3, freqDiv - 1, TIM_PSCReloadMode_Immediate);
	}
}