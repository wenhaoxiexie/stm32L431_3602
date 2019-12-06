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
* Filename:          RegCtl_I2cHw.c
* Processor family:  LPC11xx
*
* Description:       This file holds the functions for the I2C communication.
*******************************************************************************/

#include <driver_config.h>
#include <ph_Status.h>
#include <RegCtl_I2cHw.h>
#include <debug_printf.h>

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;
#define DEBUG_REG	0

#if CONFIG_ENABLE_DRIVER_I2C==1
#include "type.h"

volatile uint32_t I2CMasterState = I2C_IDLE;
volatile uint32_t I2CSlaveState = I2C_IDLE;
volatile uint32_t timeout = 0;

volatile uint32_t I2CMode;

volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint8_t I2CSlaveBuffer[BUFSIZE];
volatile uint32_t I2CCount = 0;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

volatile uint32_t RdIndex = 0;
volatile uint32_t WrIndex = 0;

#if CONFIG_I2C_DEFAULT_I2C_IRQHANDLER==1
#endif
/*
From device to device, the I2C communication protocol may vary,
in the example below, the protocol uses repeated start to read data from or
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO
for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could
be READ or WRITE depending on the I2C command.
*/


/*******************************************************************************
**   Main Function  main()
*******************************************************************************/

/*********************************************************************//**
 * @brief 		Initialise the i2c bus for master mode
 * @param[in]	None
 *
 * @return 		None
 **********************************************************************/
void RegCtl_I2cHwInit(void)
{
	  I2CInit((uint32_t)I2CMASTER);	/* initialize I2c */
}

/*********************************************************************//**
 * @brief 		Get a databyte from a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Pointer to the databyte to be read
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *              - PH_ERR_INTERFACE_ERROR
 **********************************************************************/
phStatus_t RegCtl_I2cHwGetReg(uint8_t address, uint8_t *reg_data)
{
	/* Write SLA(W), address and one data byte */
	I2CWriteLength = 2;					/* 2 bytes to write */
	I2CReadLength = 0;					/* no bytes to read */
	I2CMasterBuffer[0] = RC663_ADDR;
	I2CMasterBuffer[1] = address;		/* address */
	I2CEngine();

	/* Operation not OK ? */
	if (I2C_OK != I2CMasterState)
	{
		/* other return error */
		return PH_ERR_INTERFACE_ERROR;
	}

	/* Write SLA(R) and read one byte back. */
	I2CWriteLength = 1;
	I2CReadLength = 1;
	I2CMasterBuffer[0] = RC663_ADDR | RD_BIT;	/* address */
	I2CEngine();

	/* Operation not OK ? */
	if (I2C_OK != I2CMasterState)
	{
		/* no data read - return a dummy databyte */
		*reg_data = 0xFFU;

		/* other return error */
		return PH_ERR_INTERFACE_ERROR;
	}

	/* return the byte read */
	*reg_data = I2CSlaveBuffer[0];

#if DEBUG_REG
	debug_printf("GR");
	debug_puts_hex(address);
	debug_puts_hex(I2CSlaveBuffer[0]);
#endif

	/* return success */
	return PH_ERR_SUCCESS;

}

/*********************************************************************//**
 * @brief 		Set a databyte from a register
 * @param[in]	address		Address of the register
 * @param[in]	*reg_data	Databyte to be written
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *              - PH_ERR_INTERFACE_ERROR
 **********************************************************************/
phStatus_t RegCtl_I2cHwSetReg(uint8_t address, uint8_t reg_data)
{
#if DEBUG_REG
	debug_printf("SR");
	debug_puts_hex(address);
	debug_puts_hex(reg_data);
#endif

	/* Write SLA(W), address and one data byte */
	I2CWriteLength = 3;					/* 3 bytes to write */
	I2CReadLength = 0;					/* no bytes to read */
	I2CMasterBuffer[0] = RC663_ADDR;
	I2CMasterBuffer[1] = address;		/* address */
	I2CMasterBuffer[2] = reg_data;		/* Data */

	I2CEngine();

	/* Operation not OK ? */
	if (I2C_OK != I2CMasterState)
	{
		/* other return error */
		return PH_ERR_INTERFACE_ERROR;
	}

	/* return success */
	return PH_ERR_SUCCESS;
}

