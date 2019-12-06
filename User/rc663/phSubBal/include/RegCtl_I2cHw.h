/*******************************************************************************
*         Copyright (c), NXP Semiconductors Gratkorn / Austria
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
*particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
********************************************************************************
*
* Filename:          RegCtl_I2cHw.h
* Processor family:  LPC11xx
*
* Description:       This file holds the functions for the I2C communication.
*******************************************************************************/

#ifndef REGCTL_I2CHW_H_
#define REGCTL_I2CHW_H_

#include <ph_Status.h>

//#define I2C_USED
#define RC663_ADDR		0x50U

extern void RegCtl_I2cHwInit(void);
extern phStatus_t RegCtl_I2cHwGetReg(uint8_t address, uint8_t *reg_data);
extern phStatus_t RegCtl_I2cHwSetReg(uint8_t address, uint8_t reg_data);
extern phStatus_t RegCtl_I2cHwModReg(uint8_t address, uint8_t mask, uint8_t set);

#endif /* REGCTL_I2CHW_H_ */

#ifndef __I2C_H
#define __I2C_H
#if CONFIG_ENABLE_DRIVER_I2C==1

/* If I2C SEEPROM is tested, make sure FAST_MODE_PLUS is 0.
For board to board test, this flag can be turned on. */

#define FAST_MODE_PLUS	0

#define BUFSIZE             64
#define MAX_TIMEOUT		0x00FFFFFF

#define I2CMASTER		0x01
#define I2CSLAVE		0x02

#define PCF8594_ADDR	0xA0
#define READ_WRITE		0x01

#define RD_BIT			0x01

#define I2C_IDLE			0
#define I2C_STARTED			1
#define I2C_RESTARTED		2
#define I2C_REPEATED_START	3
#define DATA_ACK			4
#define DATA_NACK			5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH			0x00000180  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL			0x00000180  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_SCLH_120_KHZ (0x00000190/2)  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL_120_KHZ	(0x00000190/2)  /* I2C SCL Duty Cycle Low Reg */

#define I2SCLH_SCLH_360_KHZ I2SCLH_SCLH_120_KHZ/3  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL_360_KHZ	I2SCLL_SCLL_120_KHZ/3  /* I2C SCL Duty Cycle Low Reg */

#define I2SCLH_SCLH_10_KHZ  (0x00000120 * 10)  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL_10_KHZ	(0x00000120 * 10)  /* I2C SCL Duty Cycle Low Reg */

#define I2SCLH_HS_SCLH		0x00000015  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL		0x00000015  /* Fast Plus I2C SCL Duty Cycle Low Reg */

#if CONFIG_I2C_DEFAULT_I2C_IRQHANDLER==1
extern void I2C_IRQHandler( void );
#endif
extern uint32_t I2CInit( uint32_t I2cMode );
extern uint32_t I2CStart( void );
extern uint32_t I2CStop( void );
extern uint32_t I2CEngine( void );

#endif
#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
