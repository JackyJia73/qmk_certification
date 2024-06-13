
/**
  ******************************************************************************
  * @file    app_ffd_mac_802_15_4.h
  * @author  MCD Application Team
  * @brief   Header for FFD MAC 802.15.4 Core Mac Device implementation
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_FFD_MAC_802_15_4_H
#define APP_FFD_MAC_802_15_4_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/

#include "tl.h"
#include "tl_mac_802_15_4.h"

#include "report.h"


/* ---------------------------------------------------------------------------*/
/* Exported types and defines ------------------------------------------------*/
/* ---------------------------------------------------------------------------*/


/* FFD MAC_802_15_4 application generic defines */
/*------------------------------------*/
typedef enum
{
  APP_MAC_802_15_4_LIMITED,
  APP_MAC_802_15_4_FULL,
} APP_MAC_802_15_4_InitMode_t;


typedef enum
{
  CFG_ASSO_PENDING,
  CFG_SRV_SER_REQ_NBR,
} CFG_COORD_Srv_Task_Req_t;

typedef enum
{
  CFG_DATA_PENDING,
  CFG_SRV_DATA_REQ_NBR,
} CFG_COORD_Data_Task_Req_t;

/* Application errors                 */
/*------------------------------------*/

/*
 *  List of all errors tracked by the Thread application
 *  running on M4. Some of these errors may be fatal
 *  or just warnings
 */
typedef enum
{
  ERR_APPLI_REC_MULTI_MSG_FROM_RFCore,
  ERR_INTERFACE_IPCC_SEND_ACK_TO_RFCore,
  ERR_WRONG_BINARY,
  ERR_WRONG_FIRMWARE_CHECK,
} ErrAppliIdEnum_t;

typedef enum{
	MAC_SETUP_TASK_NONE,
	MAC_SETUP_TASK_RESET,
	MAC_SETUP_TASK_SET_EXT_ADDR,
	MAC_SETUP_TASK_SET_SHORT_ADDR,
	MAC_SETUP_TASK_SET_ASSO_PERMIT,
	MAC_SETUP_TASK_SET_ASSO_REQUEST,
	MAC_SETUP_TASK_SET_ALLOCATE_SHORT_ADDR,
	MAC_SETUP_TASK_SET_TX_POWER,
	MAC_SETUP_TASK_START_DEVICE,
	MAC_SETUP_TASK_SET_RX_ON_WHEN_IDEL,
	MAC_SETUP_TASK_STOP
} MAC_SetupTask_t;

typedef enum{
	APP_MAC_UNKNOWN,
	APP_MAC_CONNECTED,
	APP_MAC_DISCONNECTED
} APP_MAC_ConnectState_t;
/* ---------------------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* ---------------------------------------------------------------------------*/
void APP_FFD_MAC_802_15_4_Init( APP_MAC_802_15_4_InitMode_t InitMode, TL_CmdPacket_t* pCmdBuffer);
void APP_FFD_MAC_802_15_4_Stop(void);
void APP_FFD_MAC_802_15_4_Error(uint32_t ErrId);
void APP_FFD_MAC_802_15_4_SetupTask(void);
void APP_FFD_MAC_802_15_4_CoordSrvTask(void);
void APP_FFD_MAC_802_15_4_CoordDataTask(void);
void APP_FFD_Init( void );
void APP_FFD_postMacInfCnfNot(const char * buf);
void APP_FFD_parseClientReqResCmd ( const char * CommandString  );

void APP_RFD_MAC_802_15_4_SendData(uint8_t *report, size_t sz);

void MACAPP_Report_Keyboard(report_keyboard_t *report);
void MACAPP_Report_NKRO(report_nkro_t *report);
uint8_t MACAPP_Get_LED_Report(void);
void MACAPP_Report_Extra(report_extra_t *report);
void MACAPP_Report_Mouse(report_mouse_t *report);

APP_MAC_ConnectState_t APP_MAC_ConnectState(void);

MAC_SetupTask_t app_ffd_task_index(void);

MAC_Status_t app_ffd_mac_reset(void);
MAC_Status_t app_ffd_mac_set_ext_addr(void);
MAC_Status_t app_ffd_mac_set_short_addr(void);
MAC_Status_t app_ffd_mac_set_asso_permit(void);
MAC_Status_t app_ffd_mac_set_tx_power(void);
MAC_Status_t app_ffd_mac_start_device(void);
MAC_Status_t app_ffd_mac_set_rx(void);
MAC_Status_t app_ffd_mac_complete(void);

MAC_Status_t app_rfd_mac_set_ext_addr(void);
MAC_Status_t app_rfd_mac_set_asso_request(void);
MAC_Status_t app_rfd_mac_set_allocate_short_addr(void);
MAC_Status_t app_rfd_mac_complete(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __APP_FFD_MAC_802_15_4_H */
