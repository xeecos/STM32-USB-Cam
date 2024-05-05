#ifndef __DELAY_H
#define __DELAY_H 			   
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void Delay_Init();
void Delay_Ms(uint16_t nms);
void Delay_Us(uint32_t nus);

#ifdef __cplusplus
}
#endif
#endif





























