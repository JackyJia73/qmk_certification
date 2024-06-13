/**
  ******************************************************************************
 * @file    app_ffd_mac_802_15_4.c
 * @author  MCD Application Team
 * @brief   Application based on MAC 802.15.4
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "board.h"
#include "stm32wbxx.h"

#include "st_wireless.h"

//#include "ble_dbg_conf.h"
//#include "app_conf.h"
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

uint8_t xorSign( const uint8_t * pmessage, uint8_t message_len);

/* Channel in use */
#define DEMO_CHANNEL 16

/* Private function prototypes -----------------------------------------------*/
static void APP_FFD_MAC_802_15_4_Config(void);
static void MACAPP_Report(uint8_t *data, uint8_t sz);

/* variables -----------------------------------------------*/
MAC_associateInd_t g_MAC_associateInd;
MAC_callbacks_t macCbConfig ;
uint8_t g_srvSerReq;
uint8_t g_srvDataReq;
volatile ITStatus CertifOutputPeripheralReady = SET;

MAC_associateCnf_t g_MAC_associateCnf;

uint8_t led_state_from_dongle=0;

static const uint16_t     g_panId             = 0x1AAA;
static const uint16_t     g_ShortAddr         = 0x3344;
static const uint16_t     g_coordShortAddr    = 0x1122;
static       uint8_t      g_dataHandle        = 0x02;
static const long long    g_extAddr           = 0xACDE480000000002;
static const uint8_t      g_channel           = DEMO_CHANNEL;
static const uint8_t      g_channel_page      = 0x00;

static uint8_t rfBuffer[256];

static MAC_SetupTask_t task_index = MAC_SETUP_TASK_NONE;

static APP_MAC_ConnectState_t connect_state = APP_MAC_UNKNOWN;

/*
 * Thread for send data via MAC
 */
#define SEND_DATA_MAX_SIZE    40 /* NOT including the 4 bytes for Sync Word, Message Type, Data Size and End Code */
#define LIST_SEND_DATA_SIZE   20
uint8_t mac_send_data[LIST_SEND_DATA_SIZE][SEND_DATA_MAX_SIZE + 4];
msg_t idx_mac_send_data;

#define MB_SIZE 16

static msg_t mb_buffer[MB_SIZE];
static MAILBOX_DECL(mb_mac_send, mb_buffer, MB_SIZE);

static thread_t *th_mac_send;

static THD_WORKING_AREA(waThreadMacSend, 256);
static THD_FUNCTION(ThreadMacSend, arg) {
	mailbox_t *mb = (mailbox_t*)arg;
	msg_t msg_result, msg_addr;
	uint8_t *data, *msg;
	uint8_t sz;
	uint8_t report_id;

	chMBReset(mb);
	chMBResumeX(mb);

	while(1){
		msg_result = chMBFetchTimeout(mb, &msg_addr, TIME_INFINITE);

		if(msg_result != MSG_OK) continue;

		/* analysis the received message from mailbox */
		msg = (uint8_t*)msg_addr;
		sz = msg[0];
		data = msg + 1;

		/* send data */
		MACAPP_Report(data, sz);

		report_id = data[3];

		if(report_id == REPORT_ID_MOUSE){
			chThdSleepMilliseconds(10);
		}
	}
}
static thread_descriptor_t td_mac_send = {
	"mac_send",                             /* thread name */
	THD_WORKING_AREA_BASE(waThreadMacSend), /* Pointer to the working area base. */
	THD_WORKING_AREA_END(waThreadMacSend),  /* Pointer to the working area end. */
	HIGHPRIO,                               /* Thread priority, will be defined later */
	ThreadMacSend,                          /* Thread function pointer */
	NULL                                    /* Thread argument, will be defined later */
};


/* Functions Definition ------------------------------------------------------*/

