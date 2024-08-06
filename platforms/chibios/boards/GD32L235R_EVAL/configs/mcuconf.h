/*
    ChibiOS - Copyright (C) 2006..2020 Giovanni Di Sirio

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

#ifndef MCUCONF_H
#define MCUCONF_H

#include "gd32l23x_rcu.h"

/*
 * GD32L23x drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the whole
 * driver is enabled in halconf.h.
 *
 * IRQ priorities:
 * 3...0       Lowest...Highest.
 *
 * DMA priorities:
 * 0...3        Lowest...Highest.
 */

#define GD32L23x_MCUCONF
#define GD32L235RB
#define GD32L23x
#define GD32L235

/*
 * HAL driver system settings.
 */
#define GD32_NO_INIT                       FALSE
#define GD32_IRC16M_ENABLED                TRUE
#define GD32_IRC48M_ENABLED                TRUE
#define GD32_IRC32K_ENABLED                TRUE
#define GD32_HXTAL_ENABLED                 TRUE
#define GD32_LXTAL_ENABLED                 FALSE
#define GD32_SCS                           RCU_CKSYSSRC_PLL
#define GD32_PLLSEL                        RCU_PLLSRC_HXTAL
#define GD32_PLLMF_VALUE                   RCU_PLL_MUL6
#define GD32_PREDV_VALUE                   RCU_PLL_PREDV1
#define GD32_AHBPSC                        RCU_AHB_CKSYS_DIV1
#define GD32_APB1PSC                       RCU_APB1_CKAHB_DIV2
#define GD32_APB2PSC                       RCU_APB2_CKAHB_DIV1
#define GD32_ADCPSC                        RCU_ADCCK_APB2_DIV16
#define GD32_I2S_CLOCK_REQUIRED            FALSE
#define GD32_CKOUTSEL                      RCU_CKOUTSRC_NONE
#define GD32_RTCSRC                        RCU_RTCSRC_NONE
#define GD32_USBD_CLOCK_REQUIRED           TRUE
#define GD32_USBDCLK_SEL                   RCU_USBDSRC_PLL

/*
 * IRQ system settings.
 */
#define GD32_IRQ_EXTI0_PRIORITY            2
#define GD32_IRQ_EXTI1_PRIORITY            2
#define GD32_IRQ_EXTI2_PRIORITY            2
#define GD32_IRQ_EXTI3_PRIORITY            2
#define GD32_IRQ_EXTI4_PRIORITY            2
#define GD32_IRQ_EXTI5_9_PRIORITY          2
#define GD32_IRQ_EXTI10_15_PRIORITY        2

/*
 * ADC driver system settings.
 */
#define GD32_ADC_ADCPRE                   GD32_ADCPSC_DIV16
#define GD32_ADC_USE_ADC                  FALSE
#define GD32_ADC_ADC_DMA_PRIORITY         2
#define GD32_ADC_ADC_IRQ_PRIORITY         3

/*
 * CAN driver system settings.
 */
#define GD32_CAN_USE_CAN0                  FALSE
#define GD32_CAN_CAN0_IRQ_PRIORITY         3

/*
 * DAC driver system settings.
 */
#define GD32_DAC_DUAL_MODE                 FALSE
#define GD32_DAC_USE_DAC0_CH0              FALSE
#define GD32_DAC_DAC0_CH0_IRQ_PRIORITY     3
#define GD32_DAC_DAC0_CH0_DMA_PRIORITY     2
#define GD32_DAC_DAC0_CH0_DMA_STREAM       GD32_DMA_STREAM_ID(1, 5)

/*
 * GPT driver system settings.
 */
