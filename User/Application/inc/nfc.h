#ifndef __NFC_H__
#define __NFC_H__

#include "stm32l4xx_hal.h"

#define UID_LEN 32

typedef struct nfc_data_t
{
	uint8_t len;
	uint8_t irq_flag;
	uint8_t guid[UID_LEN];
}nfc_data;

extern nfc_data nfc_temp;

uint8_t nfc_add(uint8_t permision);
uint8_t nfc_delete(uint8_t index,uint8_t permision);

#endif  //__NFC_H__

