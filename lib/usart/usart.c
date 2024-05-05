#include "usart.h"
#include "stm32f10x.h"
#include "sys.h"

int _write(int fd, char *ptr, int len)  
{  
  for(int i=0;i<len;i++)
  {
	while ((USART1->SR & 0X40) == 0);
	USART1->DR = ptr[i];
  }
  return len;
}

uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA = 0; 

void USART1_IRQHandler(void)
{
	uint8_t res;
	if (USART1->SR & (1 << 5)) // 接收到数据
	{
		res = USART1->DR;
		if ((USART_RX_STA & 0x8000) == 0) // 接收未完成
		{
			if (USART_RX_STA & 0x4000) // 接收到了0x0d
			{
				if (res != 0x0a)
					USART_RX_STA = 0; // 接收错误,重新开始
				else
					USART_RX_STA |= 0x8000; // 接收完成了
			}
			else // 还没收到0X0D
			{
				if (res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; // 接收数据错误,重新开始接收
				}
			}
		}
	}
}
void Usart_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

	// NVIC_InitTypeDef nvic_init;
    // nvic_init.NVIC_IRQChannel = USART1_IRQn;
    // nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
    // nvic_init.NVIC_IRQChannelSubPriority = 0;
    // nvic_init.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&nvic_init);
}
