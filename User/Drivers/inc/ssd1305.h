#ifndef __SSD1305_H__
#define __SSD1305_H__

#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"

#if 0
#define SDA_GPIO_PORT   GPIOC
#define SDA_GPIO_PIN    GPIO_PIN_1

#define SCL_GPIO_PORT   GPIOC
#define SCL_GPIO_PIN    GPIO_PIN_0

#define SET_SDA()     HAL_GPIO_WritePin(SDA_GPIO_PORT,SDA_GPIO_PIN,GPIO_PIN_SET)    
#define CLR_SDA()     HAL_GPIO_WritePin(SDA_GPIO_PORT,SDA_GPIO_PIN,GPIO_PIN_RESET)  

#define SET_CLK()     HAL_GPIO_WritePin(SCL_GPIO_PORT,SCL_GPIO_PIN,GPIO_PIN_SET)    
#define CLR_CLK()     HAL_GPIO_WritePin(SCL_GPIO_PORT,SCL_GPIO_PIN,GPIO_PIN_RESET)

#define READ_SDA()    HAL_GPIO_ReadPin(SDA_GPIO_PORT,SDA_GPIO_PIN)
	

#define OP_Data		0x40
#define OP_Command	0x00

#endif

#define Slave_Address	0x78  //0X3C  ??IIC???????????


typedef unsigned char uchar;

void IC_IIC_Start(void);
void IC_IIC_Stop(void);
GPIO_PinState Write_IIC_Data(uchar Data);
void Write_Command(uchar command);
void Write_Data(uchar date);

void Reset_IC(void);
void init_ic(void);
void Clear_Screen(void);
void All_Screen(void);

void oled_init(void);
void oled_powerOff(void);
void oled_powerOn(void);

void display_one_word(const uint8_t *DestBuf,uint8_t IconW,uint8_t IconH, uint8_t StartPageN,  uint8_t StartDsColN);
void display_one_word_const(const uint8_t *DestBuf,uint8_t IconW,uint8_t IconH, uint8_t StartPageN,  uint8_t StartDsColN);
void display_welcome(void);
void display_number(int num,int locate);
void display_number_all(int num,int locate);
void display_number_del(int locate);
void display_passwod_put(void);
void display_passwod_warn(void);
void display_passwod_warn_again(void);
void display_passwod_null(void);
void display_passwod_admin(void);
void display_passwod_exit(void);
void display_password_full(void);

void display_iphone_number(int num,int locate);

void display_mainmenu(void);
void display_submenu_add(void);
void display_subsubmenu_add(uint8_t button);

void display_submenu_del(uint8_t button);

void display_submenu_sys(void);
void display_subsubmenu_sys(void);

void display_sys_info_menu(void);
void display_sys_host_info(void);
void display_sys_battery_info(void);
void display_sys_source_info(uint8_t fp_num,uint8_t keyboard_num,uint8_t card_num);
void display_sys_log_null(void);
void display_sys_log_menu(int16_t cur_index);
void display_sys_reset(void);
void display_sys_exit(void);

void display_sys_config_wlan(void);
void display_sys_config_wlan_countdown(uint8_t count);
void display_sys_unlock_wlan_countdown(uint8_t count);
void display_sys_config_sucessful(void);
void display_sys_config_fail(void);
void display_sys_time(uint8_t count);

void display_wifi_wait(void);
void display_wifi_factory_sucess(void);
void display_wifi_factory_fail(void);

void display_factory_success(void);
void display_factory_fail(void);
void display_factory_sel(void);
void display_factory_hjs_number(uint32_t count);

void display_wlan_time_sys(void);
void display_wifi_statue(void);

void display_spe_char(uint8_t locate);
void display_spe_char_factory(uint8_t ch,uint8_t locate);
void display_spe_char_del(uint8_t locate);
void display_company_name(void);
void display_check_failed(void);
void display_check_sucessful(uint8_t index,uint8_t permision,uint8_t type);
void display_check_exit(void);

void display_operate_fail(void);
void display_operate_sucessful(void);

void display_date_md(uint8_t month,uint8_t day);
void display_date_ymd(uint8_t year,uint8_t month,uint8_t day);
void display_date_hm(uint8_t hour,uint8_t min);

void display_del_sucessful(void);
void display_del_failed(void);

void display_number_password_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);
void display_number_password(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);

void display_number_finger_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);
void display_number_finger(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);

void display_number_card_const(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);
void display_number_card(uint8_t number,uint8_t page,uint8_t lock_flag,uint8_t lock_num);

void display_finger_check_fail(void);
void display_finger_press(void);
void display_finger_collection(void);
void display_finger_sucess(void);
void display_finger_fail(void);
void display_finger_exit(void);
void display_finger_outtime(void);
void display_finger_del_sucess(void);
void display_finger_del_fail(void);
void display_finger_non_exit(void);
void display_finger_null(void);
void display_finger_realse(void);
void display_finger_full(void);


void display_confirm(uint8_t type,uint8_t permision,uint8_t number);

void display_set_sucess(void);
void display_set_fail(void);

void display_whether_factory(void);
void display_date_set(void);
void display_date_update(uint8_t page,uint8_t start_col,uint8_t end_col,uint8_t number);

void display_card_non_exit(void);
void display_card_full(void);
void display_card_rigister(void);
void display_card_exit(void);
void display_card_add_sucess(void);
void display_card_null(void);

void display_battery_update(uint8_t class);
void display_battery_warn(void);

void display_keyboard_count(uint16_t count);

void display_iphone_num(void);
void display_iphone_result(void);
void display_iphone_appoint(void);
void display_iphone_nonexistent(void);
void display_iphone_offline(void);

void oled_refresh_gram(void);
void oled_refresh_gram_with_arg(uint8_t page);
void oled_refresh_gram_with_multi_arg(uint8_t page,uint8_t start_col,uint8_t end_col,uint8_t contrast_sel);


#endif //__SSD1305__H__
