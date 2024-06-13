/**
  ******************************************************************************
  * @file    hids_app.c
  * @author  MCD Application Team
  * @brief   Human Interface Device Service Application
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



/* Includes ------------------------------------------------------------------*/
//#include <stdlib.h>

#include "quantum.h"
#include "app_common.h"

#include "app_ble.h"
#include "ble.h"
#include "hids_app.h"
#include "stm32_seq.h"
#include <time.h>
#include "ch.h"
#include "hal.h"
#include "keycode_config.h"
#include "print.h"

/* Private typedef -----------------------------------------------------------*/
#if((BLE_CFG_HIDS_INPUT_REPORT_NB != 0) || (BLE_CFG_HIDS_KEYBOARD_DEVICE != 0) || (BLE_CFG_HIDS_MOUSE_DEVICE != 0))
typedef struct
{
#if(BLE_CFG_HIDS_INPUT_REPORT_NB != 0)
  uint8_t ReportNotificationEnabled[BLE_CFG_HIDS_INPUT_REPORT_NB];
#endif
#if(BLE_CFG_HIDS_KEYBOARD_DEVICE != 0)
  uint8_t KeyboardInputNotificationEnabled;
#endif
#if(BLE_CFG_HIDS_MOUSE_DEVICE != 0)
  uint8_t MouseInputNotificationEnabled;
#endif
} HIDSAPP_Context_t;
#endif

typedef enum {
	HIDS_REPORT_TYPE_OUTPUT,
	HIDS_REPORT_TYPE_INPUT,
	HIDS_REPORT_TYPE_FEATURE
}HidsReportType_t;

