#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include "stm32l4xx_hal.h"
#include "stdio.h"

uint8_t keyboard_password_save(uint8_t *password,uint8_t len,uint8_t permision);

uint8_t keyboard_password_del(uint8_t index,uint8_t permision);

uint8_t keyboard_password_confirm(uint8_t *password,uint8_t len,uint8_t* permision);


#endif //__KEYBOARD_H_
