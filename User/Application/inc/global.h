#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "stm32l4xx_hal.h"
#include "stdio.h"
#include "delay.h"
#include "stmflash.h"
#include "stdlib.h"
#include "string.h"
#include "ssd1305.h"
#include "usart.h"
#include "rtc.h"
#include "74hc595.h"

#define DEBUG_PRINT

#define USART3_REC_LEN  			2048
#define USART2_REC_LEN      1024


#define CARD_NUM       100
#define KEYBOARD_NUM   100
#define FINGER_NUM   	 100
#define LOG_NUM        500
#define WIFI_UPLOAD_NUM    100


#define REVAL_CHECK_SUCCESS(status)         {if ((status) == LOCK_ERROR) {return (status);}}
#define REVAL_CHECK_SUCCESS_FCT(status,fct) {(status) = (fct); REVAL_CHECK_SUCCESS(status);}

// 管理员、普通用户、防劫持用户
typedef enum{
	ANDMI=1,
	USUALLY,
	ANTI_HAK,
}PERMISION;

// 密码、卡片、指纹 app
typedef enum {
	KEYBOARD_TYPE=1,
	FINGER_TYPE,
	CARD_TYPE,
	APP_TYPE
}USER_TYPE;

typedef enum{
	OPEN_DOOR=1,
	ADD_USER_OPR,
	DEL_USER_OPR,
	OPERATOR_MAX
}OPERATOR_STATE;

// 操作日志
typedef struct{
		uint8_t index;
		OPERATOR_STATE state;
		PERMISION permision;
		USER_TYPE type;
		RTC_DateTypeDef date;
		RTC_TimeTypeDef time;
}log_store_t;

typedef struct
{
	uint8_t exist;
	uint8_t index;
	char upload_info[64]; 
}wifi_upload_store_t;

// 返回值
typedef enum
{
  LOCK_OK       = 0x00,
  LOCK_ERROR    = 0x01,
	LOCK_EXIT     = 0x02,
	LOCK_OUTTIME  = 0x03,
	LOCK_MAX      = 0x04,
	LOCK_OUTTIME_FP=0x05
}FP_STATUS_T;

// 全局区 最新数值
typedef struct {
	uint16_t cur_index_number;
	uint16_t cur_card_index;
	uint16_t cur_keyboard_index;
	uint16_t cur_fingerprint_index;
	uint16_t cur_vol_size;
	uint16_t cur_log_number;
	uint16_t wifi_button;
	uint8_t cur_battery;
}cur_index_t;

// 按键状态
typedef struct{
		uint8_t key_prees;
		uint8_t number;
}KEY_STATE;

// 密码用户属性
typedef struct {
	uint8_t index;
	uint8_t user_type;
	uint8_t user_permision;
	uint8_t data_len;
	uint8_t data[16];
	uint8_t del_flag;
}keyboard_store_t;

// 指纹用户属性
typedef struct {
	uint8_t index;
	uint8_t user_type;
	uint8_t user_permision;
	uint8_t del_flag;
}finger_store_t;

// 卡片用户属性
typedef struct {
	uint8_t index;
	uint8_t user_type;
	uint8_t user_permision;
	uint8_t uid_len;
	uint8_t del_flag;
	uint8_t uid[16];
}nfc_store_t;

// 存贮空间大小
#define CARD_SIZE        sizeof(nfc_store_t)*CARD_NUM
#define KEYBOARD_SIZE    sizeof(keyboard_store_t)*KEYBOARD_NUM
#define FINGER_SIZE      sizeof(finger_store_t)*FINGER_NUM	
#define CUR_INDEX_SIZE   sizeof(cur_index_t)
#define LOG_SIZE         sizeof(log_store_t)*LOG_NUM
#define WIFI_SIZE      sizeof(wifi_upload_store_t)*WIFI_UPLOAD_NUM

extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;

extern uint8_t usart2_rx_buf[USART2_REC_LEN];
extern uint16_t usart2_rx_len;

extern uint8_t usart3_rx[1];
extern uint8_t usart3_rx_buf[USART3_REC_LEN];
extern uint16_t usart3_rx_len;

#endif //__GLOBAL_H__
