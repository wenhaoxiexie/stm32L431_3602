#ifndef __FPCST90X_H__
#define __FPCST90X_H__

#include "stm32l4xx_hal.h"
#include "stdio.h"
#include "global.h"

#define MASK_INS                0x01 

#define PAKEGE_HEAD_LEN 9

// 包头和包标识
#define PKG_HEAD_BYTE0          0xef       
#define PKG_HEAD_BYTE1          0x01

#define MASK_ACK 0x07

/*Instruction*/
#define PS_GetImage             0x01  
#define PS_GenChar              0x02          
#define PS_RegModel             0x05       
#define PS_StoreChar            0x06   

#define PS_DeletChar            0x0c 
#define PS_Empty                0x0d

#define PS_HighSpeedSearch      0x1b

#define PS_GetEnrollImage       0x29 

#define PS_Sleep                0x33      

/* led config */
#define Ps_LedConfig            0x40

typedef enum
{
  FP_OK       = 0x00,
  FP_ERROR    = 0x01,
  FP_BUSY     = 0x02,
  FP_TIMEOUT  = 0x03
}FP_StatusTypeDef;

void fp_power_on(void);
void fp_power_off(void);
void fp_sleep(void);

uint8_t fp_add(uint8_t permision);
uint8_t fp_delete(uint8_t index,uint8_t permision);

void fp_led_config(void);

void fp_led_red(void);
void fp_led_green(void);
void fp_led_blue(void);
void fp_led_white(void);

void fp_led_shutdown(void);

void fp_str_copy(uint8_t *Src, uint8_t *pDes, uint32_t len);

void fp_trans_buff_clear(void);

void fp_recev_buff_clear(void);

uint16_t caculator_hex_sum(uint8_t*pBuff);

void MakePackageHead(uint8_t* pHead,uint8_t Type,uint32_t nDataLen);

FP_StatusTypeDef fp_usart2_send(uint8_t Type, uint8_t INS, uint8_t *pData, uint16_t Datalen);

FP_StatusTypeDef checkout_sum_tool(uint8_t* pBuff);

FP_StatusTypeDef usart2_recv_data(uint8_t* pBuf,uint16_t dataLen,uint16_t timeOut);

FP_StatusTypeDef fp_pakge_recv_parse(uint8_t* pPakege,uint16_t* pLen,uint16_t timeOut,uint8_t* pResult);

FP_StatusTypeDef fp_tarns_recv_command(uint8_t Type, uint8_t INS, uint8_t *pDataIn, uint16_t *Datalen, uint8_t *pDataOut, uint8_t *pErr, uint16_t TimeOutMs);

#endif  // __FPCST90X_H__
