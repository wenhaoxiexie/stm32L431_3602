#ifndef __MENU_H__
#define __MENU_H__

#include "stm32l4xx_hal.h"
#include "global.h"
#include "ssd1305.h"
#include "apt8l16.h"
#include "delay.h"
#include "user_operator.h"
#include "flash.h"
#include "usart.h"
#include "keyboard.h"
#include "74hc595.h"
#include "nfc9883.h"
#include "win6170.h"
#include "nfc.h"
#include "wifi_8266.h"

void voice_repeat(uint8_t addr);

void multi_mode_init(void);

uint8_t keyboard_login(uint8_t* permision);

uint8_t finger_login(uint8_t* permision);

uint8_t card_login(uint8_t* permision);

uint8_t main_menu(void);

float sys_battery_get(void);

uint8_t keyboard_lock(uint8_t *try_count);

void display_menu(void);

void sleep_mode(void);

#endif  // __MENU_H__