/* Private defines -----------------------------------------------------------*/
#define ENABLED         1
#define DISABLED        0

  
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t report_desc[] =
{
#if 0
  0x05, 0x01,        /* Usage Page (Generic Desktop Ctrls)     */
  0x09, 0x06,        /* Usage (Keyboard)                       */
  0xA1, 0x01,        /* Collection (Application)               */
  0x85, 0x01 /*REPORT_ID_KEYBOARD*/,     // Report Id (1 for keyboard)
  0x05, 0x07,        /*   Usage (Key Codes)                    */
  0x19, 0xE0,        /*     Usage Minimum (224)                */
  0x29, 0xE7,        /*     Usage Maximum (231)                */
  0x15, 0x00,        /*     Logical Minimum (0)                */
  0x25, 0x01,        /*     Logical Maximum (1)                */
  0x95, 0x08,        /*     Report Count (8)                   */
  0x75, 0x01,        /*     Report Size (1)                    */
  0x81, 0x02,        /*     Input (Data,Var,Abs)               */
  0x95, 0x01,        /*     Report Count (1)                   */
  0x75, 0x08,        /*     Report Size (8)                    */
  0x81, 0x01,        /*     Input (Const,Array,Abs)            */
  0x95, 0x05,        /*     Report Count (5)                   */
  0x75, 0x01,        /*     Report Size (1)                    */
  0x05, 0x08,        /*     Usage Page (Page# for LEDs)        */
  0x19, 0x01,        /*     Usage Minimum (1)                  */
  0x29, 0x05,        /*     Usage Maximum (5)                  */
  0x91, 0x02,        /*     Output(Data, Variable, Absolute), ;LED report */
  0x95, 0x01,        /*     Report Count(1)                               */
  0x75, 0x03,        /*     Report Size(3)                                */
  0x91, 0x01,        /*     Output(Constant), ;LED report padding         */
  0x95, 0x06,        /*     Report Count(6)                               */
  0x75, 0x08,        /*     Report Size(8)                                */
  0x15, 0x00,        /*     Logical Minimum (0)                           */
  0x25, 0x65,        /*     Logical Maximum (101)                         */
  0x05, 0x07,        /*     Usage Page (Key Codes)                        */
  0x19, 0x00,        /*     Usage Minimum (0)                             */
  0x29, 0x65,        /*     Usage Maximum (101)                           */
  0x81, 0x00,        /*     Input (Data Array), ;Key arrays(6 bytes)      */
  0xC0,               /* End Collection                                    */


  0x05, 0x01,         /* USAGE_PAGE (Generic Desktop) */
  0x09, 0x02,         /* USAGE (Mouse) */
  0xa1, 0x01,         /* COLLECTION (Application) */
  0x85, 0x02 /*REPORT_ID_MOUSE*/,              // Report ID (2)
  0x09, 0x01,         /*   USAGE (Pointer) */
  0xa1, 0x00,         /*   COLLECTION (Physical) */
  0x05, 0x09,         /*     USAGE_PAGE (Button) */
  0x19, 0x01,         /*     USAGE_MINIMUM (Button 1) */
  0x29, 0x03,         /*     USAGE_MAXIMUM (Button 3) */
  0x15, 0x00,         /*     LOGICAL_MINIMUM (0) */
  0x25, 0x01,         /*     LOGICAL_MAXIMUM (1) */
  0x95, 0x03,         /*     REPORT_COUNT (3) */
  0x75, 0x01,         /*     REPORT_SIZE (1) */
  0x81, 0x02,         /*     INPUT (Data,Var,Abs) */
  0x95, 0x01,         /*     REPORT_COUNT (1) */
  0x75, 0x05,         /*     REPORT_SIZE (5) */
  0x81, 0x03,         /*     INPUT (Cnst,Var,Abs) */
  0x05, 0x01,         /*     USAGE_PAGE (Generic Desktop) */
  0x09, 0x30,         /*     USAGE (X) */
  0x09, 0x31,         /*     USAGE (Y) */
  0x09, 0x38,         /*     USAGE (Wheel) */
  0x15, 0x81,         /*     LOGICAL_MINIMUM (-127) */
  0x25, 0x7f,         /*     LOGICAL_MAXIMUM (127) */
  0x75, 0x08,         /*     REPORT_SIZE (8) */
  0x95, 0x03,         /*     REPORT_COUNT (3) */
  0x81, 0x06,         /*     INPUT (Data,Var,Rel) */
  0xc0,               /*   END_COLLECTION (Physical) */
  0xc0,               /* END_COLLECTION (Application) */

#else
  	  0x05, 0x01,     // Usage Pg (Generic Desktop)
      0x09, 0x06,     // Usage (Keyboard)
      0xA1, 0x01,     // Collection: (Application)
      0x85, REPORT_ID_KEYBOARD,     // Report Id (1 for keyboard)
      //
      0x05, 0x07,     // Usage Pg (Key Codes)
      0x19, 0xE0,     // Usage Min (224)
      0x29, 0xE7,     // Usage Max (231)
      0x15, 0x00,     // Log Min (0)
      0x25, 0x01,     // Log Max (1)
      //
      // Modifier byte
      0x75, 0x01,     // Report Size (1)   1 bit * 8
      0x95, 0x08,     // Report Count (8)
      0x81, 0x02,     // Input: (Data, Variable, Absolute)

      // Reserved byte
      0x95, 0x01, // Report Count (1)
      0x75, 0x08, // Report Size (8)
      0x81, 0x01, // Input: (Constant)

      //LED repor
      0x95, 0x05,    //Report Count (5)
      0x75, 0x01,    //Report Size (1)
      0x05, 0x08,    //Usage Pg (LEDs )
      0x19, 0x01,    //Usage Min
      0x29, 0x05,    //Usage Max
      0x91, 0x02,    //Output (Data, Variable, Absolute)
      //3 bit reserved
      0x95, 0x01,    //Report Count (1)
      0x75, 0x03,    //Report Size (3)
      0x91, 0x01,    //Output (Constant)

      // Key arrays (6 bytes)
      // this is key array,support simultaneously pressing 6keys report,
      // from report_buf[3]~report_buf[3]
      0x95, 0x06,     // Report Count (6)
      0x75, 0x08,     // Report Size (8)
      0x15, 0x00,     // Log Min (0)
      0x25, 0xE7,     // Log Max (237)
      0x05, 0x07,     // Usage Pg (Key Codes) , here is the key page,look usb hid key define
      0x19, 0x00,     // Usage Min (0)
      0x29, 0xE7,     // Usage Max (237)
      0x81, 0x00,     // Input: (Data, Array)
      0xC0,            // End Collection


  //Mouse
  //    typedef struct {
  //    uint8_t report_id;
  //    uint8_t buttons;
  //     uint8_t x;
  //     uint8_t y;
  //     int8_t            v;
  //     int8_t            h;
  // } __attribute__((packed)) report_mouse_t;

      /// MOUSE MAP
      0x05, 0x01,                         // Usage Page (Generic Desktop Ctrls)
      0x09, 0x02,                         // Usage (Mouse)
      0xA1, 0x01,                         // Collection (Application)
      0x85, REPORT_ID_MOUSE,              // Report ID (2)
      0x09, 0x01,                         // Usage (Pointer)
      0xA1, 0x00,                         // Collection (Physical)
      /// BUTTON INPUT
      0x05, 0x09,                         // Usage Page (Button)
      0x19, 0x01,                         // Usage Minimum (0x01)
      0x29, 0x08,                         // Usage Maximum (0x08)
      0x15, 0x00,                         // Logical Minimum (0)
      0x25, 0x01,                         // Logical Maximum (1)
      0x95, 0x08,                         // Report Count (8)
      0x75, 0x01,                         // Report Size (1)
      0x81, 0x02,                         // Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

      /// X Y INPUT
      0x05, 0x01,                         // Usage Page (Generic Desktop Ctrls)
      0x09, 0x30,                         // Usage (X)
      0x09, 0x31,                         // Usage (Y)
      0x15, 0x81,                         // Logical Minimum (-127)
      0x25, 0x7F,                         // Logical Maximum (127)
      0x95, 0x02,                         // Report Count (2)
      0x75, 0x08,                         // Report Size (8)
      0x81, 0x06,                         // Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
      /// WHEEL INPUT
      0x09, 0x38,                         // Usage (Wheel)
      0x15, 0x81,                         // Logical Minimum (-127)
      0x25, 0x7F,                         // Logical Maximum (127)
      0x95, 0x01,                         // Report Count (1)
      0x75, 0x08,                         // Report Size (8)
      0x81, 0x06,                         // Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
      /// AC PAN INPUT
      0x05, 0x0C,                         // Usage Page (Consumer)
      0x0A, 0x38, 0x02,                   // Usage (AC Pan)
      0x15, 0x81,                         // Logical Minimum (-127)
      0x25, 0x7F,                         // Logical Maximum (127)
      0x95, 0x01,                         // Report Count (1)
      0x75, 0x08,                         // Report Size (8)
      0x81, 0x06,                         // Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
      0xC0,                               // End Collection
      0xC0,                               // End Collection



  //SYSTEM
  // typedef struct {
  //     uint8_t  report_id;
  //     uint16_t usage;
  // } __attribute__((packed)) report_extra_t;
      /// SYSTEM MAP
      0x05, 0x01,                         // Usage Page (Generic Desktop Ctrls)
      0x09, 0x80,                         // Usage (Sys Control)
      0xA1, 0x01,                         // Collection (Application)
      0x85, REPORT_ID_SYSTEM,             // Report ID (3)
      0x19, 0x01,                         // Usage Minimum (Pointer)
      0x2A, 0xB7, 0x00,                   //   Usage Maximum (System Display LCD Autoscale)
      0x15, 0x01,                         //   Logical Minimum
      0x26, 0xB7, 0x00,                   //   Logical Maximum
      0x95, 0x01,                         // Report Count (1)
      0x75, 0x10,                         // Report Size (16)
      0x81, 0x00,                         //   Input (Data, Array, Absolute)
      0xC0,                               // End Collection






  //CONSUMER
  // typedef struct {
  //     uint8_t  report_id;
  //     uint16_t usage;
  // } __attribute__((packed)) report_extra_t;
      /// CONSUMER MAP
      0x05, 0x0C,                         // Usage Page (Consumer)
      0x09, 0x01,                         // Usage (Consumer Control)
      0xA1, 0x01,                         // Collection (Application)
      0x85, REPORT_ID_CONSUMER,           // Report ID (4)
      0x19, 0x01,                         // Usage Minimum (Consumer Control)
      0x2A, 0xA0, 0x02,                   // Usage Maximum (0x02A0)
      0x15, 0x01,                         // Logical Minimum (1)
      0x26, 0xA0, 0x02,                   // Logical Maximum (672)
      0x95, 0x01,                         // Report Count (1)
      0x75, 0x10,                         // Report Size (16)
      0x81, 0x00,                         // Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
      0xC0,                               // End Collection





  //NKRO
  // typedef union {
  //     uint8_t raw[KEYBOARD_REPORT_SIZE];
  //     struct nkro_report {
  //         uint8_t report_id;
  //         uint8_t mods;
  //         uint8_t bits[KEYBOARD_REPORT_BITS];
  //     } nkro;
  // } __attribute__((packed)) report_keyboard_t;

  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x85, REPORT_ID_NKRO,        //   Report ID (6)
  // Modifiers (8 bits)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0xE0,        //   Usage Minimum (0xE0) LEFT CONTROL
  0x29, 0xE7,        //   Usage Maximum (0xE7) GUI
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x95, 0x08,        //   Report Count (8)
  0x75, 0x01,        //   Report Size (1)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  //KEYCODES
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0x00,        //   Usage Minimum (0x00)
  0x29, 0xEF,        //   Usage Maximum (0xEF) KEYBOARD_REPORT_BITS * 8 - 1 =239
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  //0x95, 0xF0,        //   Report Count (-16) KEYBOARD_REPORT_BITS * 8 = 240
  0x95, 0x78,        //   Report Count (0x78) KEYBOARD_REPORT_BITS * 8 = 120
  0x75, 0x01,        //   Report Size (1)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  // Status LEDs (5 bits)
  0x05, 0x08,        //   Usage Page (LEDs)
  0x19, 0x01,        //   Usage Minimum (Num Lock)
  0x29, 0x05,        //   Usage Maximum (Kana)
  0x95, 0x05,        //   Report Count (5)
  0x75, 0x01,        //   Report Size (1)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  // LED padding (3 bits)
  0x95, 0x01,        //   Report Count (1)
  0x75, 0x03,        //   Report Size (3)
  0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection



#if 0
  //RAW_HID
  // BLEÖÐÔö¼Ó´Ëreport£¬  32¸ö×Ö½ÚµÄÓÃ»§×Ô¶¨ÒåÊý¾Ý¡£
  // Dongle ÖÐ¶¨ÒåÎª interface # 1
  // It is important that the Raw HID interface is at a constant
  // interface number, to support Linux/OSX platforms and chrome.hid
  // If Raw HID is enabled, let it be always 1.


  // typedef union {
  //     uint8_t raw[KEYBOARD_REPORT_SIZE];
  //     struct nkro_report {
  //         uint8_t report_id;
  //         uint8_t mods;
  //         uint8_t bits[KEYBOARD_REPORT_BITS];
  //     } nkro;
  // } __attribute__((packed)) report_keyboard_t;

      0x06, 0x60, 0xFF, // Usage Page (Vendor Defined)  RAW_USAGE_PAGE = 0xFF60
      0x09, 0x61,                 // Usage (Vendor Defined)   RAW_USAGE_ID = 0x61
      0xA1, 0x01,                         // Collection (Application)
      0x85, REPORT_ID_RAW_HID,        //   Report ID (9)
      // Data to host
      0x09, 0x62,            //   Usage (Vendor Defined)
      0x15, 0x00,            //   Logical Minimum (0)
      0x26, 0xFF, 0x00,      //   Logical Maximum (255)
      0x95, 0x20,          //   Report Count  RAW_BUFFER_SIZE= 32
      0x75, 0x08,            //   Report Size (8)
      0x81, 0x02,            //   Input (Data, Variable, Absolute)
      // Data from host
      0x09, 0x63,            //   Usage (Vendor Defined)
      0x15, 0x00,            //   Logical Minimum (0)
      0x26, 0xFF, 0x00,      //   Logical Maximum (255)
      0x95, 0x20,          //   Report Count  RAW_BUFFER_SIZE= 32
      0x75, 0x08,            //   Report Size (8)
      0x91, 0x02,            //   Output (Data, Variable, Absolute)
      0xC0                   // End Collection
#endif

#endif
};


