/*
 * st_wireless.c
 *
 */

#include "board.h"
#include "stm32wbxx.h"
#include "quantum.h"
#include "report.h"
#include "host_driver.h"

#include "hids_app.h"
#include "st_wireless.h"
#include "hal_ipcc_lld.h"
#include "st_tasks.h"

#include "ble.h"
#include "tl.h"
#include "shci_tl.h"
#include "shci.h"
#include "ipccv1.h"
#include "hw.h"
#include "app_ble.h"
#include "app_ffd_mac_802_15_4.h"

#include "ch.h"
#include "hal.h"
#include "hal_pal.h"

#include "print.h"


static void APPE_SysStatusNot(SHCI_TL_CmdStatus_t status);
static void APPE_SysUserEvtRx(void * pPayload);
static void APPE_SysEvtReadyProcessing(void * pPayload);
static void APPE_SysEvtError(void * pPayload);

static int shci_config(void);
//static void Process_InitiateSwitchProtocol(void);
//static void Process_ActivateNewProtocol(void);
static void Wait_Getting_Ack_From_RFCore(void);
static void Receive_Ack_From_RFCore(void);
static void Receive_Notification_From_RFCore(void);

/******************************************************************************
 * Transport Layer
 ******************************************************************************/
/**
 * Queue length of BLE Event
 * This parameter defines the number of asynchronous events that can be stored in the HCI layer before
 * being reported to the application. When a command is sent to the BLE core coprocessor, the HCI layer
 * is waiting for the event with the Num_HCI_Command_Packets set to 1. The receive queue shall be large
 * enough to store all asynchronous events received in between.
 * When CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE is set to 27, this allow to store three 255 bytes long asynchronous events
 * between the HCI command and its event.
 * This parameter depends on the value given to CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE. When the queue size is to small,
 * the system may hang if the queue is full with asynchronous events and the HCI layer is still waiting
 * for a CC/CS event, In that case, the notification TL_BLE_HCI_ToNot() is called to indicate
 * to the application a HCI command did not receive its command event within 30s (Default HCI Timeout).
 */
#define CFG_TLBLE_EVT_QUEUE_LENGTH 5
/**
 * This parameter should be set to fit most events received by the HCI layer. It defines the buffer size of each element
 * allocated in the queue of received events and can be used to optimize the amount of RAM allocated by the Memory Manager.
 * It should not exceed 255 which is the maximum HCI packet payload size (a greater value is a lost of memory as it will
 * never be used)
 * It shall be at least 4 to receive the command status event in one frame.
 * The default value is set to 27 to allow receiving an event of MTU size in a single buffer. This value maybe reduced
 * further depending on the application.
 */
#define CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE 255   /**< Set to 255 with the memory manager and the mailbox */

#define TL_BLE_EVENT_FRAME_SIZE ( TL_EVT_HDR_SIZE + CFG_TLBLE_MOST_EVENT_PAYLOAD_SIZE )


/* Private defines -----------------------------------------------------------*/
#define POOL_SIZE (CFG_TLBLE_EVT_QUEUE_LENGTH*4U*DIVC((sizeof(TL_PacketHeader_t) + TL_BLE_EVENT_FRAME_SIZE), 4U))

/* USER CODE BEGIN PD */
/* Section specific to button management using UART */
#define C_SIZE_CMD_STRING       256U
#define RX_BUFFER_SIZE          8U

#define LL_HSEM_COREID                  HSEM_CR_COREID_CURRENT

/**
  * @brief  Get the lock with 1-step lock.
  * @rmtoll RLR          LOCK          LL_HSEM_1StepLock
  * @rmtoll RLR          COREID        LL_HSEM_1StepLock
  * @rmtoll RLR          PROCID        LL_HSEM_1StepLock
  * @param  HSEMx HSEM Instance.
  * @param  Semaphore Semaphore number. Value between Min_Data=0 and Max_Data=31
  * @retval 1 lock fail, 0 lock successful or already locked by same core
  */
