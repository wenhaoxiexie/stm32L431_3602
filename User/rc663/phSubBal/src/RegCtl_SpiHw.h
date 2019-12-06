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
* Filename:          RegCtl_SpiHw.h
* Processor family:  LPC11xx
*
* Description:       This file holds the functions for the SPI communication.
*                    In case of the Silica TUSA board, the SPI communication
*                    gets emulated, because it uses GPIO pins that are not
*                    designated for use of SPI communication.
*******************************************************************************/

#ifndef REGCTL_SPIHW_H_
#define REGCTL_SPIHW_H_

#include <ph_Status.h>
#include "stm32l4xx_hal.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"


#define SPI_USED
extern void Reset_RC663_device(void);
extern void RegCtl_SpiHwInit(void);
extern phStatus_t RegCtl_SpiHwGetReg(uint8_t address, uint8_t *reg_data);
extern phStatus_t RegCtl_SpiHwSetReg(uint8_t address, uint8_t reg_data);
//extern phStatus_t RegCtl_SpiHwModReg(uint8_t address, uint8_t mask, uint8_t set);
//extern phStatus_t RegCtl_SpiHwSetMultiData(uint8_t *buf, uint32_t Len);
//extern phStatus_t RegCtl_SpiHwGetMultiData(uint8_t *txbuf, uint8_t *rxbuf, uint32_t Len);

#endif  /* __SSP_H__ */
/*****************************************************************************
**                            End Of File
******************************************************************************/
