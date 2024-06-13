/* Copyright 2021 Glorious, LLC <salman@pcgamingrace.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include_next <mcuconf.h>

/*
 * ST, yiwei
 */
//#define CFG_HW_RESET_BY_FW                  TRUE
#define CFG_HW_RESET_BY_FW                  FALSE

//#undef STM32_PLLSRC
//#define STM32_PLLSRC                        STM32_PLLSRC_HSI16

//#undef STM32_PLLN_VALUE
//#define STM32_PLLN_VALUE                    8

//#undef STM32_C2HPRE
//#define STM32_C2HPRE                        STM32_C2HPRE_DIV1

//#undef STM32_PLLSAI1N_VALUE
//#define STM32_PLLSAI1N_VALUE                8

//#undef STM32_CLK48SEL
//#define STM32_CLK48SEL                      STM32_CLK48SEL_HSI48

#define STM32_MSI_ENABLED                   FALSE

//#undef STM32_RTCSEL
//#define STM32_RTCSEL                        STM32_RTCSEL_LSE
//
//#undef STM32_LSI1_ENABLED
//#define STM32_LSI1_ENABLED                  FALSE
//
//#undef STM32_LSE_ENABLED
//#define STM32_LSE_ENABLED                   TRUE


/* option 1*/
//#undef STM32_RTCSEL
//#define STM32_RTCSEL                        STM32_RTCSEL_LSI

//#undef STM32_LSI1_ENABLED
//#define STM32_LSI1_ENABLED                  TRUE

//#undef STM32_LSE_ENABLED
//#define STM32_LSE_ENABLED                   FALSE

/*
 * IPCC unit.
 */
#define HAL_USE_IPCC                        TRUE
#define STM32_IPCC_C1_RX_HANDLER            VectorF0
#define STM32_IPCC_C1_TX_HANDLER            VectorF4
#define STM32_IPCC_C1_RX_NUMBER             44
#define STM32_IPCC_C1_TX_NUMBER             45
#define STM32_IPCC_C1_RX_IRQ_PRIORITY       10 /*0*/
#define STM32_IPCC_C1_TX_IRQ_PRIORITY       10 /*1*/

/*
 * WS2812
 */
#undef STM32_PWM_USE_TIM1
#define STM32_PWM_USE_TIM1                  TRUE

/* end of ST, yiwei */

//#undef STM32_ADC_USE_ADC1
//#define STM32_ADC_USE_ADC1 TRUE

//#undef STM32_SPI_USE_SPI1
//#define STM32_SPI_USE_SPI1 TRUE