__STATIC_INLINE uint32_t LL_HSEM_1StepLock(HSEM_TypeDef *HSEMx, uint32_t Semaphore)
{
  return ((HSEMx->RLR[Semaphore] != (HSEM_R_LOCK | LL_HSEM_COREID)) ? 1UL : 0UL);
}

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/*
 * Stack & BLE
 */
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t EvtPool[POOL_SIZE];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t SystemCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t SystemSpareEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
//PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t BleSpareEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255];

/*
 * MAC 802.15.4
 */
PLACE_IN_SECTION("MB_MEM1") ALIGN(4) static TL_MAC_802_15_4_Config_t Mac_802_15_4_ConfigBuffer;

PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t Mac_802_15_4_CmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t Mac_802_15_4_NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) char mac_802_15_4_CnfIndNot[C_SIZE_CMD_STRING];

/* USER CODE BEGIN PV */

/* USER CODE END PV */
static TL_CmdPacket_t *p_mac_802_15_4_cmdbuffer;
static TL_EvtPacket_t *p_mac_802_15_4_notif_RFCore_to_M4;

static volatile uint8_t pendingMsgFromRFCore = 0U; /* Pending Tx From Core Protocol status */

//static int FlagSwitchingOnGoing = 0;

static ST_WIRELESS_CODE_t flag_shci_ready=ST_WIRELESS_ERR_UNKNOWN;
static SHCI_C2_CONCURRENT_Mode_Param_t ConcurrentMode = BLE_ENABLE;

static binary_semaphore_t bsemCmd;
static binary_semaphore_t bsemRf;

/*
 * SHCI Event
 */
static binary_semaphore_t bsemShciEvt;
static thread_t *thSchiEvt;
static THD_WORKING_AREA(waThreadSchiEvt, 128 /*512*/);
static THD_FUNCTION(ThreadShciEvt, arg) {
	while(1){
		chBSemWait(arg);
		shci_user_evt_proc();
	}
}
static thread_descriptor_t td_evt = {
	"shci_user_evt_proc",                   /* thread name */
	THD_WORKING_AREA_BASE(waThreadSchiEvt), /* Pointer to the working area base. */
	THD_WORKING_AREA_END(waThreadSchiEvt),  /* Pointer to the working area end. */
	HIGHPRIO,                               /* Thread priority, will be defined later */
	ThreadShciEvt,                          /* Thread function pointer */
	NULL                                    /* Thread argument, will be defined later */
};

/*
 * Switch Protocols
 */
// static binary_semaphore_t bsemSWPro;
// static thread_t *thSWPro;
// static THD_WORKING_AREA(waThreadSWPro, 128 /*512*/);
// static THD_FUNCTION(ThreadSWPro, arg) {
// 	while(1){
// 		chBSemWait(arg);
// 		Process_InitiateSwitchProtocol();
// 	}
// }
// static thread_descriptor_t td_SWPro = {
// 	"switch_protocol",                    /* thread name */
// 	THD_WORKING_AREA_BASE(waThreadSWPro), /* Pointer to the working area base. */
// 	THD_WORKING_AREA_END(waThreadSWPro),  /* Pointer to the working area end. */
// 	NORMALPRIO,                             /* Thread priority, will be defined later */
// 	ThreadSWPro,                          /* Thread function pointer */
// 	NULL                                  /* Thread argument, will be defined later */
// };

/*
 * Notification from RFCore
 */
static binary_semaphore_t bsemNotiRf;
static thread_t *thNotiRf;
static THD_WORKING_AREA(waThreadNotiRf, 128 /*512*/);
static THD_FUNCTION(ThreadNotiRf, arg) {
	while(1){
		chBSemWait(arg);
		APP_ENTRY_ProcessMsgFromRFCoreTask();
	}
}
static thread_descriptor_t td_NotiRf = {
	"notif_rf",                            /* thread name */
	THD_WORKING_AREA_BASE(waThreadNotiRf), /* Pointer to the working area base. */
	THD_WORKING_AREA_END(waThreadNotiRf),  /* Pointer to the working area end. */
	HIGHPRIO,                              /* Thread priority, will be defined later */
	ThreadNotiRf,                          /* Thread function pointer */
	NULL                                   /* Thread argument, will be defined later */
};


