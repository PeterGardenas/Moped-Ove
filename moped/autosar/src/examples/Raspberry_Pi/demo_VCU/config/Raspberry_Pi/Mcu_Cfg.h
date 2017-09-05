/*
* Configuration of module: Mcu (Mcu_Cfg.h)
*
* Created by:              
* Copyright:               
*
* Configured for (MCU):    STM32_F107
*
* Module vendor:           ArcCore
* Generator version:       1.0.1
*
* Generated by Arctic Studio (http://arccore.com) 
*/


#if !(((MCU_SW_MAJOR_VERSION == 1) && (MCU_SW_MINOR_VERSION == 0)) )
#error Mcu: Configuration file expected BSW module version to be 1.0.X*
#endif


#ifndef MCU_CFG_H_
#define MCU_CFG_H_

#include "Std_Types.h"
//#include "Mcu_ConfigTypes.h"

#define MCU_DEV_ERROR_DETECT 	STD_OFF
#define MCU_PERFORM_RESET_API 	STD_OFF
#define MCU_VERSION_INFO_API 	STD_ON

typedef enum {
  MCU_CLOCKTYPE_EXT_REF_16MHZ = 0,
  MCU_NBR_OF_CLOCKS,
} Mcu_ClockType;

typedef enum {
	MCU_MODE_RUN = 0,
	MCU_MODE_SLEEP = 1,
	MCU_MODE_NORMAL = 2, // Here for backwards compatibility reasons. Not used anymore.
} Mcu_ModeType;

#define MCU_NBR_OF_RAM_SECTIONS	0


#define MCU_DEFAULT_CONFIG McuConfigData[0]


#endif /*MCU_CFG_H_*/