void APP_FFD_MAC_802_15_4_Init( APP_MAC_802_15_4_InitMode_t InitMode, TL_CmdPacket_t* pCmdBuffer)
{
  led_state_from_dongle = 0;

  idx_mac_send_data = 0;

  chMBObjectInit(&mb_mac_send, mb_buffer, MB_SIZE);

  td_mac_send.arg = &mb_mac_send;
  th_mac_send = chThdCreate(&td_mac_send);

  /* Register cmdbuffer */
  APP_ENTRY_RegisterCmdBuffer(pCmdBuffer);

  /* Init config buffer and call TL_MAC_Init */
  APP_ENTRY_TL_MAC_802_15_4_Init();

  /* Send MAC_802_15_4 start system cmd to RF Core */
  SHCI_C2_MAC_802_15_4_Init();

  /* Register task */
  /* Create the different tasks */
//  UTIL_SEQ_RegTask( 1<<CFG_TASK_MSG_FROM_RF_CORE, UTIL_SEQ_RFU, APP_ENTRY_ProcessMsgFromRFCoreTask);
//  UTIL_SEQ_RegTask( 1<<CFG_TASK_FFD, UTIL_SEQ_RFU,APP_FFD_MAC_802_15_4_SetupTask);
//  UTIL_SEQ_RegTask( 1<<CFG_TASK_SERVICE_COORD, UTIL_SEQ_RFU,APP_FFD_MAC_802_15_4_CoordSrvTask);
//  UTIL_SEQ_RegTask( 1<<CFG_TASK_DATA_COORD, UTIL_SEQ_RFU,APP_FFD_MAC_802_15_4_CoordDataTask);
//  UTIL_SEQ_RegTask( 1<<CFG_TASK_RECEIVE_DATA, UTIL_SEQ_RFU,APP_MAC_ReceiveData);

  /* Configuration MAC 802_15_4 */
  APP_FFD_MAC_802_15_4_Config();

  /*Start Main Coordinator - FFD Task*/
//  UTIL_SEQ_SetTask( 1<< CFG_TASK_FFD, CFG_SCH_PRIO_0 );
  APP_FFD_MAC_802_15_4_SetupTask();
}

void APP_FFD_MAC_802_15_4_Stop()
{
  MAC_resetReq_t ResetReq;

  connect_state = APP_MAC_UNKNOWN;

  memset(&ResetReq,0x00,sizeof(MAC_resetReq_t));
  ResetReq.set_default_PIB = TRUE;

  task_index = MAC_SETUP_TASK_STOP;

  if (MAC_MLMEResetReq( &ResetReq ) != MAC_SUCCESS)
  {
      APP_DBG_MSG("APP_FFD_MAC_802_15_4_Stop ERROR");
  }
//  UTIL_SEQ_WaitEvt(EVENT_DEVICE_RESET_CNF);

  /* Pause  the different MAC tasks */
//  UTIL_SEQ_PauseTask( 1<<CFG_TASK_MSG_FROM_RF_CORE);
//  UTIL_SEQ_PauseTask( 1<<CFG_TASK_FFD);
//  UTIL_SEQ_PauseTask( 1<<CFG_TASK_SERVICE_COORD);
//  UTIL_SEQ_PauseTask( 1<<CFG_TASK_DATA_COORD);
//  UTIL_SEQ_PauseTask( 1<<CFG_TASK_RECEIVE_DATA);

  //BSP_LED_Off(LED_BLUE);
}


void APP_FFD_MAC_802_15_4_CoordSrvTask(void)
{
  MAC_Status_t MacStatus = MAC_ERROR;

  MAC_associateRes_t AssociateRes;
  uint16_t shortAssociationAddr = 0x3344;

  APP_DBG_MSG("Srv task :");

  switch (g_srvSerReq)
  {
  case CFG_ASSO_PENDING:
    APP_DBG_MSG("Srv task : Response to Association Indication");

    memcpy(AssociateRes.a_device_address,g_MAC_associateInd.a_device_address,0x08);
    memcpy(AssociateRes.a_assoc_short_address,&shortAssociationAddr,0x2);
    AssociateRes.security_level = 0x00;
    AssociateRes.status = MAC_SUCCESS;

    MacStatus = MAC_MLMEAssociateRes(&AssociateRes);
    if ( MAC_SUCCESS != MacStatus ) {
      APP_DBG_MSG("FFD MAC - Association Response Fails");
      return;
    }
    break;
  default:
    APP_DBG_MSG("Srv task : unknown Request");
  }
  g_srvSerReq = CFG_SRV_SER_REQ_NBR;
}

void APP_FFD_MAC_802_15_4_CoordDataTask(void)
{
  APP_DBG_MSG("Data task :");
  switch (g_srvDataReq)
  {
    case CFG_DATA_PENDING:
      //BSP_LED_Toggle(LED3);
      APP_DBG_MSG("Data task : Data Indication Receive");
    default:
      APP_DBG_MSG("Data task : unknown Request");
  }
  g_srvDataReq = CFG_SRV_DATA_REQ_NBR;
}