void appe_Tl_Init(void)
{
  TL_MM_Config_t tl_mm_config;
  SHCI_TL_HciInitConf_t SHci_Tl_Init_Conf;

  flag_shci_ready = 0;

  /**< Reference table initialization */
  TL_Init();

  
  /**< System channel initialization */
//  UTIL_SEQ_RegTask(1<< CFG_TASK_SYSTEM_HCI_ASYNCH_EVT_ID, UTIL_SEQ_RFU, shci_user_evt_proc);
  chBSemObjectInit(&bsemCmd, true);
  chBSemObjectInit(&bsemRf, true);
  chBSemObjectInit(&bsemShciEvt, true);
  
  td_evt.prio = HIGHPRIO;
  td_evt.arg = &bsemShciEvt;
    
  if (0) thSchiEvt = chThdCreate(&td_evt);


  // chBSemObjectInit(&bsemSWPro, true);
  // td_SWPro.prio = HIGHPRIO;
  // td_SWPro.arg = &bsemSWPro;
  // thSWPro = chThdCreate(&td_SWPro);

  chBSemObjectInit(&bsemNotiRf, true);
  td_NotiRf.prio = HIGHPRIO;
  td_NotiRf.arg = &bsemNotiRf;
  if(0) thNotiRf = chThdCreate(&td_NotiRf);

  SHci_Tl_Init_Conf.p_cmdbuffer = (uint8_t*)&SystemCmdBuffer;
  SHci_Tl_Init_Conf.StatusNotCallBack = APPE_SysStatusNot;
  shci_init(APPE_SysUserEvtRx, (void*) &SHci_Tl_Init_Conf);
  

  /**< Memory Manager channel initialization */
  tl_mm_config.p_BleSpareEvtBuffer = 0;
  tl_mm_config.p_SystemSpareEvtBuffer = SystemSpareEvtBuffer;
  tl_mm_config.p_AsynchEvtPool = EvtPool;
  tl_mm_config.AsynchEvtPoolSize = POOL_SIZE;
  TL_MM_Init(&tl_mm_config);

  TL_Enable();

  return;
}

static void APPE_SysStatusNot(SHCI_TL_CmdStatus_t status)
{
  (void)(status);
  return;
}

/**
 * The type of the payload for a system user event is tSHCI_UserEvtRxParam
 * When the system event is both :
 *    - a ready event (subevtcode = SHCI_SUB_EVT_CODE_READY)
 *    - reported by the FUS (sysevt_ready_rsp == FUS_FW_RUNNING)
 * The buffer shall not be released
 * (eg ((tSHCI_UserEvtRxParam*)pPayload)->status shall be set to SHCI_TL_UserEventFlow_Disable)
 * When the status is not filled, the buffer is released by default
 */
