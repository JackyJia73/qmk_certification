/*
 * st_wireless.h
 *
 */

#ifndef LIB_ST_ST_WIRELESS_H_
#define LIB_ST_ST_WIRELESS_H_

#include "tl.h"
#include "shci.h"

typedef enum{
	ST_WIRELESS_SUCCESS,
	ST_WIRELESS_ERR_SHCI_CONFIG,
	ST_WIRELESS_ERR_UNKNOWN
}ST_WIRELESS_CODE_t;


int is_shci_ready(void);
int st_wireless_init(void);
int st_wireless_start(uint8_t ble_channel);

void APP_ENTRY_ProcessMsgFromRFCoreTask(void);
void APP_ENTRY_TL_MAC_802_15_4_Init(void);
void APP_ENTRY_RegisterCmdBuffer(TL_CmdPacket_t* p_buffer);
SHCI_C2_CONCURRENT_Mode_Param_t APP_Wireless_Concurrent_Mode(void);
void ScheduleProcessSwitchProtocol(void);

#endif /* LIB_ST_ST_WIRELESS_H_ */