MAC_Status_t app_ffd_mac_reset(void){
	/* Reset MAC */
	MAC_resetReq_t    ResetReq;

	memset(&ResetReq,0x00,sizeof(MAC_resetReq_t));
	ResetReq.set_default_PIB = TRUE;

	task_index = MAC_SETUP_TASK_RESET;

	if ( MAC_SUCCESS != MAC_MLMEResetReq( &ResetReq ) ) {
		APP_DBG_MSG("FFD MAC - Reset Fails");
		return MAC_ERROR;
	}

	return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_set_ext_addr(void){
  /* Set Device Extended Address */
  MAC_setReq_t      SetReq;

  APP_DBG_MSG("FFD MAC APP - Set Extended Address");
  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*) &g_extAddr;

  task_index = MAC_SETUP_TASK_SET_EXT_ADDR;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Extended Addr Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_set_short_addr(void){
  MAC_setReq_t      SetReq;

  /* Set Device Short Address */
  APP_DBG_MSG("FFD MAC APP - Set Short Address");
  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr =(uint8_t*) &g_ShortAddr;

  task_index = MAC_SETUP_TASK_SET_SHORT_ADDR;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Short Addr Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_set_asso_permit(void){
  MAC_setReq_t      SetReq;
  uint8_t PIB_Value = g_TRUE;

  /* Set Association Permit */
  APP_DBG_MSG("FFD MAC APP - Set Association Permit");

  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_ASSOCIATION_PERMIT_c;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;

  task_index = MAC_SETUP_TASK_SET_ASSO_PERMIT;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Association Permit Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_set_tx_power(void){
  MAC_setReq_t      SetReq;
  int8_t tx_power_pib_value = 2;  /* int8 dBm value in the range [-21;6] */

  /* Set Tx Power */
  APP_DBG_MSG("FFD MAC APP - Set TX Power");
  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t *)&tx_power_pib_value;

  task_index = MAC_SETUP_TASK_SET_TX_POWER;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Tx Power Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_start_device(void){
  MAC_startReq_t    StartReq;

  /* Start Device */
  APP_DBG_MSG("FFD MAC APP - Start FFD Device");
  memset(&StartReq,0x00,sizeof(MAC_startReq_t));
  memcpy(StartReq.a_PAN_id,(uint8_t*)&g_panId,0x02);
  StartReq.channel_number   = g_channel;
  StartReq.beacon_order     = 0x0F;
  StartReq.superframe_order = 0x0F;
  StartReq.PAN_coordinator  = g_TRUE;
  StartReq.battery_life_extension = g_FALSE;

  task_index = MAC_SETUP_TASK_START_DEVICE;

  if ( MAC_SUCCESS != MAC_MLMEStartReq( &StartReq) ) {
	APP_DBG_MSG("FFD MAC - Set Association Permit Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_set_rx(void){
  MAC_setReq_t      SetReq;
  uint8_t PIB_Value = g_TRUE;

  /* Set RxOnWhenIdle */
  APP_DBG_MSG("FFD MAC APP - Set RX On When Idle");
  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_RX_ON_WHEN_IDLE_c;
  SetReq.PIB_attribute_valuePtr = &PIB_Value;

  task_index = MAC_SETUP_TASK_SET_RX_ON_WHEN_IDEL;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Rx On When Idle Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_ffd_mac_complete(void){
	connect_state = APP_MAC_CONNECTED;

	return MAC_SUCCESS;
}

/*
 * RFD functions
 */
MAC_Status_t app_rfd_mac_set_ext_addr(void){
  /* Set Device Extended Address */
  MAC_setReq_t      SetReq;
  long long  extAddr = g_extAddr + 1;

  APP_DBG_MSG("FFD MAC APP - Set Extended Address");
  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
  SetReq.PIB_attribute_valuePtr = (uint8_t*) &extAddr;

  task_index = MAC_SETUP_TASK_SET_EXT_ADDR;

  if ( MAC_SUCCESS != MAC_MLMESetReq( &SetReq ) ) {
	APP_DBG_MSG("FFD MAC - Set Extended Addr Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_rfd_mac_set_asso_request(void){
  MAC_associateReq_t AssociateReq;

  /* Set Association Request */
  APP_DBG_MSG("RFD MAC APP - Association REQ\0");

  AssociateReq.channel_number   = g_channel;
  AssociateReq.channel_page     = g_channel_page;
  AssociateReq.coord_addr_mode  = g_SHORT_ADDR_MODE_c;
  memcpy(AssociateReq.coord_address.a_short_addr,&g_coordShortAddr,0x02);
  AssociateReq.capability_information = 0x80;
  memcpy(AssociateReq.a_coord_PAN_id,&g_panId,0x02);
  AssociateReq.security_level = 0x00;

  task_index = MAC_SETUP_TASK_SET_ASSO_REQUEST;

  if(MAC_MLMEAssociateReq( &AssociateReq) != MAC_SUCCESS){
	APP_DBG_MSG("RFD MAC - Set Association request Fails");
	return MAC_ERROR;
  }

  return MAC_SUCCESS;
}

MAC_Status_t app_rfd_mac_set_allocate_short_addr(void){
	MAC_setReq_t      SetReq;

	/* Set allocated Short Address : */
	/* Set Device Short Address */
	APP_DBG_MSG("RFD MAC APP - Set Short Address\0");
	memset(&SetReq,0x00,sizeof(MAC_setReq_t));
	SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
	SetReq.PIB_attribute_valuePtr = (uint8_t*)&(g_MAC_associateCnf.a_assoc_short_address);

	task_index = MAC_SETUP_TASK_SET_ALLOCATE_SHORT_ADDR;

	if ( MAC_MLMESetReq( &SetReq ) != MAC_SUCCESS ) {
	  APP_DBG_MSG("RFD MAC - Set Short Addr Fails\0");
	  return MAC_ERROR;
	}

	return MAC_SUCCESS;
}

MAC_Status_t app_rfd_mac_complete(void){
	connect_state = APP_MAC_CONNECTED;

	return MAC_SUCCESS;
}

void APP_FFD_MAC_802_15_4_SetupTask(void)
{
//
//  MAC_Status_t MacStatus = MAC_ERROR;
//
//  MAC_resetReq_t    ResetReq;
//  MAC_setReq_t      SetReq;
//  MAC_startReq_t    StartReq;
//
//  uint8_t PIB_Value = 0x00;
//
//  int8_t tx_power_pib_value = 0;

  APP_DBG_MSG("Run FFD MAC 802.15.4 - 2 - FFD Startup");

  /* Reset FFD Device */
  /* Reset MAC */
  if(app_ffd_mac_reset()){
	  return;
  }
//  memset(&ResetReq,0x00,sizeof(MAC_resetReq_t));
//  ResetReq.set_default_PIB = TRUE;
//
//  MacStatus = MAC_MLMEResetReq( &ResetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Reset Fails");
//    return;
//  }
//  /* Wait for Reset Confirmation */
//  UTIL_SEQ_WaitEvt( 1U<< CFG_EVT_DEVICE_RESET_CNF );
//  APP_DBG_MSG("FFD MAC APP - Reset CNF Received");

  /* Set Device Extended Address */
//  APP_DBG_MSG("FFD MAC APP - Set Extended Address");
//  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
//  SetReq.PIB_attribute = g_MAC_EXTENDED_ADDRESS_c;
//  SetReq.PIB_attribute_valuePtr = (uint8_t*) &extAddr;
//  MacStatus = MAC_MLMESetReq( &SetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Extended Addr Fails");
//    return;
//  }
//  UTIL_SEQ_WaitEvt( 1U<< CFG_EVT_SET_CNF );
//  APP_DBG_MSG("FFD MAC APP - Set Extended Address CNF Received");


  /* Set Device Short Address */
//  APP_DBG_MSG("FFD MAC APP - Set Short Address");
//  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
//  SetReq.PIB_attribute = g_MAC_SHORT_ADDRESS_c;
//  SetReq.PIB_attribute_valuePtr =(uint8_t*) &shortAddr;
//  MacStatus = MAC_MLMESetReq( &SetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Short Addr Fails");
//    return;
//  }
//
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_SET_CNF );
//  APP_DBG_MSG("FFD MAC APP - Set Short Address CNF Received");

  /* Set Association Permit */
//  APP_DBG_MSG("FFD MAC APP - Set Association Permit");
//
//  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
//  SetReq.PIB_attribute = g_MAC_ASSOCIATION_PERMIT_c;
//  PIB_Value = g_TRUE;
//  SetReq.PIB_attribute_valuePtr = &PIB_Value;
//
//  MacStatus = MAC_MLMESetReq( &SetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Association Permit Fails");
//    return;
//  }
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_SET_CNF );
//  APP_DBG_MSG("FFD MAC APP - Set Association Permit CNF Received");



//  /* Set Tx Power */
//  APP_DBG_MSG("FFD MAC APP - Set TX Power");
//  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
//  SetReq.PIB_attribute = g_PHY_TRANSMIT_POWER_c;
//  tx_power_pib_value = 2;                       /* int8 dBm value in the range [-21;6] */
//  SetReq.PIB_attribute_valuePtr = (uint8_t *)&tx_power_pib_value;
//
//  MacStatus = MAC_MLMESetReq( &SetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Tx Power Fails");
//    return;
//  }
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_SET_CNF );
  
  
//  /* Start Device */
//  APP_DBG_MSG("FFD MAC APP - Start FFD Device");
//  memset(&StartReq,0x00,sizeof(MAC_startReq_t));
//  memcpy(StartReq.a_PAN_id,(uint8_t*)&panId,0x02);
//  StartReq.channel_number   = channel;
//  StartReq.beacon_order     = 0x0F;
//  StartReq.superframe_order = 0x0F;
//  StartReq.PAN_coordinator  = g_TRUE;
//  StartReq.battery_life_extension = g_FALSE;
//
//  MacStatus = MAC_MLMEStartReq( &StartReq);
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Association Permit Fails");
//    return;
//  }
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_DEVICE_STARTED_CNF );
//  APP_DBG_MSG("FFD MAC APP - Start FFD Device CNF Received");


//  /* Set RxOnWhenIdle */
//  APP_DBG_MSG("FFD MAC APP - Set RX On When Idle");
//  memset(&SetReq,0x00,sizeof(MAC_setReq_t));
//  SetReq.PIB_attribute = g_MAC_RX_ON_WHEN_IDLE_c;
//  PIB_Value = g_TRUE;
//  SetReq.PIB_attribute_valuePtr = &PIB_Value;
//
//  MacStatus = MAC_MLMESetReq( &SetReq );
//  if ( MAC_SUCCESS != MacStatus ) {
//    APP_DBG_MSG("FFD MAC - Set Rx On When Idle Fails");
//    return;
//  }
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_SET_CNF );
//  APP_DBG_MSG("FFD MAC APP - Set RX On When Idle CNF Received");
  /* Go in Echo loop */
  APP_DBG_MSG("FFD MAC APP - Ready to Handle Association Req and Receive Data");

  //BSP_LED_On(LED_BLUE);
}

void APP_RFD_MAC_802_15_4_SendData(uint8_t *report, size_t sz)
{
  MAC_Status_t MacStatus = MAC_ERROR;
  uint8_t index=0;

  //BSP_LED_On(LED3);
  MAC_dataReq_t DataReq;
  APP_DBG_MSG("RFD MAC APP - Send Data to Coordinator\0");
  DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c;
  DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c;

  memcpy(DataReq.a_dst_PAN_id,&g_panId,0x02);
  memcpy(DataReq.dst_address.a_short_addr,&g_coordShortAddr,0x02);

  DataReq.msdu_handle = g_dataHandle++;
  DataReq.ack_Tx =0x00;
  DataReq.GTS_Tx = FALSE;
  rfBuffer[index++] = 0xAB;
  rfBuffer[index++] = 0xCD;
  memcpy(rfBuffer + index, report, sz);
  sz += index;
  rfBuffer[sz] = xorSign(rfBuffer, sz);
  DataReq.msduPtr = (uint8_t*) rfBuffer;
  DataReq.msdu_length = sz + 1;
  DataReq.security_level = 0x00;
  MacStatus = MAC_MCPSDataReq( &DataReq );
  if ( MAC_SUCCESS != MacStatus ) {
    APP_DBG_MSG("RFD MAC - Data Req Fails\0");
    return;
  }
//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_DATA_DATA_CNF );
  //BSP_LED_Off(LED3);
  APP_DBG_MSG("RFD MAC APP - DATA CNF Received\0");
}

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
static void APP_FFD_MAC_802_15_4_Config()
{
  APP_DBG_MSG("configure FFD MAC 802.15.4 - 2");
  /* Register MAC 802.15.4 callback functions */

  memset(&macCbConfig,0x00,sizeof(MAC_callbacks_t));

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
}


MAC_SetupTask_t app_ffd_task_index(void){
	return task_index;
}

/**
  * @brief  compute  simple xor signature of the data to transmit
  *
  * @param  pmessage   : Message to sign.
  * @param  message_len: Message Len
  * @retval Message Signature
  */

uint8_t xorSign( const uint8_t * pmessage, uint8_t message_len)
{
  uint8_t seed = 0x00;
  for (uint8_t i=0x00;i<message_len;i++)
    seed = pmessage[i]^seed;
  return seed;
}

APP_MAC_ConnectState_t APP_MAC_ConnectState(void){
	return connect_state;
}

/************** APP FFD ********************/
#define MAC_MSG_FRAME_SYNC_WORD     0x77
#define MAC_MSG_FRAME_TYPE_SYSTEM   0x01
#define MAC_MSG_FRAME_TYPE_HID      0x02

#define MAC_MSG_INDEX_MSG_TYPE      0x01
#define MAC_MSG_INDEX_DATA_SIZE     0x02
#define MAC_MSG_INDEX_DATA          0x03

#define MAC_XOR_LENGTH              (1 + 1 + p[MAC_MSG_INDEX_DATA_SIZE])
#define MAC_MSG_LENGTH              (1 + 1 + 1 + p[MAC_MSG_INDEX_DATA_SIZE] + 1)

/*
 * Frames via MAC
 * ======================================================
 * | uint8_t   | Sync Word         | 0x77               |
 * | uint8_t   | Message Type      | 01 -> System       |
 * |           |                   | 02 -> HID          |
 * | uint8_t   | Data size         | <= 251(255-1-1-1-1)|
 * | array     | Data              |                    |
 * | uint8_t   | End Code          | XORSign            |
 * ======================================================
 */
static void MACAPP_Report(uint8_t *data, uint8_t sz){
	MAC_Status_t MacStatus = MAC_ERROR;
	MAC_dataReq_t DataReq;

	memcpy(rfBuffer, data, sz);

	APP_DBG_MSG("RFD MAC APP - Send Data to Coordinator\0");
	DataReq.src_addr_mode = g_SHORT_ADDR_MODE_c;
	DataReq.dst_addr_mode = g_SHORT_ADDR_MODE_c;

	memcpy(DataReq.a_dst_PAN_id,&g_panId,0x02);
	memcpy(DataReq.dst_address.a_short_addr,&g_coordShortAddr,0x02);

	DataReq.msdu_handle = g_dataHandle++;
	DataReq.ack_Tx =0x00;
	DataReq.GTS_Tx = FALSE;
	DataReq.msduPtr = rfBuffer;
	DataReq.msdu_length = sz;
	DataReq.security_level = 0x00;
	MacStatus = MAC_MCPSDataReq( &DataReq );
	if ( MAC_SUCCESS != MacStatus ) {
		APP_DBG_MSG("RFD MAC - Data Req Fails\0");
		return;
	}
	//  UTIL_SEQ_WaitEvt( 1U << CFG_EVT_DATA_DATA_CNF );
	//BSP_LED_Off(LED3);
	APP_DBG_MSG("RFD MAC APP - DATA CNF Received\0");
}

void MACAPP_Report_Keyboard(report_keyboard_t *report){
	uint8_t index=0;
	uint8_t *msg = mac_send_data[idx_mac_send_data];
	uint8_t *p = msg + 1;
	msg_t msg_result;

	p[index++] = MAC_MSG_FRAME_SYNC_WORD;
	p[index++] = MAC_MSG_FRAME_TYPE_HID;

	if(keymap_config.nkro){
		if(report->nkro.report_id != REPORT_ID_NKRO){
			return; /* invalid report */
		}

		p[index++] = 16; // data size
		memcpy(p + index, report->raw, 16);

		index += 16;
	} else {
		p[index++] = 9; // data size: report ID + report
		p[index++] = REPORT_ID_KEYBOARD;
		memcpy(p+index, report->raw, 8);

		index += 8;
	}

	p[index++] = xorSign(p + MAC_MSG_INDEX_MSG_TYPE, MAC_XOR_LENGTH);

	if(index != MAC_MSG_LENGTH){
		return;
	}

	msg[0] = index;

	chSysLock();
	msg_result = chMBPostI(&mb_mac_send, (msg_t)msg);
	chSysUnlock();

	if(msg_result != MSG_OK) return;

	if(idx_mac_send_data >= (LIST_SEND_DATA_SIZE - 1)){
		idx_mac_send_data = 0;
	} else {
		idx_mac_send_data++;
	}
}


void MACAPP_Report_NKRO(report_nkro_t *report){
	uint8_t index=0;
	uint8_t *msg = mac_send_data[idx_mac_send_data];
	uint8_t *p = msg + 1;
	msg_t msg_result;

	p[index++] = MAC_MSG_FRAME_SYNC_WORD;
	p[index++] = MAC_MSG_FRAME_TYPE_HID;

	if(keymap_config.nkro){
		if(report->nkro.report_id != REPORT_ID_NKRO){
			return; /* invalid report */
		}

		p[index++] = 16; // data size
		memcpy(p + index, report->raw, 16);

		index += 16;
	} else {
		p[index++] = 9; // data size: report ID + report
		p[index++] = REPORT_ID_KEYBOARD;
		memcpy(p+index, report->raw, 8);

		index += 8;
	}

	p[index++] = xorSign(p + MAC_MSG_INDEX_MSG_TYPE, MAC_XOR_LENGTH);

	if(index != MAC_MSG_LENGTH){
		return;
	}

	msg[0] = index;

	chSysLock();
	msg_result = chMBPostI(&mb_mac_send, (msg_t)msg);
	chSysUnlock();

	if(msg_result != MSG_OK) return;

	if(idx_mac_send_data >= (LIST_SEND_DATA_SIZE - 1)){
		idx_mac_send_data = 0;
	} else {
		idx_mac_send_data++;
	}
}

uint8_t MACAPP_Get_LED_Report(void){
	return led_state_from_dongle;
}

void MACAPP_Report_Extra(report_extra_t *report){
	uint8_t index=0;
	uint8_t *msg = mac_send_data[idx_mac_send_data];
	uint8_t *p = msg + 1;
	msg_t msg_result;

	p[index++] = MAC_MSG_FRAME_SYNC_WORD;
	p[index++] = MAC_MSG_FRAME_TYPE_HID;

	p[index++] = 3; // data size

	p[index++] = report->report_id;
	p[index++] = (uint8_t)(report->usage & 0xFF);
	p[index++] = (uint8_t)((report->usage >> 8) & 0xFF);

	p[index++] = xorSign(p + MAC_MSG_INDEX_MSG_TYPE, MAC_XOR_LENGTH);

	if(index != MAC_MSG_LENGTH){
		return;
	}

	msg[0] = index;

	chSysLock();
	msg_result = chMBPostI(&mb_mac_send, (msg_t)msg);
	chSysUnlock();

	if(msg_result != MSG_OK) return;

	idx_mac_send_data++;

	if(idx_mac_send_data >= LIST_SEND_DATA_SIZE){
		idx_mac_send_data = 0;
	}
}

void MACAPP_Report_Mouse(report_mouse_t *report){
	uint8_t index=0;
	uint8_t sz_report=sizeof(report_mouse_t);
	uint8_t *msg = mac_send_data[idx_mac_send_data];
	uint8_t *p = msg + 1;
	msg_t msg_result;

	if((report->report_id != REPORT_ID_MOUSE) || (sz_report > SEND_DATA_MAX_SIZE)){
		return; /* invalid report */
	}

	p[index++] = MAC_MSG_FRAME_SYNC_WORD;
	p[index++] = MAC_MSG_FRAME_TYPE_HID;

	p[index++] = sz_report; // data size

	memcpy(p+index, report, sz_report);

	index += sz_report;

	p[index++] = xorSign(p + MAC_MSG_INDEX_MSG_TYPE, MAC_XOR_LENGTH);

	if(index != MAC_MSG_LENGTH){
		return;
	}

	msg[0] = index;

	chSysLock();
	msg_result = chMBPostI(&mb_mac_send, (msg_t)msg);
	chSysUnlock();

	if(msg_result != MSG_OK) return;

	idx_mac_send_data++;

	if(idx_mac_send_data >= LIST_SEND_DATA_SIZE){
		idx_mac_send_data = 0;
	}
}
/**
 * @}
 */


