#ifndef __PHLPCD_H_
#define __PHLPCD_H_

#include "public.h"
#include "ph_Status.h"
#include "phhalHw_Rc663_Config.h"

#define PH_LPCD_GUARDTIME 350  // ms
#define PH_LPCD_RFON_TIME 10 //us

typedef struct
{
	uint16_t wId;                                       /**< Layer ID for this component, NEVER MODIFY! */
	void  * pHalDataParams; /**< Pointer to the parameter structure of the underlying layer. */
	uint8_t bValueI;
	uint8_t bValueQ;
	uint8_t bChargePump;
	uint8_t bDigitalFilter;
	uint16_t wDetectionOption;
	uint8_t bTCOption;
	uint8_t bTCScenario;
	uint8_t bLoopOptions;
	uint8_t bLoopScenarios;
	uint16_t wPowerDownTimeMs;
	uint16_t wDetectionTimeUs;
	//uint32_t dwInput;
	uint8_t bStage;
	uint8_t bRxAnaBackup;

}halHw_Lpcd_DataParams_t;

phStatus_t nfc_lpcd_init(void);

phStatus_t phApp_ConfigureLPCD(void);

phStatus_t phhalHw_Rc663_Lpcd_SetConfig(phhalHw_Rc663_DataParams_t * pDataParams,uint8_t bMode,uint8_t bI,
																				uint8_t bQ,uint16_t wPowerDownTimeMs,uint16_t wDetectionTimeUs);
phStatus_t phhalHw_Rc663_Lpcd_GetConfig(phhalHw_Rc663_DataParams_t * pDataParams,uint8_t * pI,uint8_t * pQ);

phStatus_t phhalHw_Lpcd_Cmd( phhalHw_Rc663_DataParams_t * pDataParams);

#endif //__PHLPCD_H_