static int char_report_enabled = DISABLE;

static uint8_t led_report_value = 0;

static uint8_t index_report_keyboard_input;
static uint8_t index_report_mouse_input;
static uint8_t index_report_system_input;
static uint8_t index_report_consumer_input;
static uint8_t index_report_nkro_input;

/**
 * START of Section BLE_APP_CONTEXT
 */

#if((BLE_CFG_HIDS_INPUT_REPORT_NB != 0) || (BLE_CFG_HIDS_KEYBOARD_DEVICE != 0) || (BLE_CFG_HIDS_MOUSE_DEVICE != 0))
HIDSAPP_Context_t HIDSAPP_Context[BLE_CFG_HIDS_NUMBER];
#endif

/**
 * END of Section BLE_APP_CONTEXT
 */

/* Global variables ----------------------------------------------------------*/
uint8_t hid_ref_descs[BLE_CFG_HIDS_INPUT_REPORT_NB + BLE_CFG_HIDS_OUTPUT_REPORT_NB] = {
	/*
	 * input
	*/
	/* Report ID */
	REPORT_ID_KEYBOARD,
	REPORT_ID_MOUSE,
	REPORT_ID_SYSTEM,
	REPORT_ID_CONSUMER,
	REPORT_ID_NKRO,
	/*
	 * output
	 */
	REPORT_ID_KEYBOARD,
	REPORT_ID_NKRO
};

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/
static int ReportID2CharIndex(int report_id, HidsReportType_t t){
	if(t == HIDS_REPORT_TYPE_INPUT){
		if(BLE_CFG_HIDS_INPUT_REPORT_NB == 0){
			return -1;
		}

		for(int i=0; i<BLE_CFG_HIDS_INPUT_REPORT_NB; i++){
			if(hid_ref_descs[i] == report_id){
				return i;
			}
		}
	} else if(t == HIDS_REPORT_TYPE_OUTPUT){
		if(BLE_CFG_HIDS_OUTPUT_REPORT_NB == 0){
			return -2;
		}

		for(int i=BLE_CFG_HIDS_INPUT_REPORT_NB; i<(BLE_CFG_HIDS_INPUT_REPORT_NB + BLE_CFG_HIDS_INPUT_REPORT_NB); i++){
			if(hid_ref_descs[i] == report_id){
				return i;
			}
		}
	}

	return -100;
}