#define GD32_GPT_USE_TIM0                        FALSE
#define GD32_GPT_USE_TIM1                        FALSE
#define GD32_GPT_USE_TIM2                        FALSE
#define GD32_GPT_USE_TIM5                        FALSE
#define GD32_GPT_USE_TIM6                        FALSE
#define GD32_GPT_USE_TIM8                        FALSE
#define GD32_GPT_USE_TIM11                       FALSE
#define GD32_GPT_USE_TIM14                       FALSE
#define GD32_GPT_USE_TIM40                       FALSE
#define GD32_IRQ_TIM0_TRG_CMT_UP_BRK_PRIORITY    3
#define GD32_IRQ_TIM0_Channel_PRIORITY           3
#define GD32_IRQ_TIM1_PRIORITY                   3
#define GD32_IRQ_TIM2_PRIORITY                   3
#define GD32_IRQ_TIM5_PRIORITY                   3
#define GD32_IRQ_TIM6_PRIORITY                   3
#define GD32_IRQ_TIM8_PRIORITY                   3
#define GD32_IRQ_TIM11_PRIORITY                  3
#define GD32_IRQ_TIM14_PRIORITY                  3
#define GD32_IRQ_TIM40_PRIORITY                  3

/*
 * I2C driver system settings.
 */
#define GD32_I2C_USE_I2C0                  FALSE
#define GD32_I2C_USE_I2C1                  FALSE
#define GD32_I2C_USE_I2C2                  FALSE
#define GD32_I2C_BUSY_TIMEOUT              50
#define GD32_I2C_I2C0_RX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 0)
#define GD32_I2C_I2C0_TX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 6)
#define GD32_I2C_I2C1_RX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 2)
#define GD32_I2C_I2C1_TX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 7)
#define GD32_I2C_I2C2_RX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 2)
#define GD32_I2C_I2C2_TX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 4)
#define GD32_I2C_I2C0_IRQ_PRIORITY         3
#define GD32_I2C_I2C1_IRQ_PRIORITY         3
#define GD32_I2C_I2C2_IRQ_PRIORITY         3
#define GD32_I2C_I2C0_DMA_PRIORITY         2
#define GD32_I2C_I2C1_DMA_PRIORITY         2
#define GD32_I2C_I2C2_DMA_PRIORITY         2
#define GD32_I2C_DMA_ERROR_HOOK(i2cp)      osalSysHalt("DMA failure")

/*
 * I2S driver system settings.
 */
#define GD32_I2S_USE_SPI1                  FALSE
#define GD32_I2S_SPI1_IRQ_PRIORITY         3
#define GD32_I2S_SPI1_DMA_PRIORITY         1
#define GD32_I2S_SPI1_RX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 3)
#define GD32_I2S_SPI1_TX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 4)
#define GD32_I2S_DMA_ERROR_HOOK(i2sp)      osalSysHalt("DMA failure")

/*
 * ICU driver system settings.
 */
#define GD32_ICU_USE_TIM0                  FALSE
#define GD32_ICU_USE_TIM1                  FALSE
#define GD32_ICU_USE_TIM2                  FALSE
#define GD32_ICU_USE_TIM5                  FALSE
#define GD32_ICU_USE_TIM6                  FALSE
#define GD32_ICU_USE_TIM8                  FALSE
#define GD32_ICU_USE_TIM11                 FALSE
#define GD32_ICU_USE_TIM14                 FALSE
#define GD32_ICU_USE_TIM40                 FALSE

/*
 * PWM driver system settings.
 */
#define GD32_PWM_USE_ADVANCED              FALSE
#define GD32_PWM_USE_TIM0                  FALSE
#define GD32_PWM_USE_TIM1                  FALSE
#define GD32_PWM_USE_TIM2                  FALSE
#define GD32_PWM_USE_TIM5                  FALSE
#define GD32_PWM_USE_TIM6                  FALSE
#define GD32_PWM_USE_TIM8                  FALSE
#define GD32_PWM_USE_TIM11                 FALSE
#define GD32_PWM_USE_TIM14                 FALSE
#define GD32_PWM_USE_TIM40                 FALSE

/*
 * CRC driver system settings.
 */
