/*
* Configuration of module: EcuM (EcuM_Cfg.h)
*
* Created by:              
* Copyright:               
*
* Configured for (MCU):    STM32_F107
*
* Module vendor:           ArcCore
* Generator version:       2.1.11
*
* Generated by Arctic Studio (http://arccore.com) 
*/


#if !(((ECUM_SW_MAJOR_VERSION == 1) && (ECUM_SW_MINOR_VERSION == 0)) )
#error EcuM: Configuration file expected EcuMFixed module version to be 1.0.X*
#endif


#ifndef ECUM_CFG_H_
#define ECUM_CFG_H_

#define ECUM_VERSION_INFO_API	STD_OFF
#define ECUM_DEV_ERROR_DETECT	STD_OFF

#define ECUM_MAIN_FUNCTION_PERIOD  (200)
#define ECUM_NVRAM_READALL_TIMEOUT (10000)
#define ECUM_NVRAM_WRITEALL_TIMEOUT (10000)
#define ECUM_NVRAM_MIN_RUN_DURATION (10000)

#define ECUM_VALIDATION_TIMEOUT	  0

typedef enum {
	ECUM_USER_User_1,
	ECUM_USER_ENDMARK	// Must be the last in list!
} EcuM_UserList;

// EcuM Sleep Mode IDs
#define ECUM_SLEEP_MODE_ECUMSLEEPMODE 0
#define ECUM_SLEEP_MODE_CNT 1

typedef enum {
	ECUM_WKSOURCE_POWER = (1<<0),
	ECUM_WKSOURCE_RESET = (1<<1),
	ECUM_WKSOURCE_INTERNAL_RESET = (1<<2),
	ECUM_WKSOURCE_INTERNAL_WDG = (1<<3),
	ECUM_WKSOURCE_EXTERNAL_WDG = (1<<4),
	ECUM_WKSOURCE_ECUMWAKEUPSOURCE = (1<<5),	
	ECUM_WKSOURCE_ALL_SOURCES = 0x3FFFFFFF
} EcuM_WakeupSourceType ;

#define ECUM_WKSOURCE_USER_CNT 1

#endif /*ECUM_CFG_H_*/

