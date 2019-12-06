#ifndef __USER_CHANGE_H__
#define __USER_CHANGE_H__

#include "stm32l4xx_hal.h"

uint8_t user_add_menu(void);
uint8_t user_add_menu_sub(uint8_t permision);
uint8_t user_add_hak_menu_sub(uint8_t permision);

uint8_t user_add_keyboard_menu(uint8_t permision);
uint8_t user_add_card_menu(void);


uint8_t user_delete_menu(void);
uint8_t user_delete_menu_sub(uint8_t permision);
uint8_t user_delete_hak_menu_sub(uint8_t permision);

uint8_t user_delete_password_menu(uint8_t permision);

uint8_t user_delete_fp_menu(uint8_t permision);

uint8_t user_delete_card_menu(uint8_t permision);

uint8_t sys_set_menu(void);

uint8_t sys_set_factory_menu(void);
uint8_t sys_set_date_menu(void);
uint8_t sys_set_config_wlan(void);
uint8_t sys_set_config_wlan_countdown(void);

uint8_t sys_operator_log(void);
uint8_t sys_resource_used_log(void);
uint8_t sys_battery_used_log(void);
uint8_t sys_host_about_log(void);
void sys_log_record(uint8_t index,uint8_t state,uint8_t permision,uint8_t type,uint8_t cur_index);


uint8_t sys_info_menu(void);

#endif // __USER_CHANGE_H_