static void APPE_SysUserEvtRx(void * pPayload)
{
  TL_AsynchEvt_t *p_sys_event;
  WirelessFwInfo_t WirelessInfo;

  p_sys_event = (TL_AsynchEvt_t*)(((tSHCI_UserEvtRxParam*)pPayload)->pckt->evtserial.evt.payload);

  switch(p_sys_event->subevtcode)
  {
  case SHCI_SUB_EVT_CODE_READY:
    /* Read the firmware version of both the wireless firmware and the FUS */
    SHCI_GetWirelessFwInfo(&WirelessInfo);
    BLE_DBG_APP_MSG("Wireless Firmware version %d.%d.%d\n", WirelessInfo.VersionMajor, WirelessInfo.VersionMinor, WirelessInfo.VersionSub);
    BLE_DBG_APP_MSG("Wireless Firmware build %d\n", WirelessInfo.VersionReleaseType);
    BLE_DBG_APP_MSG("FUS version %d.%d.%d\n", WirelessInfo.FusVersionMajor, WirelessInfo.FusVersionMinor, WirelessInfo.FusVersionSub);

    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_CODE_READY\n\r");

    APPE_SysEvtReadyProcessing(pPayload);
    break;

  case SHCI_SUB_EVT_ERROR_NOTIF:
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_ERROR_NOTIF \n\r");
    APPE_SysEvtError(pPayload);
    break;

  case SHCI_SUB_EVT_BLE_NVM_RAM_UPDATE:
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_BLE_NVM_RAM_UPDATE -- BLE NVM RAM HAS BEEN UPDATED BY CPU2 \n");
    BLE_DBG_APP_MSG("     - StartAddress = %lx , Size = %ld\n",
                ((SHCI_C2_BleNvmRamUpdate_Evt_t*)p_sys_event->payload)->StartAddress,
                ((SHCI_C2_BleNvmRamUpdate_Evt_t*)p_sys_event->payload)->Size);
    break;

  case SHCI_SUB_EVT_NVM_START_WRITE:
    BLE_DBG_APP_MSG("==>> SHCI_SUB_EVT_NVM_START_WRITE : NumberOfWords = %ld\n",
                ((SHCI_C2_NvmStartWrite_Evt_t*)p_sys_event->payload)->NumberOfWords);
    break;

  case SHCI_SUB_EVT_NVM_END_WRITE:
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_NVM_END_WRITE\n\r");
    break;

  case SHCI_SUB_EVT_NVM_START_ERASE:
    BLE_DBG_APP_MSG("==>>SHCI_SUB_EVT_NVM_START_ERASE : NumberOfSectors = %ld\n",
                ((SHCI_C2_NvmStartErase_Evt_t*)p_sys_event->payload)->NumberOfSectors);
    break;

  case SHCI_SUB_EVT_NVM_END_ERASE:
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_NVM_END_ERASE\n\r");
    break;

  default:
    break;
  }

  return;
}

static void APPE_SysEvtReadyProcessing(void * pPayload)
{
  TL_AsynchEvt_t *p_sys_event;
  SHCI_C2_Ready_Evt_t *p_sys_ready_event;

  p_sys_event = (TL_AsynchEvt_t*)(((tSHCI_UserEvtRxParam*)pPayload)->pckt->evtserial.evt.payload);
  p_sys_ready_event = (SHCI_C2_Ready_Evt_t*) p_sys_event->payload;

  if (p_sys_ready_event->sysevt_ready_rsp == WIRELESS_FW_RUNNING)
  {
	flag_shci_ready = 1;
  }
  else if (p_sys_ready_event->sysevt_ready_rsp == FUS_FW_RUNNING)
  {
	flag_shci_ready = 0;
    /**
    * The FUS firmware is running on the CPU2
    * In the scope of this application, there should be no case when we get here
    */
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_CODE_READY - FUS_FW_RUNNING \n\r");

    /* The packet shall not be released as this is not supported by the FUS */
    ((tSHCI_UserEvtRxParam*)pPayload)->status = SHCI_TL_UserEventFlow_Disable;
  }
  else
  {
	flag_shci_ready = 0;

    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_CODE_READY - UNEXPECTED CASE \n\r");
  }

  return;
}

/**
 * @brief Notify a system error coming from the M0 firmware
 * @param  ErrorCode  : errorCode detected by the M0 firmware
 *
 * @retval None
 */
static void APPE_SysEvtError(void * pPayload)
{
  TL_AsynchEvt_t *p_sys_event;
  SCHI_SystemErrCode_t *p_sys_error_code;

  p_sys_event = (TL_AsynchEvt_t*)(((tSHCI_UserEvtRxParam*)pPayload)->pckt->evtserial.evt.payload);
  p_sys_error_code = (SCHI_SystemErrCode_t*) p_sys_event->payload;

  BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_ERROR_NOTIF WITH REASON %x \n\r",(*p_sys_error_code));

  if ((*p_sys_error_code) == ERR_BLE_INIT)
  {
    /* Error during BLE stack initialization */
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_ERROR_NOTIF WITH REASON - ERR_BLE_INIT \n");
  }
  else
  {
    BLE_DBG_APP_MSG(">>== SHCI_SUB_EVT_ERROR_NOTIF WITH REASON - BLE ERROR \n");
  }
  return;
}

