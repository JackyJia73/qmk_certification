#include "st_tasks.h"

#include "ch.h"
#include "hal.h"
#include "timer.h"
#include "quantum.h"
#include "report.h"
#include "host_driver.h"

#include "app_ble.h"
#include "bas_app.h"
#include "hids_app.h"
#include "st_wireless.h"
#include "app_ffd_mac_802_15_4.h"


//static uint16_t last_ms = 0;
//static QMK_TRAN_MODE_t trans_mode = QMK_TRANSMIT_MODE_USB;



void st_tasks(void){
	//uint16_t now_ms;

	/*if(!is_shci_ready()){
		return;
	}*/

	//now_ms = timer_read();

	/* detect current mode from the triple modes -- USB, BLE, MAC(2.4G) */
	/*if(APP_Wireless_Concurrent_Mode() == BLE_ENABLE){
		if(APP_BLE_CONNECTED_SERVER == APP_BLE_Get_Server_Connection_Status()){
			if(trans_mode != QMK_TRANSMIT_MODE_BLE){
				if(trans_mode == QMK_TRANSMIT_MODE_USB){
					last_host_driver = host_get_driver();
				}

				host_set_driver(&HIDS_BLE_Drivers);

				trans_mode = QMK_TRANSMIT_MODE_BLE;
			}
		} else {
			if(trans_mode != QMK_TRANSMIT_MODE_USB){
				host_set_driver(last_host_driver);

				trans_mode = QMK_TRANSMIT_MODE_USB;
			}
		}
	} else if(APP_Wireless_Concurrent_Mode() == MAC_ENABLE){
		if(APP_MAC_CONNECTED == APP_MAC_ConnectState()){
			if(trans_mode != QMK_TRANSMIT_MODE_MAC){
				if(trans_mode == QMK_TRANSMIT_MODE_USB){
					last_host_driver = host_get_driver();
				}

				host_set_driver(&HIDS_MAC_Drivers);

				trans_mode = QMK_TRANSMIT_MODE_MAC;
			}
		} else {
			if(trans_mode != QMK_TRANSMIT_MODE_USB){
				host_set_driver(last_host_driver);

				trans_mode = QMK_TRANSMIT_MODE_USB;
			}
		}
	}

	switch(trans_mode){
	case QMK_TRANSMIT_MODE_USB:
		// TODO
		break;
	case QMK_TRANSMIT_MODE_BLE:
		if(TIMER_DIFF_16(now_ms, last_ms) > 1000){
			BASAPP_Level();

			last_ms = now_ms;
		}
		break;
	case QMK_TRANSMIT_MODE_MAC:
		// TODO
		break;
	default:
		// error
		break;
	}
	*/
	return;
}
