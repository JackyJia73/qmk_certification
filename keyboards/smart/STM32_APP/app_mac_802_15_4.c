/*
 * app_mac_802_15_4.c
 *
 *  Created on: Nov 13, 2023
 *      Author: yuyw
 */

/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32wbxx.h"

#include "st_wireless.h"

#include "stm32_seq.h"

#include "utilities_common.h"

#include "app_ffd_mac_802_15_4.h"

#include "802_15_4_mac_sap.h"
#include "app_ffd_mac_802_15_4_process.h"

#include "shci.h"

#include "ch.h"
#include "hal.h"
#include "report.h"
#include "keycode_config.h"

/* Defines -----------------------------------------------*/
#define PRINT_NO_MESG(...)

#define APP_DBG_MSG      PRINT_NO_MESG


MAC_callbacks_t macCbConfig ;

static APP_MAC_ConnectState_t connect_state = APP_MAC_UNKNOWN;
static uint8_t led_state_from_dongle=0;

static void APP_FFD_MAC_802_15_4_Config(void);


static void APP_FFD_MAC_802_15_4_Config(void)
{
  APP_DBG_MSG("configure FFD MAC 802.15.4 - 2");
  /* Register MAC 802.15.4 callback functions */

  memset(&macCbConfig,0x00,sizeof(MAC_callbacks_t));

#if 0
  /* Mac Call Back Initialization */
  macCbConfig.mlmeResetCnfCb = APP_MAC_mlmeResetCnfCb;
  macCbConfig.mlmeScanCnfCb = APP_MAC_mlmeScanCnfCb;
  macCbConfig.mlmeAssociateCnfCb = APP_MAC_mlmeAssociateCnfCb;
  macCbConfig.mlmeAssociateIndCb = APP_MAC_mlmeAssociateIndCb;
  macCbConfig.mlmeBeaconNotifyIndCb = APP_MAC_mlmeBeaconNotifyIndCb;
  macCbConfig.mlmeCalibrateCnfCb = APP_MAC_mlmeCalibrateCnfCb;
  macCbConfig.mlmeCommStatusIndCb = APP_MAC_mlmeCommStatusIndCb;
  macCbConfig.mlmeDisassociateCnfCb = APP_MAC_mlmeDisassociateCnfCb;
  macCbConfig.mlmeDisassociateIndCb = APP_MAC_mlmeDisassociateIndCb;
  macCbConfig.mlmeDpsCnfCb = APP_MAC_mlmeDpsCnfCb;
  macCbConfig.mlmeDpsIndCb = APP_MAC_mlmeDpsIndCb;
  macCbConfig.mlmeGetCnfCb = APP_MAC_mlmeGetCnfCb;
  macCbConfig.mlmeGtsCnfCb = APP_MAC_mlmeGtsCnfCb;
  macCbConfig.mlmeGtsIndCb = APP_MAC_mlmeGtsIndCb;
  macCbConfig.mlmeOrphanIndCb = APP_MAC_mlmeOrphanIndCb;
  macCbConfig.mlmePollCnfCb = APP_MAC_mlmePollCnfCb;
  macCbConfig.mlmeRxEnableCnfCb = APP_MAC_mlmeRxEnableCnfCb;
  macCbConfig.mlmeSetCnfCb = APP_MAC_mlmeSetCnfCb;
  macCbConfig.mlmeSoundingCnfCb = APP_MAC_mlmeSoundingCnfCb;
  macCbConfig.mlmeStartCnfCb = APP_MAC_mlmeStartCnfCb;
  macCbConfig.mlmeSyncLossIndCb = APP_MAC_mlmeSyncLossIndCb;
  macCbConfig.mcpsDataIndCb = APP_MAC_mcpsDataIndCb;
  macCbConfig.mcpsDataCnfCb = APP_MAC_mcpsDataCnfCb;
  macCbConfig.mcpsPurgeCnfCb = APP_MAC_mcpsPurgeCnfCb;
  macCbConfig.mlmePollIndCb = APP_MAC_mlmePollIndCb;
#endif
}

void APP_FFD_MAC_802_15_4_Init( APP_MAC_802_15_4_InitMode_t InitMode, TL_CmdPacket_t* pCmdBuffer){
	// nothing but init macCbConfig
	APP_FFD_MAC_802_15_4_Config();
}

void APP_FFD_MAC_802_15_4_Stop(){
	// nothing
}

APP_MAC_ConnectState_t APP_MAC_ConnectState(void){
	return connect_state;
}

uint8_t MACAPP_Get_LED_Report(void){
	return led_state_from_dongle;
}

void MACAPP_Report_Keyboard(report_keyboard_t *report){
	// nothing
}

void MACAPP_Report_NKRO(report_nkro_t *report){
	// nothing
}

void MACAPP_Report_Extra(report_extra_t *report){
	// nothing
}

void MACAPP_Report_Mouse(report_mouse_t *report){
	// nothing
}
