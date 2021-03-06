#ifndef  __74HC595_H__
#define  __74HC595_H__

#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "stdio.h"

/* 
	1.	对应位置1-->熄灭 

	2.  D1..D21 对应硬件排版
*/

//#define KEY0_LED1_OFF 0x8000
//#define KEY1_LED10_OFF 0x0040
//#define KEY2_LED6_OFF 0x0400
//#define KEY3_LED5_OFF 0x0800
//#define KEY4_LED11_OFF 0x0004
//#define KEY5_LED7_OFF 0x0200
//#define KEY6_LED4_OFF 0x1000
//#define KEY7_LED12_OFF 0x0002
//#define KEY8_LED8_OFF 0x0100
//#define KEY9_LED3_OFF 0x2000
//#define KEY10_LED21_OFF 0x0001
//#define KEY11_LED2_OFF 0x4000

#define KEY0_LED1_OFF 0x0080
#define KEY1_LED10_OFF 0x0008
#define KEY2_LED6_OFF 0x0400
#define KEY3_LED5_OFF 0x0800
#define KEY4_LED11_OFF 0x0004
#define KEY5_LED7_OFF 0x0200
#define KEY6_LED4_OFF 0x0x1000
#define KEY7_LED12_OFF 0x0002
#define KEY8_LED8_OFF 0x0100
#define KEY9_LED3_OFF 0x2000
#define KEY10_LED21_OFF 0x0001
#define KEY11_LED2_OFF 0x4000


#define KEY10_ON        0xfffe    //~KEY10_LED21_OFF

#define KEY10_11_ON     0xbffe    //~(KEY10_LED21_OFF|KEY11_LED2_OFF)

#define KEY_MAIN_MENU   0xb3f2    // ~(KEY1_LED10_OFF|KEY2_LED6_OFF|KEY3_LED5_OFF|KEY4_LED11_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_ADD_MENU    0xb3f6    //~(KEY1_LED10_OFF|KEY2_LED6_OFF|KEY3_LED5_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_TURN_PAGE   0xbafe    //~(KEY2_LED6_OFF|KEY8_LED8_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_LOG_RECORD  0xfafe    //~(KEY2_LED6_OFF|KEY8_LED8_OFF|KEY10_LED21_OFF)
#define KEY_CONFIG_WLAN      0xbff6    //~(KEY1_LED10_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_ADD_ANTI_MENU    0xbbf6    //~(KEY1_LED10_OFF|KEY2_LED6_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)

#define KEY_0_1_10_11_ON       0xbf76      //(uint16_t)~(KEY0_LED1_OFF|KEY1_LED10_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_0_1_2_10_11_ON     0xbb77      //(uint16_t)~(KEY0_LED1_OFF|KEY1_LED10_OFF|KEY2_LED6_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)
#define KEY_0_1_2_3_10_11_ON        (uint16_t)~(KEY0_LED1_OFF|KEY1_LED10_OFF|KEY2_LED6_OFF|KEY3_LED5_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)

#define KEY_0_1_2_3_4_5_10_11_ON  0xf173   //(uint16_t)~(KEY0_LED1_OFF|KEY1_LED10_OFF|KEY2_LED6_OFF|KEY3_LED5_OFF|KEY4_LED11_OFF \
																	         //|KEY5_LED7_OFF|KEY10_LED21_OFF|KEY11_LED2_OFF)


#define KEY_ALL_ON 0x8030
#define KEY_ALL_OFF 0xffff

void led_powerOff(void);
void led_powerOn(void);

void send_bytes(uint16_t data);

#endif  //74HC595_H__