#define GD32_CRC_USE_CRC0                   FALSE
#define GD32_CRC_CRC0_DMA_IRQ_PRIORITY      14
#define GD32_CRC_CRC0_DMA_PRIORITY          2
#define CRC_USE_DMA                         FALSE
#define CRCSW_USE_CRC1                      FALSE
#define CRCSW_CRC32_TABLE                   FALSE
#define CRCSW_CRC16_TABLE                   FALSE
#define CRCSW_PROGRAMMABLE                  FALSE

/*
 * SERIAL driver system settings.
 */
#define GD32_SERIAL_USE_USART0             FALSE
#define GD32_SERIAL_USE_USART1             FALSE
#define GD32_SERIAL_USE_UART3              FALSE
#define GD32_SERIAL_USE_UART4              FALSE
#define GD32_IRQ_USART0_PRIORITY           3
#define GD32_IRQ_USART1_PRIORITY           3
#define GD32_IRQ_UART3_PRIORITY            3
#define GD32_IRQ_UART4_PRIORITY            3

/*
 * SPI driver system settings.
 */
#define GD32_SPI_USE_SPI0                  FALSE
#define GD32_SPI_USE_SPI1                  FALSE
#define GD32_SPI_SPI0_RX_DMA_STREAM        GD32_DMA_STREAM_ID(2, 0)
#define GD32_SPI_SPI0_TX_DMA_STREAM        GD32_DMA_STREAM_ID(2, 3)
#define GD32_SPI_SPI1_RX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 3)
#define GD32_SPI_SPI1_TX_DMA_STREAM        GD32_DMA_STREAM_ID(1, 4)
#define GD32_SPI_SPI0_DMA_PRIORITY         1
#define GD32_SPI_SPI1_DMA_PRIORITY         1
#define GD32_SPI_SPI0_IRQ_PRIORITY         2
#define GD32_SPI_SPI1_IRQ_PRIORITY         2
#define GD32_SPI_DMA_ERROR_HOOK(spip)      osalSysHalt("DMA failure")

/*
 * ST driver system settings.
 */
#define GD32_ST_IRQ_PRIORITY               3
#define GD32_ST_USE_TIMER                  1

/*
 * UART driver system settings.
 */
#define GD32_UART_USE_USART0               FALSE
#define GD32_UART_USE_USART1               FALSE
#define GD32_UART_USE_UART3                FALSE
#define GD32_UART_USE_UART4                FALSE
#define GD32_UART_USART0_RX_DMA_STREAM     GD32_DMA_STREAM_ID(2, 5)
#define GD32_UART_USART0_TX_DMA_STREAM     GD32_DMA_STREAM_ID(2, 7)
#define GD32_UART_USART1_RX_DMA_STREAM     GD32_DMA_STREAM_ID(1, 5)
#define GD32_UART_USART1_TX_DMA_STREAM     GD32_DMA_STREAM_ID(1, 6)
#define GD32_UART_UART3_RX_DMA_STREAM      GD32_DMA_STREAM_ID(1, 2)
#define GD32_UART_UART3_TX_DMA_STREAM      GD32_DMA_STREAM_ID(1, 4)
#define GD32_UART_UART4_RX_DMA_STREAM      GD32_DMA_STREAM_ID(1, 0)
#define GD32_UART_UART4_TX_DMA_STREAM      GD32_DMA_STREAM_ID(1, 7)
#define GD32_UART_USART0_DMA_PRIORITY      0
#define GD32_UART_USART1_DMA_PRIORITY      0
#define GD32_UART_UART3_DMA_PRIORITY       0
#define GD32_UART_UART4_DMA_PRIORITY       0
#define GD32_UART_DMA_ERROR_HOOK(uartp)    osalSysHalt("DMA failure")

/*
 * USB driver system settings.
 */
#define GD32_USB_USE_USBD                  TRUE
#define GD32_USB_LOW_POWER_ON_SUSPEND      FALSE
#define GD32_USB_USBD_HP_IRQ_PRIORITY      2
#define GD32_USB_USBD_LP_IRQ_PRIORITY      3

/*
 * WDG driver system settings.
 */
#define GD32_WDG_USE_IWDG                  FALSE

#endif /* MCUCONF_H */
