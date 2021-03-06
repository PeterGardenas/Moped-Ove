
#ifndef CANIF_PBCFG_H_
#define CANIF_PBCFG_H_

#if !(((CANIF_SW_MAJOR_VERSION == 3) && (CANIF_SW_MINOR_VERSION == 0)) )
#error CanIf: Configuration file expected BSW module version to be 3.0.*
#endif

#if !(((CANIF_AR_RELEASE_MAJOR_VERSION == 4) && (CANIF_AR_RELEASE_MINOR_VERSION == 0)) )
#error CanIf: Configuration file expected AUTOSAR version to be 4.0.*
#endif




//Number of tx l-pdus
#define CANIF_NUM_TX_LPDU	3

#define CANIF_PDU_ID_PLUGININSTALLATIONVCUPDU		0
#define CANIF_PDU_ID_PLUGINCOMMUNICATIONTCUTOVCUPDU		1
#define CANIF_PDU_ID_DRIVERSETSPEEDSTEERINGPDU		2
#define CANIF_PDU_ID_PLUGINCOMMUNICATIONSCUTOVCUPDU		3
#define CANIF_PDU_ID_POISTIONDATAPDU		4

#define CANIF_PDU_ID_ACKNOWLEDGEMENTVCUPDU		0
#define CANIF_PDU_ID_PLUGINCOMMUNICATIONVCUTOTCUPDU		1
#define CANIF_PDU_ID_PLUGINCOMMUNICATIONVCUTOSCUPDU		2


#endif
