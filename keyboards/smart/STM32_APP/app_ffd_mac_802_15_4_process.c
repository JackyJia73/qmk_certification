/**
  ******************************************************************************
* @file    app_mac_802-15-4_process.c
* @author  MDG-RF Team
* @brief   MAC Cli Implementation for Demo Purpose
*
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "app_ffd_mac_802_15_4_process.h"

//#include "app_conf.h"
#include "shci.h"
#include "stm32_seq.h"
#include "app_ffd_mac_802_15_4.h"
#include "802_15_4_mac_sap.h"
//#include "stm_logging.h"

#include "ch.h"
#include "hal.h"

/* Global define -------------------------------------------------------------*/
#ifndef FALSE
#define FALSE false
#endif
#ifndef TRUE
#define TRUE true
#endif

#define WORK_AS_APP_RFD

/* Global variables ----------------------------------------------------------*/

int volatile FrameOnGoing = FALSE;

extern MAC_associateCnf_t g_MAC_associateCnf;
extern uint8_t led_state_from_dongle;

/* Private defines -----------------------------------------------------------*/
#define MAX_PIB_ATTRIBUTE_VALUE_LENGTH 52

#define DATA "HELLO NODE\0"

/* Private macros ------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, 
                             uint8_t sign, uint8_t expectedRes);
/* Public variables ----------------------------------------------------------*/

extern MAC_associateInd_t g_MAC_associateInd;
MAC_dataInd_t      g_DataInd;

extern void HID_SW_Callback(uint8_t ch);

/**
* Init
*/

/******************************************************************************/


/******************************************************************************/


/* callbacks to be called by the MAC
* Create an NTS message and enqueue it in the IPCC queue
*/

MAC_Status_t APP_MAC_mlmeAssociateCnfCb( const  MAC_associateCnf_t * pAssociateCnf )
{
#ifdef WORK_AS_APP_RFD
	if(app_ffd_task_index() != MAC_SETUP_TASK_SET_ASSO_REQUEST){
		return MAC_ERROR;
	}

//	APP_DBG("RFD MAC APP - Association CNF Received");
	memset(&g_MAC_associateCnf,0x00,sizeof(MAC_associateCnf_t));
	memcpy(&g_MAC_associateCnf,pAssociateCnf,sizeof(MAC_associateCnf_t));

	return app_rfd_mac_set_allocate_short_addr();
#else
  return MAC_NOT_IMPLEMENTED_STATUS;
#endif /* WORK_AS_APP_RFD */
}

MAC_Status_t APP_MAC_mlmeAssociateIndCb( const  MAC_associateInd_t * pAssociateInd )
{
#ifdef WORK_AS_APP_RFD
	return MAC_ERROR;
#else
  /* Store ASSOCIATION IND */
  MAC_Status_t MacStatus = MAC_ERROR;

  MAC_associateRes_t AssociateRes;

//  APP_DBG_MSG("FFD MAC APP - Association Requested");

  uint16_t shortAssociationAddr = 0x3344;

  memcpy(&g_MAC_associateInd,pAssociateInd,sizeof(MAC_associateInd_t));

  memcpy(AssociateRes.a_device_address,g_MAC_associateInd.a_device_address,0x08);
  memcpy(AssociateRes.a_assoc_short_address,&shortAssociationAddr,0x02);
  AssociateRes.security_level = 0x00;
  AssociateRes.status = MAC_SUCCESS;

  MacStatus = MAC_MLMEAssociateRes(&AssociateRes);
  if ( MAC_SUCCESS != MacStatus ) {
//   APP_DBG_MSG("FFD MAC - Association Response Fails");
  }
  return MAC_SUCCESS;
#endif /* WORK_AS_APP_RFD */
}

