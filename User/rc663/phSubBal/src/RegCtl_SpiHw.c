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
* Filename:          RegCtl_SpiHw.c
* Processor family:  LPC11xx
*
* Description:       This file holds the functions for the SPI communication.
*                    In case of the Silica TUSA board, the SPI communication
*                    gets emulated, because it uses GPIO pins that are not
*                    designated for use of SPI communication.
*******************************************************************************/
#include <ph_Status.h>
#include <RegCtl_SpiHw.h>
#include "spi.h"
#include "usart.h"
#include "stdio.h"
#include "delay.h"

#if CONFIG_ENABLE_DRIVER_SSP==1
/* statistics of all the interrupts */
volatile uint32_t interruptRxStat0 = 0;
volatile uint32_t interruptOverRunStat0 = 0;
volatile uint32_t interruptRxTimeoutStat0 = 0;

volatile uint32_t interruptRxStat1 = 0;
volatile uint32_t interruptOverRunStat1 = 0;
volatile uint32_t interruptRxTimeoutStat1 = 0;
#endif

void SPI1_Init(void)
{

}
//复位脚为Pc7
void Reset_RC663_device(void)
{
	/* RSET signal low */
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_RESET);
	/* delay of ~1,2 ms */
	sys_delay_us(1500);
	/* RSET signal high to reset the RC663 IC */
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_SET);
	/* delay of ~1,2 ms */
	sys_delay_us(1500);
	/* RSET signal low */
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7, GPIO_PIN_RESET);
	/* delay of ~1,2 ms */
	sys_delay_us(1500);
}

/*********************************************************************/
/*
 * @brief 		Initialise the spi SSP0 for master mode
 * @param[in]	None
 *
 * @return 		None
 **********************************************************************/
void RegCtl_SpiHwInit(void)
{
	SPI1_Init();			/* Select the SSP0 for SPI interface */
}

static uint8_t SPIWriteByte(uint8_t Byte)
{	
		uint8_t Revalue=0xff;
		uint8_t status;
	
		if((status =HAL_SPI_TransmitReceive(&hspi2, &Byte, &Revalue, 1,100))!=HAL_OK)
		{
			debug_printf("spi2 send fail %d \r\n",status);
		}
		return Revalue;
}

/*****************************************************************************
** Function name:		_SSP0_SetReg
**
** Descriptions:		Send 2 databytes [RegNr, RegVal] to the SSP0 port
**
** parameters:			buffer pointer
** Returned value:		None
**
*****************************************************************************/
void _SSP0_SetReg(uint8_t *buf)
{
		uint8_t address,value;

		address = *buf++;	//地址
		value = *buf;		//数据

		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_RESET);
		SPIWriteByte(address);
		SPIWriteByte(value);
		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_SET);
}

void _SSP0_GetReg(uint8_t *buf)
{
		uint8_t ucAddr;
		uint8_t ucResult=0;

		ucAddr = *buf++;	//地址

		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_RESET);
	
		SPIWriteByte(ucAddr);
		ucResult=SPIWriteByte(0);
		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_SET);
	
		*buf=ucResult;
}
/*********************************************************************/
/*
 * @brief 		Get a databyte from a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Pointer to the databyte to be read
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_SpiHwGetReg(uint8_t address, uint8_t *reg_data)
{
		uint8_t buff[2];

		buff[0] = address;
		
		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_RESET);
	
		_SSP0_GetReg(buff);
		reg_data[1] = buff[1];
	
		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_SET);
	
		return HAL_OK;
}

/*********************************************************************//**
 * @brief 		Set a databyte to a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Databyte to be written
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_SpiHwSetReg(uint8_t address, uint8_t reg_data)
{
		uint8_t buff[2];

		buff[0] = address;
		buff[1] = reg_data;
		
		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_RESET);

		_SSP0_SetReg(buff);

		HAL_GPIO_WritePin(GPIOB, SPI2_CSS_Pin, GPIO_PIN_SET);

		return HAL_OK;
}

#if 0
/*********************************************************************//**
 * @brief 		Modify a bit in a register
 * @param[in]	address		Address of the register
 * @param[in]	*mask		Mask of the bit to set/reset
 * @param[in]	set			1: set 1: reset
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_SpiHwModReg(uint8_t address, uint8_t mask, uint8_t set)
{
    uint8_t  reg_data[2];

    /* get the register first */
    RegCtl_SpiHwGetReg(address, reg_data);

	if (set)
	{
		/* The bits of the mask, set to one are set in the new data: */
		reg_data[1] |= mask;
	}
	else
	{
		/* The bits of the mask, set to one are cleared in the new data: */
		reg_data[1] &= (uint8_t)(~mask);
	}

	/* set the register */
	RegCtl_SpiHwSetReg(address, reg_data[1]);

	/* return success */
	return PH_ERR_SUCCESS;
}
/*********************************************************************/
/*
 * @brief 		Get a databyte from a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Pointer to the databyte to be read
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_SpiHwSetMultiData(uint8_t *buf, uint32_t Len)
{
	/* Assert the SSEL pin */
    RC663_CS_CLR();
	_SSP0_SetMultiData(buf, Len);

	/* Deassert the SSEL pin */
	RC663_CS_SET();

	/* return success */
	return PH_ERR_SUCCESS;
}
/*********************************************************************/
/*
 * @brief 		Get a databyte from a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Pointer to the databyte to be read
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_SpiHwGetMultiData(uint8_t *txbuf, uint8_t *rxbuf, uint32_t Len)
{
	/* Assert the SSEL pin */
	RC663_CS_CLR();
	_SSP0_GetMultiData(txbuf, rxbuf, Len);

	/* Deassert the SSEL pin */
	RC663_CS_SET();
	/* return success */
	return PH_ERR_SUCCESS;
}
#endif
/******************************************************************************
**                            End Of File
******************************************************************************/