void shci_notify_asynch_evt(void* pdata)
{
//  UTIL_SEQ_SetTask(1<<CFG_TASK_SYSTEM_HCI_ASYNCH_EVT_ID, CFG_SCH_PRIO_0);
#if 0
	chThdStartI(thSchiEvt);
#else
	__dbg_check_lock_from_isr();
	chBSemSignalI(&bsemShciEvt);
	__dbg_check_unlock_from_isr();
#endif

  return;
}

#if 1
void shci_cmd_resp_release(uint32_t flag)
{
//  UTIL_SEQ_SetEvt(1<< CFG_IDLEEVT_SYSTEM_HCI_CMD_EVT_RSP_ID);
  __dbg_check_lock_from_isr();
  chBSemSignalI(&bsemCmd);
  __dbg_check_unlock_from_isr();

  return;
}

void shci_cmd_resp_wait(uint32_t timeout)
{
//  UTIL_SEQ_WaitEvt(1<< CFG_IDLEEVT_SYSTEM_HCI_CMD_EVT_RSP_ID);
  chBSemWait(&bsemCmd);

  return;
}
#endif

OSAL_IRQ_HANDLER(STM32_IPCC_C1_RX_HANDLER) {
  OSAL_IRQ_PROLOGUE();

  HW_IPCC_Rx_Handler();

  OSAL_IRQ_EPILOGUE();
}

OSAL_IRQ_HANDLER(STM32_IPCC_C1_TX_HANDLER) {
  OSAL_IRQ_PROLOGUE();

  HW_IPCC_Tx_Handler();

  OSAL_IRQ_EPILOGUE();
}

int is_ble_connected(void){
	return (APP_BLE_Get_Server_Connection_Status() == APP_BLE_CONNECTED_SERVER);
}