/* Public functions ----------------------------------------------------------*/
void HIDS_Notification(HIDS_App_Notification_evt_t *pNotification)
{
  switch(pNotification->HIDS_Evt_Opcode)
  {

#if(BLE_CFG_HIDS_INPUT_REPORT_NB != 0)
    case HIDS_REPORT_NOTIFICATION_ENABLED:
      {
        BLE_DBG_APP_MSG("HIDS_REPORT_NOTIFICATION_ENABLED\n");
        HIDSAPP_Context[pNotification->Index].ReportNotificationEnabled[pNotification->Index] = ENABLED;

        //aci_gatt_store_db();
      }
      break;

    case HIDS_REPORT_NOTIFICATION_DISABLED:
      {
        BLE_DBG_APP_MSG("HIDS_REPORT_NOTIFICATION_DISABLED\n");
        HIDSAPP_Context[pNotification->Index].ReportNotificationEnabled[pNotification->Index] = DISABLED;

        //aci_gatt_store_db();
      }
      break;
#endif
      
#if(BLE_CFG_HIDS_KEYBOARD_DEVICE != 0)
    case HIDS_KEYB_INPUT_NOTIFY_ENABLED:
      {
        BLE_DBG_APP_MSG("HIDS_KEYB_INPUT_NOTIFY_ENABLED\n");
        HIDSAPP_Context[pNotification->Index].KeyboardInputNotificationEnabled = ENABLED;

        //aci_gatt_store_db();
      }
      break;

    case HIDS_KEYB_INPUT_NOTIFY_DISABLED:
      {
        BLE_DBG_APP_MSG("HIDS_KEYB_INPUT_NOTIFY_DISABLED\n");
        HIDSAPP_Context[pNotification->Index].KeyboardInputNotificationEnabled = DISABLED;

        //aci_gatt_store_db();
      }
      break;
#endif

#if(BLE_CFG_HIDS_MOUSE_DEVICE != 0)          
    case HIDS_MOUSE_INPUT_NOTIFY_ENABLED:
      {
        BLE_DBG_APP_MSG("HIDS_MOUSE_INPUT_NOTIFY_ENABLED\n");
        HIDSAPP_Context[pNotification->Index].MouseInputNotificationEnabled = ENABLED;

        //aci_gatt_store_db();
      }
      break;

    case HIDS_MOUSE_INPUT_NOTIFY_DISABLED:
      {
        BLE_DBG_APP_MSG("HIDS_MOUSE_INPUT_NOTIFY_DISABLED\n");
        HIDSAPP_Context[pNotification->Index].MouseInputNotificationEnabled = DISABLED;

        //aci_gatt_store_db();
      }
      break;
#endif

#if(BLE_CFG_HIDS_REPORT_CHAR != 0)
    case HIDS_OUTPUT_REPORT:
      {
//        uint8_t i;
//
//        BLE_DBG_APP_MSG("HIDS_OUTPUT_REPORT\n");
//        BLE_DBG_HIDS_MSG("HID Instance %d Report %d \n",
//                          pNotification->Instance,
//                          pNotification->Index);

        if(pNotification->Index >= sizeof(hid_ref_descs)){
        	return; // invalid report
        }

        if(keymap_config.nkro){
        	if(hid_ref_descs[pNotification->Index] != REPORT_ID_NKRO){
        		return; // invalid report
        	}
        } else {
        	if(hid_ref_descs[pNotification->Index] != REPORT_ID_KEYBOARD){
        		return; // invalid report
        	}
        }

        // update LED states
        led_report_value = pNotification->pReport[0];

//        for(i = 0; i < pNotification->ReportLength; i++)
//          BLE_DBG_HIDS_MSG("Report[%d] 0x%X \n",
//                           i,
//                           pNotification->pReport[i]);
      }
      break;
#endif
    
#if((BLE_CFG_HIDS_MOUSE_DEVICE != 0) && (BLE_CFG_HIDS_MOUSE_INPUT_WRITE != 0))
    case HIDS_MOUSE_INPUT_REPORT:
      {
        uint8_t i;
        
        BLE_DBG_APP_MSG("HIDS_MOUSE_INPUT_REPORT\n");
        BLE_DBG_HIDS_MSG("HID Instance %d Report %d \n", 
                          pNotification->Instance,
                          pNotification->Index); 
    
        for(i = 0; i < pNotification->ReportLength; i++)
          BLE_DBG_HIDS_MSG("Report[%d] 0x%X \n",
                           i,
                           pNotification->pReport[i]);
      }
      break;
#endif
      
#if((BLE_CFG_HIDS_KEYBOARD_DEVICE != 0) && (BLE_CFG_HIDS_KEYBOARD_INPUT_WRITE != 0))
    case HIDS_KEYBOARD_INPUT_REPORT:
      {
        uint8_t i;
        
        BLE_DBG_APP_MSG("HIDS_KEYBOARD_INPUT_REPORT\n");
        BLE_DBG_HIDS_MSG("HID Instance %d Report %d \n", 
                          pNotification->Instance,
                          pNotification->Index); 
    
        for(i = 0; i < pNotification->ReportLength; i++)
          BLE_DBG_HIDS_MSG("Report[%d] 0x%X \n",
                           i,
                           pNotification->pReport[i]);
      }
      break;

    case HIDS_KEYBOARD_OUTPUT_REPORT:
      {
        uint8_t i;
        
        BLE_DBG_APP_MSG("HIDS_KEYBOARD_OUTPUT_REPORT\n");
        BLE_DBG_HIDS_MSG("HID Instance %d Report %d \n", 
                          pNotification->Instance,
                          pNotification->Index); 
    
        for(i = 0; i < pNotification->ReportLength; i++)
          BLE_DBG_HIDS_MSG("Report[%d] 0x%X \n",
                           i,
                           pNotification->pReport[i]);
      }
      break;
#endif
      
    default:
      break;
  }

  return;
}


