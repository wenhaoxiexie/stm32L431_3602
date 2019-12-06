#ifndef __WIN6170_H__
#define __WIN6170_H__

#include "stm32l4xx_hal.h"
#include "stdio.h"

void horn_powerOff(void);
void horn_powerOn(void);

void set_win6170(uint8_t addr);
uint8_t vol_set(void);

#endif //__WIN6170_H__
