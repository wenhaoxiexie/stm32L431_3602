#include "nfc9883.h"
#include "global.h"
#include "win6170.h"
#include "nfc.h"
//#include "phlpcd.h"

phbalReg_Stub_DataParams_t balReader;
phhalHw_Rc663_DataParams_t halReader;

phpalI14443p3a_Sw_DataParams_t I14443p3a;
phpalI14443p3b_Sw_DataParams_t  I14443p3b;

phpalI14443p4_Sw_DataParams_t I14443p4;
phpalMifare_Sw_DataParams_t palMifare;
phKeyStore_Rc663_DataParams_t Rc663keyStore;
phalMfc_Sw_DataParams_t alMfc;
phStatus_t status;
void *pHal;
uint8_t bSak[1];
uint8_t bUid[13];
uint8_t bMoreCardsAvailable;
uint8_t bLength;
uint8_t i;

 uint8_t pI=0;
 uint8_t pQ=0;

uint8_t bHalBufferReader[0x40];
uint8_t bBufferReader[0x60];

//extern uint8_t bRxAnaBackup_bak;
uint8_t permision;
uint16_t nfc9883_init(void)
{
	/* Perform a hardware reset */
	Reset_RC663_device();
	/* Initialize the Reader BAL (Bus Abstraction Layer) component */
	phbalReg_Stub_Init(&balReader, sizeof(phbalReg_Stub_DataParams_t));
	/* Initialize the Reader HAL (Hardware Abstraction Layer) component */
	status = phhalHw_Rc663_Init(&halReader,sizeof(phhalHw_Rc663_DataParams_t),&balReader,0,bHalBufferReader,sizeof(bHalBufferReader),bHalBufferReader,sizeof(bHalBufferReader));
	/* Set the parameter to use the SPI interface */
	halReader.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;
	/* Set the generic pointer */
	pHal = &halReader;
	/* Initializing specific objects for the communication with
	 * Mifare (R) Classic cards.
	 * The Mifare (R) Classic card is compliant of
	 * ISO 14443-3 and ISO 14443-4
	*/
	
//	// 关闭射频场
//	PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldOff(pHal));
//	// 开启射频场
//	PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldOn(pHal));
//	
	/* Initialize the 14443-3A PAL (Protocol Abstraction Layer) component */
	PH_CHECK_SUCCESS_FCT(status, phpalI14443p3a_Sw_Init(&I14443p3a,sizeof(phpalI14443p3a_Sw_DataParams_t), pHal));
	PH_CHECK_SUCCESS_FCT(status, phpalI14443p3b_Sw_Init(&I14443p3b,sizeof(phpalI14443p3b_Sw_DataParams_t), pHal));
	/* Initialize the 14443-4 PAL component */
	PH_CHECK_SUCCESS_FCT(status, phpalI14443p4_Sw_Init(&I14443p4,sizeof(phpalI14443p4_Sw_DataParams_t), pHal));
	/* Initialize the Mifare PAL component */
	PH_CHECK_SUCCESS_FCT(status, phpalMifare_Sw_Init(&palMifare,sizeof(phpalMifare_Sw_DataParams_t), pHal, &I14443p4));
	/* Initialize the keystore component */
	PH_CHECK_SUCCESS_FCT(status, phKeyStore_Rc663_Init(&Rc663keyStore,sizeof(phKeyStore_Rc663_DataParams_t), pHal));
	/* Initialize the Mifare (R) Classic AL component - set NULL because
	   * the keys are loaded in E2 by the function */
	/* phKeyStore_SetKey */
	PH_CHECK_SUCCESS_FCT(status, phalMfc_Sw_Init(&alMfc,sizeof(phalMfc_Sw_DataParams_t), &palMifare,NULL));

	/* SoftReset the IC.The SoftReset only resets the RC663 to EEPROM configuration. */
	PH_CHECK_SUCCESS_FCT(status, phhalHw_Rc663_Cmd_SoftReset(pHal));

	/* Read the version of the reader IC */
	PH_CHECK_SUCCESS_FCT(status, phhalHw_ReadRegister(&halReader,PHHAL_HW_RC663_REG_VERSION, bBufferReader));

	/* Reset the Rf field */
	PH_CHECK_SUCCESS_FCT(status, phhalHw_FieldReset(pHal));

	/* Apply the type A protocol settings and activate the RF field. */
	PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(pHal,PHHAL_HW_CARDTYPE_ISO14443A));
	/* Activate the communication layer part 3 of the ISO 14443A standard. */
	
	// lpcd
	status = phhalHw_Rc663_Cmd_LpcdConfig(&halReader,&pI,&pQ);
	
	debug_printf(" $$$$$$$$$$ st:%x  [%x %x]\r\n",status,pI,pQ);

	status = phhalHw_Rc663_Cmd_Lpcd_Set(&halReader,PHHAL_HW_RC663_CMD_LPCD_MODE_POWERDOWN,pI,pQ,300,5);
	
	debug_printf(" $$$$$$$$$$ st:%x  [%x %x]\r\n",status,pI,pQ);
	
	return status;
}

