#ifndef __DELAY_H__
#define __DELAY_H__

#include "stm32l4xx_hal.h"
#include "stdio.h"

void delay_init(uint8_t SYSCLK);
void sys_delay_us(uint32_t nus);
void sys_delay_ms(uint16_t nms);

#endif //__DELAY_H__