static int shci_config(void){
  SHCI_CmdStatus_t result;
  SHCI_C2_CONFIG_Cmd_Param_t config_param = {0};
  uint32_t RevisionID=0;
  uint32_t DeviceID=0;

  /**< Traces channel initialization */
  TL_TRACES_Init( );

  /* Enable all events Notification */
  config_param.PayloadCmdSize = SHCI_C2_CONFIG_PAYLOAD_CMD_SIZE;
  config_param.EvtMask1 = SHCI_C2_CONFIG_EVTMASK1_BIT0_ERROR_NOTIF_ENABLE
    +  SHCI_C2_CONFIG_EVTMASK1_BIT1_BLE_NVM_RAM_UPDATE_ENABLE
      +  SHCI_C2_CONFIG_EVTMASK1_BIT2_THREAD_NVM_RAM_UPDATE_ENABLE
        +  SHCI_C2_CONFIG_EVTMASK1_BIT3_NVM_START_WRITE_ENABLE
          +  SHCI_C2_CONFIG_EVTMASK1_BIT4_NVM_END_WRITE_ENABLE
            +  SHCI_C2_CONFIG_EVTMASK1_BIT5_NVM_START_ERASE_ENABLE
              +  SHCI_C2_CONFIG_EVTMASK1_BIT6_NVM_END_ERASE_ENABLE;

  /* Read revision identifier */
  /**
  * @brief  Return the device revision identifier
  * @note   This field indicates the revision of the device.
  * @rmtoll DBGMCU_IDCODE REV_ID        LL_DBGMCU_GetRevisionID
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
  RevisionID = LL_DBGMCU_GetRevisionID();

  BLE_DBG_APP_MSG(">>== DBGMCU_GetRevisionID= %lx \n\r", RevisionID);

  config_param.RevisionID = (uint16_t)RevisionID;

  DeviceID = LL_DBGMCU_GetDeviceID();
  BLE_DBG_APP_MSG(">>== DBGMCU_GetDeviceID= %lx \n\r", DeviceID);
  config_param.DeviceID = (uint16_t)DeviceID;
  result = SHCI_C2_Config(&config_param);

  //    UTIL_LPM_SetOffMode(1U << CFG_LPM_APP, UTIL_LPM_ENABLE);

  return result;
}

#if 0
/* Callback associated to both the raising edge and falling edge of the button line. */
static void button_cb(void *arg) {
  (void)arg;
  ScheduleProcessSwitchProtocol();
}

static void create_event_switch_protocols(void){
  /* Setting the button line as digital input with pull resistors. */
  palSetLineMode(LINE_BUTTON_1, PAL_MODE_INPUT_PULLUP);
  /* Enabling the event on the Line for Rising edge. */
  palEnableLineEvent(LINE_BUTTON_1, PAL_EVENT_MODE_RISING_EDGE);
  /* Associating a callback to the Line. */
  palSetLineCallback(LINE_BUTTON_1, button_cb, NULL);
}
#endif

#define MAX_SCHI_TRY_TIMERS     50

// int st_wireless_start(void){
// 	int counter = 0;

//     /**
// 	* This prevents the CPU2 to disable the HSI48 oscillator when
// 	* it does not use anymore the RNG IP
// 	*/
// 	LL_HSEM_1StepLock( HSEM, 5 );

// //	create_event_switch_protocols();

// 	ipcc_lld_init();
// 	ipcc_lld_start();

// 	appe_Tl_Init();

// 	while(!is_shci_ready() && (counter < MAX_SCHI_TRY_TIMERS)){
// 		chThdSleepMilliseconds(10);

// 		counter++;
// 	}

// 	if(counter >= MAX_SCHI_TRY_TIMERS){
// 		return ST_WIRELESS_ERR_UNKNOWN;
// 	}

// 	if(shci_config()){
// 		return ST_WIRELESS_ERR_SHCI_CONFIG;
// 	}

// 	if(ConcurrentMode == MAC_ENABLE){
// 		APP_FFD_MAC_802_15_4_Init(APP_MAC_802_15_4_FULL, &Mac_802_15_4_CmdBuffer);
// 	}else{
// 		APP_BLE_Init();
// 	}

// 	return ST_WIRELESS_SUCCESS;
// }
int st_wireless_init(void){
	int counter = 0;

    /**
	* This prevents the CPU2 to disable the HSI48 oscillator when
	* it does not use anymore the RNG IP
	*/
	LL_HSEM_1StepLock( HSEM, 5 );

//	create_event_switch_protocols();

	ipcc_lld_init();
	ipcc_lld_start();

	appe_Tl_Init();
	return ST_WIRELESS_SUCCESS;
	while(!is_shci_ready() && (counter < MAX_SCHI_TRY_TIMERS)){
		chThdSleepMilliseconds(10);

		counter++;
	}

	if(counter >= MAX_SCHI_TRY_TIMERS){
		return ST_WIRELESS_ERR_UNKNOWN;
	}

	if(shci_config()){
		return ST_WIRELESS_ERR_SHCI_CONFIG;
	}

	//	APP_BLE_Init();

	return ST_WIRELESS_SUCCESS;
}
static host_driver_t HIDS_BLE_Drivers = {
	HIDSAPP_Get_LED_Report, HIDSAPP_Report_Keyboard, HIDSAPP_Report_NKRO, HIDSAPP_Report_Mouse, HIDSAPP_Report_Extra
};

static host_driver_t HIDS_MAC_Drivers = {
	MACAPP_Get_LED_Report, MACAPP_Report_Keyboard, MACAPP_Report_NKRO, MACAPP_Report_Mouse, MACAPP_Report_Extra
};

static host_driver_t *last_host_driver = NULL;

int st_wireless_start(uint8_t ble_channel)
{
  clear_keyboard();
	APP_BLE_Init(ble_channel);
  last_host_driver = host_get_driver();
	if (1)
    host_set_driver(&HIDS_BLE_Drivers);
  else
    host_set_driver(&HIDS_MAC_Drivers);
	return ST_WIRELESS_SUCCESS;
}

int st_wireless_stop(void)
{
  clear_keyboard();
  APP_BLE_Stop();
  host_set_driver(last_host_driver);
	return ST_WIRELESS_SUCCESS;
}

int st_wireless_delete(uint8_t ble_channel)
{
  clear_keyboard();
  APP_BLE_Delete(ble_channel);
	return ST_WIRELESS_SUCCESS;
}


#if 0

/**
 * @brief  Process used to initiate the protocol switch
 * @param  None
 * @retval None
 */

static void Process_InitiateSwitchProtocol(void)
{
   BLE_DBG_APP_MSG("Process_InitiateSwitchProtocol (nb = %d)",NbTotalSwitch ++);

  /* Send Switch event to M0 */
  /* SWITCH BLE <-> MAC */
  if (FlagSwitchingOnGoing == 1)
      return;

  FlagSwitchingOnGoing = 1;

  if(ConcurrentMode == MAC_ENABLE)
  {
	BLE_DBG_APP_MSG("STOP MAC");
    APP_FFD_MAC_802_15_4_Stop();
    APP_BLE_Stop();
    chThdSleepMilliseconds(100);

    /* start a timer */
//    HW_TS_Create(CFG_TIM_WAIT_BEFORE_SWITCH, &TS_ID1, hw_ts_SingleShot, ScheduleProcessSwitchProtocol);
//
//    HW_TS_Start(TS_ID1, SWITCH_TMO);
    BLE_DBG_APP_MSG("SWITCH PROTOCOL TO BLE");
  }
  else
  {
	  BLE_DBG_APP_MSG("STOP BLE");
    APP_BLE_Stop();
    BLE_DBG_APP_MSG("SWITCH PROTOCOL TO MAC");
//    ScheduleProcessSwitchProtocol();
  }

  Process_ActivateNewProtocol();
}
#endif
/**
 * @brief  Schedule the process used to switch protocol
 * @param  None
 * @retval None
 */
//void ScheduleProcessSwitchProtocol(void)
//{
//	chBSemSignal(&bsemSWPro);
//}

/**
 * @brief  Process the activation of the new protocol
 * @param  None
 * @retval None
 */

#if 0
static void Process_ActivateNewProtocol(void)
{
  BLE_DBG_APP_MSG("Process_ActivateNewProtocol");

  /* Toggle Mode flag */
  if(ConcurrentMode == MAC_ENABLE){
    ConcurrentMode = BLE_ENABLE;
  }else{
    ConcurrentMode = MAC_ENABLE;
  }

  /* Once Switch has been acknowledged from M0, starts appropriate Protocol Init */
  if(ConcurrentMode == MAC_ENABLE)
  {
    BLE_DBG_APP_MSG("INIT MAC");

    APP_FFD_MAC_802_15_4_Init(APP_MAC_802_15_4_FULL, &Mac_802_15_4_CmdBuffer);

//    palSetLine(LINE_LED_BLUE);
  }
  else
  {
    BLE_DBG_APP_MSG("INIT BLE");
    APP_BLE_Init();

//    palSetLine(LINE_LED_RED);
  }

  FlagSwitchingOnGoing = 0;
}
#endif
/**
 * @brief Process the messages coming from the RF Core.
 * @param  None
 * @retval None
 */
void APP_ENTRY_ProcessMsgFromRFCoreTask(void)
{
  if (pendingMsgFromRFCore != 0U)
  {
      pendingMsgFromRFCore = 0U;
      MAC_802_15_4_CallBack_Processing();
  }
}

/**
 * @brief  This function is used to transfer the MAC 802.15.4 commands from the
 *         M4 to the RFCore.
 *
 * @param   pCmdBuffer : pointer to the buffer to send
 * @return  None
 */
void Mac_802_15_4_CmdTransfer(void)
{
  TL_MAC_802_15_4_SendCmd();

  /* Wait completion of cmd */
  Wait_Getting_Ack_From_RFCore();
}

void APP_ENTRY_TL_MAC_802_15_4_Init(void)
{
  Mac_802_15_4_ConfigBuffer.p_Mac_802_15_4_CmdRspBuffer = (uint8_t*)&Mac_802_15_4_CmdBuffer;
  Mac_802_15_4_ConfigBuffer.p_Mac_802_15_4_NotAckBuffer = (uint8_t*)Mac_802_15_4_NotifRspEvtBuffer;
  TL_MAC_802_15_4_Init( &Mac_802_15_4_ConfigBuffer );
}

void APP_ENTRY_RegisterCmdBuffer(TL_CmdPacket_t* p_buffer)
{
  p_mac_802_15_4_cmdbuffer = p_buffer;
}

TL_CmdPacket_t* MAC_802_15_4_GetCmdBuffer(void)
{
  return (TL_CmdPacket_t*)p_mac_802_15_4_cmdbuffer;
}

TL_Evt_t* MAC_802_15_4_GetRspPayEvt(void)
{
  return &((TL_EvtPacket_t *)p_mac_802_15_4_cmdbuffer)->evtserial.evt;
}

TL_Evt_t* MAC_802_15_4_GetNotificationBuffer(void)
{
  return &(p_mac_802_15_4_notif_RFCore_to_M4->evtserial.evt);
}

MAC_802_15_4_Notification_t* MAC_802_15_4_GetNotificationPayloadBuffer(void)
{
  return (MAC_802_15_4_Notification_t*)(p_mac_802_15_4_notif_RFCore_to_M4)->evtserial.evt.payload;
}


/**
  * @brief  This function waits for getting an acknowledgment from the RFCore.
  *
  * @param  None
  * @retval None
  */
static void Wait_Getting_Ack_From_RFCore(void)
{
//  UTIL_SEQ_WaitEvt(EVENT_ACK_FROM_RFCore_EVT);
  chBSemWait(&bsemRf);
}

/**
  * @brief  Receive an acknowledgment from the RFCore core.
  *         Each command send by the M4 to the RFCore are acknowledged.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Ack_From_RFCore(void)
{
//  UTIL_SEQ_SetEvt(EVENT_ACK_FROM_RFCore_EVT);
	chBSemSignal(&bsemRf);
}

/**
  * @brief  Receive a notification from the RFCore+ through the IPCC.
  *         This function is called under interrupt.
  * @param  None
  * @retval None
  */
static void Receive_Notification_From_RFCore(void)
{
	pendingMsgFromRFCore++;
//  UTIL_SEQ_SetTask(TASK_MSG_FROM_RF_CORE,CFG_SCH_PRIO_0);
  chBSemSignal(&bsemNotiRf);
}

/* For reception of MAC 802.15.4 Cmd return */
void TL_MAC_802_15_4_CmdEvtReceived( TL_EvtPacket_t * Otbuffer )
{
  Receive_Ack_From_RFCore();
}

/* For reception of MAC 802.15.4 Notification from RFCore */
void TL_MAC_802_15_4_NotReceived( TL_EvtPacket_t * Notbuffer )
{
  p_mac_802_15_4_notif_RFCore_to_M4 = Notbuffer;

  Receive_Notification_From_RFCore();
}

/**
  * @brief  This function is called before sending any ot command to the RFCore
  *         core. The purpose of this function is to be able to check if
  *         there are no notifications coming from the RFCore core which are
  *         pending before sending a new ot command.
  * @param  None
  * @retval None
  */
void Mac_802_15_4_PreCmdProcessing(void)
{
//  UTIL_SEQ_WaitEvt( EVENT_SYNCHRO_BYPASS_IDLE );
	// nothing
}

SHCI_C2_CONCURRENT_Mode_Param_t APP_Wireless_Concurrent_Mode(void){
	return ConcurrentMode;
}

int is_shci_ready(void){
	return flag_shci_ready;
}

