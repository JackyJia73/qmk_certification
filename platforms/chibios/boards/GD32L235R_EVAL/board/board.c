/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"

/**
 * @brief   PAL setup.
 * @details Digital I/O ports static configuration as defined in @p board.h.
 *          This variable is used by the HAL when initializing the PAL driver.
 */
// #if HAL_USE_PAL || defined(__DOXYGEN__)
// const PALConfig pal_default_config =  //??????
// {
//   {VAL_GPIOAODR, VAL_GPIOACRL, VAL_GPIOACRH},
//   {VAL_GPIOBODR, VAL_GPIOBCRL, VAL_GPIOBCRH},
//   {VAL_GPIOCODR, VAL_GPIOCCRL, VAL_GPIOCCRH},
//   {VAL_GPIODODR, VAL_GPIODCRL, VAL_GPIODCRH},
// };
// #endif

/*
 * Early initialization code.
 * This initialization must be performed just after stack setup and before
 * any other initialization.
 */
void __early_init(void) {

  gd32_clock_init();
}

/*
 * Board-specific initialization code.
 */
void boardInit(void) {
  /*
   * Configures LED and push-button clock
   */
  rcuEnableAHB(CLK_GPIO_LED1, 0);
  rcuEnableAHB(CLK_GPIO_LED2, 0);
  rcuEnableAHB(CLK_GPIO_WAKEUP, 0);
  rcuEnableAHB(CLK_GPIO_TAMPER, 0);

  /*
   * Configures LED and push-button GPIOs
   */
  palSetPadMode(PORT_LED1, PIN_LED1, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(PORT_LED2, PIN_LED2, PAL_MODE_OUTPUT_PUSHPULL);

  palSetPadMode(GPIOA, 0, PAL_MODE_INPUT);

  palSetPadMode(PORT_WAKEUP, PIN_WAKEUP, PAL_MODE_INPUT);
  palSetPadMode(PORT_TAMPER, PIN_TAMPER, PAL_MODE_INPUT);

}