MAC_Status_t APP_MAC_mlmeBeaconNotifyIndCb( const  MAC_beaconNotifyInd_t * pBeaconNotifyInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeCommStatusIndCb( const  MAC_commStatusInd_t * pCommStatusInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeDisassociateCnfCb( const  MAC_disassociateCnf_t * pDisassociateCnf )
{

  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeDisassociateIndCb( const  MAC_disassociateInd_t * pDisassociateInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;

}

MAC_Status_t APP_MAC_mlmeGetCnfCb( const  MAC_getCnf_t * pGetCnf )
{
  return MAC_NOT_IMPLEMENTED_STATUS;

}

MAC_Status_t APP_MAC_mlmeOrphanIndCb( const  MAC_orphanInd_t * pOrphanInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;

}

MAC_Status_t APP_MAC_mlmePollCnfCb( const  MAC_pollCnf_t * pPollCnf )
{
  return MAC_NOT_IMPLEMENTED_STATUS;

}

MAC_Status_t APP_MAC_mlmeResetCnfCb( const  MAC_resetCnf_t * pResetCnf )
{
//  UTIL_SEQ_SetEvt(EVENT_DEVICE_RESET_CNF);
//	  return MAC_SUCCESS;
	MAC_SetupTask_t task = app_ffd_task_index();

	switch(task){
	case MAC_SETUP_TASK_RESET:
#ifdef WORK_AS_APP_RFD
		return app_rfd_mac_set_ext_addr(); /* next task */
#else
		return app_ffd_mac_set_ext_addr(); /* next task */
#endif /* WORK_AS_APP_RFD */
	case MAC_SETUP_TASK_STOP:
		/* nothing */
		return MAC_SUCCESS;
	default:
		return MAC_ERROR;
	}
}


MAC_Status_t APP_MAC_mlmeRxEnableCnfCb( const  MAC_rxEnableCnf_t * pRxEnableCnf )
{
  return MAC_NOT_IMPLEMENTED_STATUS;

}

MAC_Status_t APP_MAC_mlmeScanCnfCb( const  MAC_scanCnf_t * pScanCnf )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}


MAC_Status_t APP_MAC_mlmeSetCnfCb( const  MAC_setCnf_t * pSetCnf )
{
//  UTIL_SEQ_SetEvt(EVENT_SET_CNF);
//  return MAC_SUCCESS;
	MAC_SetupTask_t task = app_ffd_task_index();

	switch(task){
	case MAC_SETUP_TASK_SET_EXT_ADDR:
#ifdef WORK_AS_APP_RFD
		return app_rfd_mac_set_asso_request();  /* next task */
#else
		return app_ffd_mac_set_short_addr();  /* next task */
#endif /* WORK_AS_APP_RFD */
	case MAC_SETUP_TASK_SET_SHORT_ADDR:
		return app_ffd_mac_set_asso_permit(); /* next task */
	case MAC_SETUP_TASK_SET_ASSO_PERMIT:
		return app_ffd_mac_set_tx_power();    /* next task */
	case MAC_SETUP_TASK_SET_TX_POWER:
		return app_ffd_mac_start_device();    /* next task */
	case MAC_SETUP_TASK_SET_ALLOCATE_SHORT_ADDR:
//		return app_rfd_mac_complete();        /* complete */
		return app_ffd_mac_set_rx();
	case MAC_SETUP_TASK_SET_RX_ON_WHEN_IDEL:
		return app_ffd_mac_complete();        /* complete */
	default:
		return MAC_ERROR;
	}
}


MAC_Status_t APP_MAC_mlmeStartCnfCb( const  MAC_startCnf_t * pStartCnf )
{
//  UTIL_SEQ_SetEvt(EVENT_DEVICE_STARTED_CNF);
//  return MAC_NOT_IMPLEMENTED_STATUS;
	if(app_ffd_task_index() != MAC_SETUP_TASK_START_DEVICE){
		return MAC_ERROR;
	}

	return app_ffd_mac_set_rx(); /* next task */
}

void APP_MAC_ReceiveData(void)
{
  static int cpt = 0;
  // Check validity of the received Message extracting associated 
  // simple xor signature
  if (!checkMsgXorSignature((char const *) (g_DataInd.msduPtr),
                             g_DataInd.msdu_length-1,
                             g_DataInd.msduPtr[g_DataInd.msdu_length-1],
                             0x00))
  {
//     APP_DBG_MSG("FFD MAC APP - ERROR : CORRUPTED RECEIVED DATA ");
  }
  else
  {
//	  uint8_t report_id = ((uint8_t*)(g_DataInd.msduPtr))[0];
	  cpt++;

	  led_state_from_dongle = ((uint8_t*)(g_DataInd.msduPtr))[1];

//    g_DataInd.msduPtr[g_DataInd.msdu_length-1] = '\0';//erase signature with EOS
//    APP_DBG_MSG("FFD MAC APP - RECEIVE DATA(%d) : %s ",cpt++,(char const *) g_DataInd.msduPtr);

//    HID_SW_Callback(g_DataInd.msduPtr[0]);
  }
  FrameOnGoing = FALSE;
}

MAC_Status_t APP_MAC_mcpsDataIndCb( const  MAC_dataInd_t * pDataInd )
{
    if (FrameOnGoing == FALSE)
    {
        FrameOnGoing = TRUE;
        memcpy(&g_DataInd,pDataInd,sizeof(MAC_dataInd_t));

        APP_MAC_ReceiveData();
//        UTIL_SEQ_SetTask(1<< CFG_TASK_RECEIVE_DATA,CFG_SCH_PRIO_0);
    }
    return MAC_SUCCESS;
}

MAC_Status_t APP_MAC_mcpsDataCnfCb( const  MAC_dataCnf_t * pDataCnf )
{
//  UTIL_SEQ_SetEvt( EVENT_DATA_CNF ); // TODO
  return MAC_SUCCESS;
}

MAC_Status_t APP_MAC_mcpsPurgeCnfCb( const  MAC_purgeCnf_t * pPurgeCnf )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeSyncLossIndCb( const MAC_syncLoss_t * syncLossPtr )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeCalibrateCnfCb( const MAC_calibrateCnf_t * pCallibrateCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeDpsCnfCb( const MAC_dpsCnf_t * pDpsCnf  )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeDpsIndCb( const MAC_dpsInd_t * pDpsInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeSoundingCnfCb( const MAC_soundingCnf_t * pSoudingCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeGtsCnfCb( const MAC_gtsCnf_t * pGtsCnf)
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmeGtsIndCb( const MAC_GtsInd_t * pGtsInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

MAC_Status_t APP_MAC_mlmePollIndCb( const MAC_pollInd_t * pPollInd )
{
  return MAC_NOT_IMPLEMENTED_STATUS;
}

/* Private function Definition -----------------------------------------------*/

uint8_t checkMsgXorSignature(const char * pMessage, uint8_t message_len, 
                             uint8_t sign, uint8_t expectedRes)
{
  uint8_t seed = sign;
  for (uint8_t i=0x00;i<message_len;i++)
    seed = (uint8_t)pMessage[i]^seed;
  return (seed==expectedRes);
}

/******************************************************************************/

//@} \\END CLI Commands

