#ifndef __WIFI_8266_H_
#define __WIFI_8266_H_

#include "stm32l4xx_hal.h"
#include "usart.h"

// 系统信息
#define AT_RST           "AT+RST\r\n"                   // 重启模块
#define AT_RSTORE        "AT+RESTORE\r\n"                // 恢复出厂设置
#define AT_GMR           "AT+GMR\r\n"                   // 查询版本信息
#define AT_MAC           "AT+CIPSTAMAC_CUR?\r\n"        // 查询MAC地址
#define AT_STATUS           "AT+CIPSTATUS\r\n"          // 查询状态
#define AT_CIPSNTPCFG           "AT+CIPSNTPCFG=1,8\r\n" //时间区域设置
#define AT_CIPSNTPTIME          "AT+CIPSNTPTIME?\r\n"   //查询SNTP时间

//回显
#define ATE0        "ATE0\r\n"                          // 关闭回显
#define ATE1        "ATE1\r\n"                          // 打开回显

// BAUDE

#define AT_UART_DEF "AT+UART_DEF=74880,8,1,0,3\r\n"
// RF POWER
#define AT_RFPOWER  "AT+RFPOWER=82\r\n"

// 联网设置
#define AT_CWMODE        "AT+CWMODE=1\r\n"              // station mode
#define AT_CWMODE_DEF    "AT+CWMODE_DEF=1\r\n"          // softAP+station mode
#define AT_CWAUTOCONN    "AT+CWAUTOCONN=1\r\n"          // 系统默认上电自动连接

// 配网
#define AT_CWSTARTSMART  "AT+CWSTARTSMART\r\n"          // 开始smart配置
#define AT_CWSTOPSMART   "AT+CWSTOPSMART\r\n"           // 停止smart配置
#define AT_CWQAP         "AT+CWQAP\r\n"                 // 断开路由器

// 服务器


#define AT_CIPSSLSIZE      "AT+CIPSSLSIZE=4096\r\n"     // 设置ssl大小
#define AT_CIPSSLCCONF      "AT+CIPSSLCCONF=3\r\n"      // 双向认证

#define AT_TCP_SERVER    "AT+CIPSTART=\"SSL\",\"ih.giec.cn\",9951\r\n"       // 连接服务器
//#define AT_TCP_SERVER    "AT+CIPSTART=\"TCP\",\"192.168.1.106\",8080\r\n"       // 连接服务器
#define AT_CIPSEND       "AT+CIPSEND=4\r\n"                                      // 发送数据 默认4字节
#define AT_CIPCLOSE      "AT+CIPCLOSE\r\n"                                       // 断开服务器

#define AT_END           "+++"

// 上报标识
#define OPEN_UPLOAD         1000
#define WARN_UPLOAD         2000
#define ADD_USER_UPLOAD     3001
#define DELETE_USER_UPLOAD  3002
#define BATTERY_UPLOAD      4000
#define SERVER_TIME_UPLOAD  5000
#define CLOUD_UPLOAD        8000

// 报警上报码
#define LOW_POWER_WARN      0x01
#define TRY_ERR_WARN        0x02
#define ANTI_PRY_WARN       0x03
#define ANTI_HIJACK_WARN    0x04

#define ACK_OK              "receive ok"
#define ACK_ERROR           "receive error"

#define OK_KEYWORDS         "OK"
#define SEND_KEYWORDS       "SEND OK"
#define MAC_KEYWORDS        "+CIPSTAMAC_CUR:"
#define SERVER_KEYWORDS     "CONNECT"
#define TIME_KEYWORDS       "+IPD,26:"
#define STATUS_KEYWORDS     "STATUS:"
#define PASSWORD_KEYWORDS   "+IPD"
#define END_KEYWORDS        "+++"

#define DEVICE_APP_KEYWORDS "NETWORK SUCCEED"

#define RX_LEN 1024
#define UPLOAD_MAX   100


typedef struct
{
	uint8_t wifi_mac[16];
	struct wifi_date{
			int year;
			int month;
			int day;
			int hour;
			int min;
			int sec;
	}wifi_date_t;
}wifi_sys_info_t;

extern wifi_sys_info_t wifi_sys_info;

#define HAL_WIFI_STATUE_CHECK(status,code) {if(status!=code) return HAL_ERROR;}

void wifi_power_on(void);

void wifi_power_off(void);

void wifi_init(void);

void wifi_rxBuff_clear(void);

uint8_t wifi_upload_all_info(void);

uint8_t wifi_upload_alarm_info(void);

uint8_t wifi_upload_tryErr_info(void);

uint8_t wifi_get_mac(uint8_t * mac);

uint8_t wifi_get_password(uint8_t* reBuff,uint8_t* password);

uint8_t wifi_parase_time(char* time);

void wifi_upload_user_info(uint8_t *userList,uint8_t *mac);

int8_t wifi_upload_battery(uint32_t value);

uint8_t wifi_upload_send(uint8_t* str,uint32_t length_s);

uint8_t wifi_smart_config(uint8_t* at_command,uint8_t len);

HAL_StatusTypeDef wifi_remote_open(uint8_t *password,uint8_t* len,uint8_t* remote_flag,uint8_t *permision,uint8_t* index);

void double_point_parse(uint8_t* str,uint8_t* des);

HAL_StatusTypeDef usart3_search_item(const char* itemStr,uint8_t*reBuff,uint16_t timeout);

HAL_StatusTypeDef usart3_recv_parse(const char* itemStr,uint8_t* reBuff,uint16_t timeout);

HAL_StatusTypeDef wifi_trans_recev_command(uint8_t* command,uint16_t cLen,const char* itemStr,uint8_t* reBuff,uint16_t timeout);

#endif //__WIFI_8266_H_
