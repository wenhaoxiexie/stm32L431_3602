#ifndef __NFC9883_H_
#define __NFC9883_H_

#include "stm32l4xx_hal.h"
#include "public.h"

extern phhalHw_Rc663_DataParams_t halReader;

uint16_t nfc9883_init(void);

uint16_t detect_card(void);

#endif //__NFC9883_H_
