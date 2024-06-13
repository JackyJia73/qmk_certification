
/**
  ******************************************************************************
  * @file    hids_app.h
  * @author  MCD Application Team
  * @brief   Header for hids_app.c module
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
#ifndef HIDS_APP_H
#define HIDS_APP_H

#ifdef __cplusplus
extern "C" 
{
#endif

  /* Includes ------------------------------------------------------------------*/
#include "report.h"
#include "host_driver.h"

  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* External variables --------------------------------------------------------*/
  /* Exported macros -----------------------------------------------------------*/
  /* Exported functions ------------------------------------------------------- */
  void HIDSAPP_Init(void);
  void HIDSAPP_Stop(void);

  void HIDSAPP_Report_Keyboard(report_keyboard_t *report);
  void HIDSAPP_Report_NKRO(report_nkro_t *report);
  void HIDSAPP_Report_Mouse(report_mouse_t *report);
  void HIDSAPP_Report_Extra(report_extra_t *report);
  uint8_t HIDSAPP_Get_LED_Report(void);

#ifdef __cplusplus
}
#endif

#endif /* HIDS_APP_H */
