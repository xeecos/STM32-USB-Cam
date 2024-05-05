#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "bf3003.h"
#include "usb_init.h"
#include "hw_config.h"

int main()
{
    SetSysClockToHSE();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
    #ifdef DMA_ENABLE
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    #else
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    #endif
    
    GPIO_InitTypeDef GPIO_InitStruct;  
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    MY_NVIC_PriorityGroupConfig(2);
    Delay_Init();
    Usart_Init(115200);
    BF3003_Init();
    USB_Init();
    // BF3003_Start();
    while (1) 
    {
        // Delay_Ms(1000);
        // BF3003_Handle();
    }
}