uint16_t detect_card(void)
{
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		/* Read the version of the reader IC */
		PH_CHECK_SUCCESS_FCT(status, phhalHw_ReadRegister(&halReader,PHHAL_HW_RC663_REG_VERSION, bBufferReader));
	
		PH_CHECK_SUCCESS_FCT(status, phhalHw_ReadRegister(&halReader,PHHAL_HW_RC663_REG_IRQ0, bBufferReader));
		
		PH_CHECK_SUCCESS_FCT(status, phhalHw_ReadRegister(&halReader,PHHAL_HW_RC663_REG_IRQ1, bBufferReader));
		do
		{
				status = phhalHw_WriteRegister(pHal, PHHAL_HW_RC663_REG_COMMAND, PHHAL_HW_RC663_CMD_IDLE);
		}
		while ((status & PH_ERR_MASK) == PH_ERR_IO_TIMEOUT);
	
		/* Clear IRQ0 interrupt sources */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_WriteRegister(pHal, PHHAL_HW_RC663_REG_IRQ0EN, PHHAL_HW_RC663_BIT_IRQINV));

    /* Clear IRQ1 interrupt sources */
    PH_CHECK_SUCCESS_FCT(status, phhalHw_WriteRegister(pHal, PHHAL_HW_RC663_REG_IRQ1EN, 0x00));
		
		// Type A 类型卡卡片
		/* Apply the type A protocol settings and activate the RF field. */
		PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(pHal,PHHAL_HW_CARDTYPE_ISO14443A));
		status = phpalI14443p3a_ActivateCard(&I14443p3a, NULL, 0x00, bUid, &bLength, bSak, &bMoreCardsAvailable);
	
		if(status==0)
		{
			for(i=0;i<bLength;i++)
			{
				nfc_temp.guid[i]=bUid[i];
				debug_printf("%02x \r\n",nfc_temp.guid[i]);
			}
			nfc_temp.len=bLength;
			nfc_temp.irq_flag=1;
			
		}else{
			PH_CHECK_SUCCESS_FCT(status, phhalHw_ApplyProtocolSettings(pHal,PHHAL_HW_CARDTYPE_ISO14443B));
			status =	phpalI14443p3b_RequestB(&I14443p3b,0x00,0x00,0x00,bUid,&bLength);			
			if(0==status)
			{
				for(i=0;i<bLength;i++)
				{
					nfc_temp.guid[i]=bUid[i];
					debug_printf("%02x \r\n",nfc_temp.guid[i]);
				}
				nfc_temp.len=bLength;
				nfc_temp.irq_flag=1;
			}
		}

		// lpcd
	PH_CHECK_SUCCESS_FCT(status, phhalHw_WriteRegister(&halReader,PHHAL_HW_RC663_REG_IRQ0, 0x00));
	PH_CHECK_SUCCESS_FCT(status, phhalHw_WriteRegister(&halReader,PHHAL_HW_RC663_REG_IRQ1, 0x00));


	status = phhalHw_Rc663_Cmd_LpcdConfig(&halReader,&pI,&pQ);
	debug_printf(" &&&&&&&&&&&&&&&&&&&&&&&&&&& st:%x  [%x %x]\r\n",status,pI,pQ);

	status = phhalHw_Rc663_Cmd_Lpcd_Set(&halReader,PHHAL_HW_RC663_CMD_LPCD_MODE_POWERDOWN,pI,pQ,300,5);
	debug_printf(" enter LPCD mode... st:%x  \r\n",status);
	
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	return status;
}

