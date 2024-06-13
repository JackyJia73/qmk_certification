/**
  ******************************************************************************
 * @file    bas_app.c
 * @author  MCD Application Team
 * @brief   Battery Service Application
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
#include "app_common.h"

#include "bas_app.h"
#include "ble.h"

#include "app_ble.h"


/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint16_t  Level;
  uint8_t   TimerLevel_Id;
  uint8_t   enabled;
} BSAAPP_Context_t;


/* Private defines -----------------------------------------------------------*/
#define BASAPP_DEFAULT_BAT_LEVEL       100  /**100% */


/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

BSAAPP_Context_t BASAPP_Context[BLE_CFG_BAS_NUMBER];

/**
 * END of Section BLE_APP_CONTEXT
 */

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

void BAS_Notification(BAS_Notification_evt_t *pNotification)
{
  switch(pNotification->BAS_Evt_Opcode)
  {
    case BAS_LEVEL_NOT_ENABLED_EVT:
      {
    	  BASAPP_Context[pNotification->ServiceInstance].enabled = 1;

        //aci_gatt_store_db();
      }
      break;

    case BAS_LEVEL_NOT_DISABLED_EVT:
      {
    	  BASAPP_Context[pNotification->ServiceInstance].enabled = 0;

        //aci_gatt_store_db();
      }
      break;

    case BAS_LEVEL_READ_EVT:
      {
    	  if(!BASAPP_Context[pNotification->ServiceInstance].enabled){
    		  break;
    	  }

        if(BASAPP_Context[pNotification->ServiceInstance].Level > 0)
          BASAPP_Context[pNotification->ServiceInstance].Level -= 1;
        else
          BASAPP_Context[pNotification->ServiceInstance].Level = BASAPP_DEFAULT_BAT_LEVEL;
        BAS_Update_Char(BATTERY_LEVEL_CHAR_UUID,
                        pNotification->ServiceInstance,
                        (uint8_t *)&BASAPP_Context[pNotification->ServiceInstance].Level);
      }
      break;

    default:
      break;
  }

  return;
}

void BASAPP_Init(uint8_t index)
{
  /*
   * default status is enabled
   */
  for(int i = 0; i < BLE_CFG_BAS_NUMBER; i++)
  {
	BASAPP_Context[i].enabled = 1;
  }

  /**
   * Initialize Level
   */
  BASAPP_Context[index].Level = BASAPP_DEFAULT_BAT_LEVEL;
  BAS_Update_Char(BATTERY_LEVEL_CHAR_UUID, index, (uint8_t *)&BASAPP_Context[index].Level);

  return;
}


void BASAPP_Level(void)
{
  uint8_t index;
  
  for(index = 0; index < BLE_CFG_BAS_NUMBER; index++)
  {
	if(!BASAPP_Context[index].enabled){
		continue;
	}

    if(BASAPP_Context[index].Level > 50)
      BASAPP_Context[index].Level -= 1;
    else
      BASAPP_Context[index].Level = BASAPP_DEFAULT_BAT_LEVEL;

    BAS_Update_Char(BATTERY_LEVEL_CHAR_UUID, index, (uint8_t *)&BASAPP_Context[index].Level);
  }

  return;
}

void BASAPP_Stop(void){
	/* disable all entries */
	for(int i = 0; i < BLE_CFG_BAS_NUMBER; i++){
		BASAPP_Context[i].enabled = 0;
	}
}