void HIDSAPP_Init(void)
{
  tBleStatus result;

  result = HIDS_Update_Char(REPORT_MAP_CHAR_UUID,
                            0,
                            0,
                            sizeof(report_desc),
                            (uint8_t *)&report_desc);

  if( result == BLE_STATUS_SUCCESS )
  {
    BLE_DBG_APP_MSG("Report Map Successfully Sent\n");
  }
  else 
  {
    BLE_DBG_APP_MSG("Sending of Report Map Failed error 0x%X\n", result);

    return;
  }

  /*
   * HID Information
   */
  uint8_t info[4] = {
	0x01, /* bcdHID, 2 octets, 0x0111, HID Class Specification release number 1.11 */
	0x11,
	0x00, /* bcountryCode, 1 octet, not localized */
	0x00, /* Flag, 1 octet, bit0 -> RemoteWake, bit1 -> NormallyConnectable */
  };
  result = HIDS_Update_Char(HID_INFORMATION_CHAR_UUID,
                            0,
                            0,
                            4,
                            (uint8_t *)&info);

  if(result != BLE_STATUS_SUCCESS){
	  return;
  }

  /*
   * Get index of each report ID
   * Update corresponding characteristic with the index of report ID
   */
  int index;
  index = ReportID2CharIndex(REPORT_ID_KEYBOARD, HIDS_REPORT_TYPE_INPUT);
  if(index < 0){
	  return;
  }
  index_report_keyboard_input = (uint8_t)index;

  index = ReportID2CharIndex(REPORT_ID_MOUSE, HIDS_REPORT_TYPE_INPUT);
  if(index < 0){
	  return;
  }
  index_report_mouse_input = (uint8_t)index;

  index = ReportID2CharIndex(REPORT_ID_CONSUMER, HIDS_REPORT_TYPE_INPUT);
  if(index < 0){
	  return;
  }
  index_report_consumer_input = (uint8_t)index;

  index = ReportID2CharIndex(REPORT_ID_SYSTEM, HIDS_REPORT_TYPE_INPUT);
  if(index < 0){
	  return;
  }
  index_report_system_input = (uint8_t)index;

  index = ReportID2CharIndex(REPORT_ID_NKRO, HIDS_REPORT_TYPE_INPUT);
  if(index < 0){
	  return;
  }
  index_report_nkro_input = (uint8_t)index;

  char_report_enabled = ENABLE;
}


