/**
 * Application Header File
 *
 * @req SWS_Rte_01003
 */

/** === HEADER ====================================================================================
 */

/** --- C++ guard ---------------------------------------------------------------------------------
 * @req SWS_Rte_03709
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** --- Normal include guard ----------------------------------------------------------------------
 */
#ifndef RTE_FRONTWHEELSWC_H_
#define RTE_FRONTWHEELSWC_H_

/** --- Duplicate application include guard -------------------------------------------------------
 * @req SWS_Rte_01006
 */
#ifdef RTE_APPLICATION_HEADER_FILE
#error Multiple application header files included.
#endif
#define RTE_APPLICATION_HEADER_FILE

/** --- Single runnable API -----------------------------------------------------------------------
 * @req SWS_Rte_02751
 */
#if defined(RTE_RUNNABLEAPI_FrontWheelSensorWriteRunnable) || \
defined(RTE_RUNNABLEAPI_FrontWheelSensorReadRunnable)
#define RTE_RUNNABLEAPI
#endif

/** --- Includes ----------------------------------------------------------------------------------
 * @req SWS_Rte_02751
 * @req SWS_Rte_07131
 */
#include <Rte_DataHandleType.h>
#include <Rte_FrontWheelSWC_Type.h>

/** --- PIM DATA TYPES ------------------------------------------------------------------------------ */

/** === BODY ======================================================================================
 */

/** @req SWS_Rte_03731
 *  @req SWS_Rte_07137
 *  @req SWS_Rte_07138
 *  !req SWS_Rte_06523
 *  @req SWS_Rte_03730
 *  @req SWS_Rte_07677
 *  @req SWS_Rte_02620
 *  @req SWS_Rte_02621
 *  @req SWS_Rte_01055
 *  @req SWS_Rte_03726 */

/** @req SWS_Rte_01343
 *  @req SWS_Rte_01342
 *  !req SWS_Rte_06524
 *  @req SWS_Rte_01053
 */

extern Std_ReturnType Rte_Call_FrontWheelSWC_FrontWheelSWCProto_FrontWheelSwcReadDataFromWheelSensorPort2_Read(/*IN*/UInt32* Data);

/** @req SWS_Rte_07132
 *  @req SWS_Rte_03714 */
typedef struct {
    Rte_DE_UInt32 * const FrontWheelSensorWriteRunnable_FrontWheelSwcWriteDataToPirteSwcPort1_dataInt32;
} Rte_CDS_FrontWheelSWC;

/** --- Instance handle type ---------------------------------------------------------------------- */
typedef Rte_CDS_FrontWheelSWC const * const Rte_Instance;

/** --- Singleton instance handle -----------------------------------------------------------------
 *  @req SWS_Rte_03793
 */
extern const Rte_Instance Rte_Inst_FrontWheelSWC;
#define self (Rte_Inst_FrontWheelSWC)

/** --- Calibration API --------------------------------------------------------------------------- */

/** --- Per Instance Memory API ------------------------------------------------------------------- */

/** --- Single Runnable APIs ---------------------------------------------------------------------- */
#if defined(RTE_RUNNABLEAPI)
/** --- FrontWheelSensorWriteRunnable */
#if defined(RTE_RUNNABLEAPI_FrontWheelSensorWriteRunnable)

void FrontWheelSensorWriteRunnable(void);

static inline void Rte_IWrite_FrontWheelSensorWriteRunnable_FrontWheelSwcWriteDataToPirteSwcPort1_dataInt32(/*IN*/UInt32 value) {
    self->FrontWheelSensorWriteRunnable_FrontWheelSwcWriteDataToPirteSwcPort1_dataInt32->value = value;
}

#endif

/** --- FrontWheelSensorReadRunnable */
#if defined(RTE_RUNNABLEAPI_FrontWheelSensorReadRunnable)

void FrontWheelSensorReadRunnable(void);

static inline Std_ReturnType Rte_Call_FrontWheelSwcReadDataFromWheelSensorPort2_Read(/*IN*/UInt32 Data) {
    return Rte_Call_FrontWheelSWC_FrontWheelSWCProto_FrontWheelSwcReadDataFromWheelSensorPort2_Read(Data);
}

#endif
#endif

/** --- All Runnable APIs ------------------------------------------------------------------------- */
#if !defined(RTE_RUNNABLEAPI)
void FrontWheelSensorWriteRunnable(void);

static inline void Rte_IWrite_FrontWheelSensorWriteRunnable_FrontWheelSwcWriteDataToPirteSwcPort1_dataInt32(/*IN*/UInt32 value) {
    self->FrontWheelSensorWriteRunnable_FrontWheelSwcWriteDataToPirteSwcPort1_dataInt32->value = value;
}

void FrontWheelSensorReadRunnable(void);

static inline Std_ReturnType Rte_Call_FrontWheelSwcReadDataFromWheelSensorPort2_Read(/*IN*/UInt32* Data) {
    return Rte_Call_FrontWheelSWC_FrontWheelSWCProto_FrontWheelSwcReadDataFromWheelSensorPort2_Read(Data);
}
#endif

/** === FOOTER ====================================================================================
 */

#endif /* RTE_FRONTWHEELSWC_H_ */

/** @req SWS_Rte_03710 */
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
