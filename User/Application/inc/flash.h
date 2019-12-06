#ifndef __FLASH_ALLOC_H__
#define __FLASH_ALLOC_H__

#include "stm32l4xx_hal.h"
#include "global.h"
#include "rtc.h"


extern keyboard_store_t* key_board_flash;
extern finger_store_t* finger_flash;
extern cur_index_t* get_cur_info;
extern log_store_t* log_flash;
extern nfc_store_t* nfc_flash;
extern wifi_upload_store_t* wifi_flash;

extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;

void flash_init(void);
void flash_exit(void);

void factory_reset(void);

#endif  // __FLASH_ALLOC_H__