void HIDSAPP_Report_Keyboard(report_keyboard_t *report){
	tBleStatus result = BLE_STATUS_INVALID_PARAMS;

  //if(report->report_id != REPORT_ID_KEYBOARD){
	//	return; /* invalid report */
	//}
		result = HIDS_Update_Char(REPORT_CHAR_UUID,
								  0,
								  index_report_keyboard_input,
								  sizeof(report_keyboard_t),
								  ((uint8_t*)report));


	if( result == BLE_STATUS_SUCCESS )
	{
	  BLE_DBG_APP_MSG("Keyboard Report Successfully Sent\n");
	}
	else
	{
	  BLE_DBG_APP_MSG("Sending of Keyboard Report Failed error 0x%X\n", result);
	}
}


void HIDSAPP_Report_NKRO(report_nkro_t *report){
	tBleStatus result = BLE_STATUS_INVALID_PARAMS;


  if(report->report_id != REPORT_ID_NKRO){
		return; /* invalid report */
	}
  		result = HIDS_Update_Char(REPORT_CHAR_UUID,
								  0,
								  index_report_nkro_input,
								  sizeof(report_nkro_t) - 1,
								  ((uint8_t*)report) + 1);

	if( result == BLE_STATUS_SUCCESS )
	{
	  BLE_DBG_APP_MSG("Keyboard Report Successfully Sent\n");
	}
	else
	{
	  BLE_DBG_APP_MSG("Sending of Keyboard Report Failed error 0x%X\n", result);
	}
}