/*********************************************************************//**
 * @brief 		Modify a bit in a register
 * @param[in]	address		Address of the register
 * @param[in]	*mask		Mask of the bit to set/reset
 * @param[in]	set			1: set 1: reset
 *
 * @return 		- PH_ERR_BFL_SUCCESS
 *
 **********************************************************************/
phStatus_t RegCtl_I2cHwModReg(uint8_t address, uint8_t mask, uint8_t set)
{
    uint8_t  reg_data;

    /* get the register first */
    RegCtl_I2cHwGetReg(address, &reg_data);

	if (set)
	{
		/* The bits of the mask, set to one are set in the new data: */
		reg_data |= mask;
	}
	else
	{
		/* The bits of the mask, set to one are cleared in the new data: */
		reg_data &= (uint8_t)(~mask);
	}

	/* set the register */
	RegCtl_I2cHwSetReg(address, reg_data);

    return PH_ERR_SUCCESS;
}

#if CONFIG_ENABLE_DRIVER_I2C==1

/*****************************************************************************
** Function name:		I2C_IRQHandler
**
** Descriptions:		I2C interrupt handler, deal with master mode only.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void I2C_IRQHandler(void)
{
  uint8_t StatValue;

  timeout = 0;
  /* this handler deals with master read and master write only */
  StatValue = LPC_I2C->STAT;
  switch ( StatValue )
  {
	case 0x08:			/* A Start condition is issued. */
	WrIndex = 0;
	LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x10:			/* A repeated started is issued */
	RdIndex = 0;
	/* Send SLA with R bit set, */
	LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
	LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
	break;

	case 0x18:			/* Regardless, it's a ACK */
	if ( I2CWriteLength == 1 )
	{
	  LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	  I2CMasterState = I2C_NO_DATA;
	}
	else
	{
	  LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x28:	/* Data byte has been transmitted, regardless ACK or NACK */
	if ( WrIndex < I2CWriteLength )
	{
	  LPC_I2C->DAT = I2CMasterBuffer[WrIndex++]; /* this should be the last one */
	}
	else
	{
	  if ( I2CReadLength != 0 )
	  {
		LPC_I2C->CONSET = I2CONSET_STA;	/* Set Repeated-start flag */
	  }
	  else
	  {
		LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
		I2CMasterState = I2C_OK;
	  }
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x30:
	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	I2CMasterState = I2C_NACK_ON_DATA;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x40:	/* Master Receive, SLA_R has been sent */
	if ( (RdIndex + 1) < I2CReadLength )
	{
	  /* Will go to State 0x50 */
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	}
	else
	{
	  /* Will go to State 0x58 */
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK after data is received */
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x50:	/* Data byte has been received, regardless following ACK or NACK */
	I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
	if ( (RdIndex + 1) < I2CReadLength )
	{
	  LPC_I2C->CONSET = I2CONSET_AA;	/* assert ACK after data is received */
	}
	else
	{
	  LPC_I2C->CONCLR = I2CONCLR_AAC;	/* assert NACK on last byte */
	}
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x58:
	I2CSlaveBuffer[RdIndex++] = LPC_I2C->DAT;
	I2CMasterState = I2C_OK;
	LPC_I2C->CONSET = I2CONSET_STO;	/* Set Stop flag */
	LPC_I2C->CONCLR = I2CONCLR_SIC;	/* Clear SI flag */
	break;

	case 0x20:		/* regardless, it's a NACK */
	case 0x48:
	LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
	I2CMasterState = I2C_NACK_ON_ADDRESS;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	case 0x38:		/* Arbitration lost, in this example, we don't
					deal with multiple master situation */
	default:
	I2CMasterState = I2C_ARBITRATION_LOST;
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;
  }
  return;
}
#endif

/*****************************************************************************
** Function name:		I2CStart
**
** Descriptions:		Create I2C start condition, a timeout
**				value is set if the I2C never gets started,
**				and timed out. It's a fatal error.
**
** parameters:			None
** Returned value:		true or false, return false if timed out
**
*****************************************************************************/
uint32_t I2CStart( void )
{
  uint32_t timeout = 0;
  uint32_t retVal = FALSE;

  /*--- Issue a start condition ---*/
  LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */

  /*--- Wait until START transmitted ---*/
  while( 1 )
  {
	if ( I2CMasterState == I2C_STARTED )
	{
	  retVal = TRUE;
	  break;
	}
	if ( timeout >= MAX_TIMEOUT )
	{
	  retVal = FALSE;
	  break;
	}
	timeout++;
  }
  return( retVal );
}

/*****************************************************************************
** Function name:		I2CStop
**
** Descriptions:		Set the I2C stop condition, if the routine
**				never exit, it's a fatal bus error.
**
** parameters:			None
** Returned value:		true or never return
**
*****************************************************************************/
uint32_t I2CStop( void )
{
  LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */
  LPC_I2C->CONCLR = I2CONCLR_SIC;  /* Clear SI flag */

  /*--- Wait for STOP detected ---*/
  while( LPC_I2C->CONSET & I2CONSET_STO );
  return TRUE;
}

/*****************************************************************************
** Function name:		I2CInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
**
*****************************************************************************/
uint32_t I2CInit( uint32_t I2cMode )
{
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);

  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;	/*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;		/* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;
  LPC_IOCON->PIO0_5 |= 0x01;		/* I2C SDA */
  /* IOCON may change in the next release, save change for future references. */
//  LPC_IOCON->PIO0_4 |= (0x1<<10);	/* open drain pins */
//  LPC_IOCON->PIO0_5 |= (0x1<<10);	/* open drain pins */

  /*--- Clear flags ---*/
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;

  /*--- Reset registers ---*/
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x2<<8);
  LPC_IOCON->PIO0_5 |= (0x2<<8);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
//  /* i2c frequency = 62 Khz */
//  LPC_I2C->SCLL   = I2SCLL_SCLL;
//  LPC_I2C->SCLH   = I2SCLH_SCLH;
  /* i2c frequency = 100 Khz */
  LPC_I2C->SCLL   = I2SCLH_SCLH_360_KHZ;
  LPC_I2C->SCLH   = I2SCLL_SCLL_360_KHZ;
//  /* i2c frequency = 10 Khz */
//  LPC_I2C->SCLL   = I2SCLL_SCLL_10_KHZ;
//  LPC_I2C->SCLH   = I2SCLH_SCLH_10_KHZ;

#endif

  if ( I2cMode == I2CSLAVE )
  {
	LPC_I2C->ADR0 = PCF8594_ADDR;
  }

  /* Enable the I2C Interrupt */
  NVIC_EnableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN;
  return( TRUE );
}

/*****************************************************************************
** Function name:		I2CEngine
**
** Descriptions:		The routine to complete a I2C transaction
**				from start to stop. All the intermitten
**				steps are handled in the interrupt handler.
**				Before this routine is called, the read
**				length, write length, I2C master buffer,
**				and I2C command fields need to be filled.
**				see i2cmst.c for more details.
**
** parameters:			None
** Returned value:		true or false, return false only if the
**				start condition can never be generated and
**				timed out.
**
*****************************************************************************/
uint32_t I2CEngine( void )
{
  RdIndex = 0;
  WrIndex = 0;

  /*--- Issue a start condition ---*/
  LPC_I2C->CONSET = I2CONSET_STA;	/* Set Start flag */

  I2CMasterState = I2C_BUSY;

  while ( I2CMasterState == I2C_BUSY )
  {
	if ( timeout >= MAX_TIMEOUT )
	{
	  I2CMasterState = I2C_TIME_OUT;
	  break;
	}
	timeout++;
  }
  LPC_I2C->CONCLR = I2CONCLR_STAC;

  return ( I2CMasterState );
}
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/
