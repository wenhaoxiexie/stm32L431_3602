#ifndef __HSJ08_H__
#define __HSJ08_H__

#include "stm32l4xx_hal.h"

void hsj08_init(void);

void LOCK(void);
void UNLOCK(void);

void hsj08_exit(void);

#endif // __HSJ08_H__