void HIDSAPP_Report_Mouse(report_mouse_t *report){
	tBleStatus result = BLE_STATUS_INVALID_PARAMS;

	if(report->report_id != REPORT_ID_MOUSE){
		return; /* invalid report */
	}

	result = HIDS_Update_Char(REPORT_CHAR_UUID,
							  0,
							  index_report_mouse_input,
							  sizeof(report_mouse_t) - 1,
							  ((uint8_t*)report) + 1);

	if( result == BLE_STATUS_SUCCESS )
	{
	  BLE_DBG_APP_MSG("Mouse Report Successfully Sent\n");
	}
	else
	{
	  BLE_DBG_APP_MSG("Sending of Mouse Report Failed error 0x%X\n", result);
	}
}

void HIDSAPP_Report_Extra(report_extra_t *report){
	int index = ReportID2CharIndex(report->report_id, HIDS_REPORT_TYPE_INPUT);
	uint8_t report_index;
	uint8_t data[2];
	tBleStatus result = BLE_STATUS_INVALID_PARAMS;

	if(index < 0){
		return; /* invalid report */
	}

	report_index = (uint8_t)(index & 0xFF);
	data[0] = (uint8_t)(report->usage & 0xFF);
	data[1] = (uint8_t)((report->usage >> 8) & 0xFF);

	result = HIDS_Update_Char(REPORT_CHAR_UUID,
									  0,
									  report_index,
									  2,
									  data);

	if( result == BLE_STATUS_SUCCESS )
	{
	  BLE_DBG_APP_MSG("Extra Report[%X->%X] Successfully Sent\n", report->report_id, report_index);
	}
	else
	{
	  BLE_DBG_APP_MSG("Sending of Extra Report[%X->%X] Failed error 0x%X\n", result, report->report_id, report_index);
	}
}

uint8_t HIDSAPP_Get_LED_Report(void){
	return led_report_value;
}

void HIDSAPP_Stop(void){
	char_report_enabled = DISABLE;
}
