/*
 * ipccv1.h
 *
 *  Created on: Jul 21, 2023
 *      Author: yuyw
 */

#ifndef OS_HAL_PORTS_STM32_LLD_IPCCV1_IPCCV1_H_
#define OS_HAL_PORTS_STM32_LLD_IPCCV1_IPCCV1_H_


#define LL_AHB3_GRP1_PERIPH_IPCC           RCC_AHB3ENR_IPCCEN
#define LL_C2_AHB3_GRP1_PERIPH_IPCC        RCC_C2AHB3ENR_IPCCEN
#define LL_EXTI_LINE_41                EXTI_IMR2_IM41          /*!< Extended line 41 */

/** @defgroup RCC_LL_EC_STOP_WAKEUPCLOCK  Wakeup from Stop and CSS backup clock selection
  * @{
  */
#define LL_RCC_STOP_WAKEUPCLOCK_MSI        0x00000000U             /*!< MSI selection after wake-up from STOP */
#define LL_RCC_STOP_WAKEUPCLOCK_HSI        RCC_CFGR_STOPWUCK       /*!< HSI selection after wake-up from STOP */

#define LL_PWR_MODE_STANDBY                (PWR_CR1_LPMS_1 | PWR_CR1_LPMS_0)
#define LL_PWR_MODE_SHUTDOWN               (PWR_CR1_LPMS_2)

/** @defgroup IPCC_LL_EC_Channel Channel
  * @{
  */
#define LL_IPCC_CHANNEL_1 (0x00000001U) /*!< IPCC Channel 1 */
#define LL_IPCC_CHANNEL_2 (0x00000002U) /*!< IPCC Channel 2 */
#define LL_IPCC_CHANNEL_3 (0x00000004U) /*!< IPCC Channel 3 */
#define LL_IPCC_CHANNEL_4 (0x00000008U) /*!< IPCC Channel 4 */
#define LL_IPCC_CHANNEL_5 (0x00000010U) /*!< IPCC Channel 5 */
#define LL_IPCC_CHANNEL_6 (0x00000020U) /*!< IPCC Channel 6 */


/**
  * @brief  Enable AHB3 peripherals clock.
  * @rmtoll AHB3ENR      QUADSPIEN     LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      PKAEN         LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      AES2EN        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      RNGEN         LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      HSEMEN        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      IPCCEN        LL_AHB3_GRP1_EnableClock\n
  *         AHB3ENR      FLASHEN       LL_AHB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QUADSPI (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AES2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HSEM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IPCC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  * @note  (*) Not supported by all the devices
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->AHB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->AHB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Disable AHB3 peripherals clock.
  * @rmtoll AHB3ENR      QUADSPIEN     LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      PKAEN         LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      AES2EN        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      RNGEN         LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      HSEMEN        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      IPCCEN        LL_AHB3_GRP1_DisableClock\n
  *         AHB3ENR      FLASHEN       LL_AHB3_GRP1_DisableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_AHB3_GRP1_PERIPH_QUADSPI (*)
  *         @arg @ref LL_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_AHB3_GRP1_PERIPH_AES2
  *         @arg @ref LL_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_AHB3_GRP1_PERIPH_HSEM
  *         @arg @ref LL_AHB3_GRP1_PERIPH_IPCC
  *         @arg @ref LL_AHB3_GRP1_PERIPH_FLASH
  * @note  (*) Not supported by all the devices
  * @retval None
  */
__STATIC_INLINE void LL_AHB3_GRP1_DisableClock(uint32_t Periphs)
{
  CLEAR_BIT(RCC->AHB3ENR, Periphs);
}

/**
  * @brief  Clear IPCC receive channel status for processor 1.
  * @note   Associated with IPCC_C2TOC1SR.CHxF
  * @rmtoll C1SCR        CH1C           LL_C1_IPCC_ClearFlag_CHx\n
  *         C1SCR        CH2C           LL_C1_IPCC_ClearFlag_CHx\n
  *         C1SCR        CH3C           LL_C1_IPCC_ClearFlag_CHx\n
  *         C1SCR        CH4C           LL_C1_IPCC_ClearFlag_CHx\n
  *         C1SCR        CH5C           LL_C1_IPCC_ClearFlag_CHx\n
  *         C1SCR        CH6C           LL_C1_IPCC_ClearFlag_CHx
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_ClearFlag_CHx(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  WRITE_REG(IPCCx->C1SCR, Channel);
}

/**
  * @brief  Mask transmit channel free interrupt for processor 1.
  * @rmtoll C1MR        CH1FM           LL_C1_IPCC_DisableTransmitChannel\n
  *         C1MR        CH2FM           LL_C1_IPCC_DisableTransmitChannel\n
  *         C1MR        CH3FM           LL_C1_IPCC_DisableTransmitChannel\n
  *         C1MR        CH4FM           LL_C1_IPCC_DisableTransmitChannel\n
  *         C1MR        CH5FM           LL_C1_IPCC_DisableTransmitChannel\n
  *         C1MR        CH6FM           LL_C1_IPCC_DisableTransmitChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_DisableTransmitChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  SET_BIT(IPCCx->C1MR, Channel << IPCC_C1MR_CH1FM_Pos);
}

/**
  * @brief  Check if Transmit channel free interrupt for processor 1 is masked.
  * @rmtoll C1MR        CH1FM           LL_C1_IPCC_IsEnabledTransmitChannel\n
  *         C1MR        CH2FM           LL_C1_IPCC_IsEnabledTransmitChannel\n
  *         C1MR        CH3FM           LL_C1_IPCC_IsEnabledTransmitChannel\n
  *         C1MR        CH4FM           LL_C1_IPCC_IsEnabledTransmitChannel\n
  *         C1MR        CH5FM           LL_C1_IPCC_IsEnabledTransmitChannel\n
  *         C1MR        CH6FM           LL_C1_IPCC_IsEnabledTransmitChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C1_IPCC_IsEnabledTransmitChannel(IPCC_TypeDef const *const IPCCx, uint32_t Channel)
{
  return ((READ_BIT(IPCCx->C1MR, Channel << IPCC_C1MR_CH1FM_Pos) != (Channel << IPCC_C1MR_CH1FM_Pos)) ? 1UL : 0UL);
}

/**
  * @brief  Unmask receive channel occupied interrupt for processor 1.
  * @rmtoll C1MR        CH1OM           LL_C1_IPCC_EnableReceiveChannel\n
  *         C1MR        CH2OM           LL_C1_IPCC_EnableReceiveChannel\n
  *         C1MR        CH3OM           LL_C1_IPCC_EnableReceiveChannel\n
  *         C1MR        CH4OM           LL_C1_IPCC_EnableReceiveChannel\n
  *         C1MR        CH5OM           LL_C1_IPCC_EnableReceiveChannel\n
  *         C1MR        CH6OM           LL_C1_IPCC_EnableReceiveChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_EnableReceiveChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  CLEAR_BIT(IPCCx->C1MR, Channel);
}

/**
  * @brief  Clear IPCC receive channel status for processor 2.
  * @note   Associated with IPCC_C1TOC2SR.CHxF
  * @rmtoll C2SCR        CH1C           LL_C2_IPCC_ClearFlag_CHx\n
  *         C2SCR        CH2C           LL_C2_IPCC_ClearFlag_CHx\n
  *         C2SCR        CH3C           LL_C2_IPCC_ClearFlag_CHx\n
  *         C2SCR        CH4C           LL_C2_IPCC_ClearFlag_CHx\n
  *         C2SCR        CH5C           LL_C2_IPCC_ClearFlag_CHx\n
  *         C2SCR        CH6C           LL_C2_IPCC_ClearFlag_CHx
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_ClearFlag_CHx(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  WRITE_REG(IPCCx->C2SCR, Channel);
}

/**
  * @brief  Mask transmit channel free interrupt for processor 2.
  * @rmtoll C2MR        CH1FM           LL_C2_IPCC_DisableTransmitChannel\n
  *         C2MR        CH2FM           LL_C2_IPCC_DisableTransmitChannel\n
  *         C2MR        CH3FM           LL_C2_IPCC_DisableTransmitChannel\n
  *         C2MR        CH4FM           LL_C2_IPCC_DisableTransmitChannel\n
  *         C2MR        CH5FM           LL_C2_IPCC_DisableTransmitChannel\n
  *         C2MR        CH6FM           LL_C2_IPCC_DisableTransmitChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_DisableTransmitChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  SET_BIT(IPCCx->C2MR, Channel << (IPCC_C2MR_CH1FM_Pos));
}

/**
  * @brief  Mask receive channel occupied interrupt for processor 1.
  * @rmtoll C1MR        CH1OM           LL_C1_IPCC_DisableReceiveChannel\n
  *         C1MR        CH2OM           LL_C1_IPCC_DisableReceiveChannel\n
  *         C1MR        CH3OM           LL_C1_IPCC_DisableReceiveChannel\n
  *         C1MR        CH4OM           LL_C1_IPCC_DisableReceiveChannel\n
  *         C1MR        CH5OM           LL_C1_IPCC_DisableReceiveChannel\n
  *         C1MR        CH6OM           LL_C1_IPCC_DisableReceiveChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_DisableReceiveChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  SET_BIT(IPCCx->C1MR, Channel);
}

/**
  * @brief  Mask receive channel occupied interrupt for processor 1.
  * @rmtoll C2MR        CH1OM           LL_C2_IPCC_DisableReceiveChannel\n
  *         C2MR        CH2OM           LL_C2_IPCC_DisableReceiveChannel\n
  *         C2MR        CH3OM           LL_C2_IPCC_DisableReceiveChannel\n
  *         C2MR        CH4OM           LL_C2_IPCC_DisableReceiveChannel\n
  *         C2MR        CH5OM           LL_C2_IPCC_DisableReceiveChannel\n
  *         C2MR        CH6OM           LL_C2_IPCC_DisableReceiveChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_DisableReceiveChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  SET_BIT(IPCCx->C2MR, Channel);
}

/**
  * @brief  Set Clock After Wake-Up From Stop mode
  * @rmtoll CFGR         STOPWUCK      LL_RCC_SetClkAfterWakeFromStop
  * @param  Clock This parameter can be one of the following values:
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_MSI
  *         @arg @ref LL_RCC_STOP_WAKEUPCLOCK_HSI
  * @retval None
  */
__STATIC_INLINE void LL_RCC_SetClkAfterWakeFromStop(uint32_t Clock)
{
  MODIFY_REG(RCC->CFGR, RCC_CFGR_STOPWUCK, Clock);
}

/**
  * @brief  Set Low-Power mode for CPU2
  * @rmtoll C2CR1        LPMS          LL_C2_PWR_SetPowerMode
  * @param  LowPowerMode This parameter can be one of the following values:
  *         @arg @ref LL_PWR_MODE_STOP0
  *         @arg @ref LL_PWR_MODE_STOP1
  *         @arg @ref LL_PWR_MODE_STOP2 (*)
  *         @arg @ref LL_PWR_MODE_STANDBY
  *         @arg @ref LL_PWR_MODE_SHUTDOWN
  *
  *         (*) Not available on devices STM32WB15xx, STM32WB10xx, STM32WB1Mxx
  * @retval None
  */
__STATIC_INLINE void LL_C2_PWR_SetPowerMode(uint32_t LowPowerMode)
{
  MODIFY_REG(PWR->C2CR1, PWR_C2CR1_LPMS, LowPowerMode);
}

/**
  * @brief  Boot CPU2 after reset or wakeup from stop or standby modes
  * @rmtoll CR4          C2BOOT        LL_PWR_EnableBootC2
  * @retval None
  */
__STATIC_INLINE void LL_PWR_EnableBootC2(void)
{
  SET_BIT(PWR->CR4, PWR_CR4_C2BOOT);
}

/**
  * @brief  Enable C2AHB3 peripherals clock.
  * @rmtoll C2AHB3ENR    PKAEN         LL_C2_AHB3_GRP1_EnableClock\n
  *         C2AHB3ENR    AES2EN        LL_C2_AHB3_GRP1_EnableClock\n
  *         C2AHB3ENR    RNGEN         LL_C2_AHB3_GRP1_EnableClock\n
  *         C2AHB3ENR    HSEMEN        LL_C2_AHB3_GRP1_EnableClock\n
  *         C2AHB3ENR    IPCCEN        LL_C2_AHB3_GRP1_EnableClock\n
  *         C2AHB3ENR    FLASHEN       LL_C2_AHB3_GRP1_EnableClock
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_PKA
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_AES2
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_RNG
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_HSEM
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_IPCC
  *         @arg @ref LL_C2_AHB3_GRP1_PERIPH_FLASH
  * @retval None
  */
__STATIC_INLINE void LL_C2_AHB3_GRP1_EnableClock(uint32_t Periphs)
{
  __IO uint32_t tmpreg;
  SET_BIT(RCC->C2AHB3ENR, Periphs);
  /* Delay after an RCC peripheral clock enabling */
  tmpreg = READ_BIT(RCC->C2AHB3ENR, Periphs);
  (void)tmpreg;
}

/**
  * @brief  Enable ExtiLine Rising Edge Trigger for Lines in range 32 to 63
  * @note The configurable wakeup lines are edge-triggered. No glitch must be
  *       generated on these lines. If a rising edge on a configurable interrupt
  *       line occurs during a write operation in the EXTI_RTSR register, the
  *       pending bit is not set.Rising and falling edge triggers can be set for
  *       the same interrupt line. In this case, both generate a trigger
  *       condition.
  * @rmtoll RTSR2        RTx           LL_EXTI_EnableRisingTrig_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_33 (*)
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  *         (*) value not defined in all devices
  * @retval None
  */
__STATIC_INLINE void LL_EXTI_EnableRisingTrig_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->RTSR2, ExtiLine);
}

/**
  * @brief  Enable ExtiLine Event request for Lines in range 32 to 63 for cpu2
  * @rmtoll C2EMR2         EMx           LL_C2_EXTI_EnableEvent_32_63
  * @param  ExtiLine This parameter can be a combination of the following values:
  *         @arg @ref LL_EXTI_LINE_40
  *         @arg @ref LL_EXTI_LINE_41
  * @retval None
  */
__STATIC_INLINE void LL_C2_EXTI_EnableEvent_32_63(uint32_t ExtiLine)
{
  SET_BIT(EXTI->C2EMR2, ExtiLine);
}

/**
  * @brief  Enable Transmit channel free interrupt for processor 1.
  * @rmtoll C1CR          TXFIE         LL_C1_IPCC_EnableIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_EnableIT_TXF(IPCC_TypeDef *IPCCx)
{
  SET_BIT(IPCCx->C1CR, IPCC_C1CR_TXFIE);
}

/**
  * @brief  Disable Transmit channel free interrupt for processor 1.
  * @rmtoll C1CR          TXFIE         LL_C1_IPCC_DisableIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_DisableIT_TXF(IPCC_TypeDef *IPCCx)
{
  CLEAR_BIT(IPCCx->C1CR, IPCC_C1CR_TXFIE);
}

/**
  * @brief  Check if Transmit channel free interrupt for processor 1 is enabled.
  * @rmtoll C1CR          TXFIE         LL_C1_IPCC_IsEnabledIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C1_IPCC_IsEnabledIT_TXF(IPCC_TypeDef const *const IPCCx)
{
  return ((READ_BIT(IPCCx->C1CR, IPCC_C1CR_TXFIE) == (IPCC_C1CR_TXFIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Receive channel occupied interrupt for processor 1.
  * @rmtoll C1CR          RXOIE         LL_C1_IPCC_EnableIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_EnableIT_RXO(IPCC_TypeDef *IPCCx)
{
  SET_BIT(IPCCx->C1CR, IPCC_C1CR_RXOIE);
}

/**
  * @brief  Disable Receive channel occupied interrupt for processor 1.
  * @rmtoll C1CR          RXOIE         LL_C1_IPCC_DisableIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_DisableIT_RXO(IPCC_TypeDef *IPCCx)
{
  CLEAR_BIT(IPCCx->C1CR, IPCC_C1CR_RXOIE);
}

/**
  * @brief  Check if Receive channel occupied interrupt for processor 1 is enabled.
  * @rmtoll C1CR          RXOIE         LL_C1_IPCC_IsEnabledIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C1_IPCC_IsEnabledIT_RXO(IPCC_TypeDef  const *const IPCCx)
{
  return ((READ_BIT(IPCCx->C1CR, IPCC_C1CR_RXOIE) == (IPCC_C1CR_RXOIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Transmit channel free interrupt for processor 2.
  * @rmtoll C2CR          TXFIE         LL_C2_IPCC_EnableIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_EnableIT_TXF(IPCC_TypeDef *IPCCx)
{
  SET_BIT(IPCCx->C2CR, IPCC_C2CR_TXFIE);
}

/**
  * @brief  Disable Transmit channel free interrupt for processor 2.
  * @rmtoll C2CR          TXFIE         LL_C2_IPCC_DisableIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_DisableIT_TXF(IPCC_TypeDef *IPCCx)
{
  CLEAR_BIT(IPCCx->C2CR, IPCC_C2CR_TXFIE);
}

/**
  * @brief  Check if Transmit channel free interrupt for processor 2 is enabled.
  * @rmtoll C2CR          TXFIE         LL_C2_IPCC_IsEnabledIT_TXF
  * @param  IPCCx IPCC Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_IPCC_IsEnabledIT_TXF(IPCC_TypeDef  const *const IPCCx)
{
  return ((READ_BIT(IPCCx->C2CR, IPCC_C2CR_TXFIE) == (IPCC_C2CR_TXFIE)) ? 1UL : 0UL);
}

/**
  * @brief  Enable Receive channel occupied interrupt for processor 2.
  * @rmtoll C2CR          RXOIE         LL_C2_IPCC_EnableIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_EnableIT_RXO(IPCC_TypeDef *IPCCx)
{
  SET_BIT(IPCCx->C2CR, IPCC_C2CR_RXOIE);
}

/**
  * @brief  Disable Receive channel occupied interrupt for processor 2.
  * @rmtoll C2CR          RXOIE         LL_C2_IPCC_DisableIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval None
  */
__STATIC_INLINE void LL_C2_IPCC_DisableIT_RXO(IPCC_TypeDef *IPCCx)
{
  CLEAR_BIT(IPCCx->C2CR, IPCC_C2CR_RXOIE);
}

/**
  * @brief  Check if Receive channel occupied interrupt for processor 2 is enabled.
  * @rmtoll C2CR          RXOIE         LL_C2_IPCC_IsEnabledIT_RXO
  * @param  IPCCx IPCC Instance.
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_IPCC_IsEnabledIT_RXO(IPCC_TypeDef const *const IPCCx)
{
  return ((READ_BIT(IPCCx->C2CR, IPCC_C2CR_RXOIE) == (IPCC_C2CR_RXOIE)) ? 1UL : 0UL);
}

/**
  * @brief  Set IPCC transmit channel status for processor 1.
  * @note   Associated with IPCC_C1TOC2SR.CHxF
  * @rmtoll C1SCR        CH1S           LL_C1_IPCC_SetFlag_CHx\n
  *         C1SCR        CH2S           LL_C1_IPCC_SetFlag_CHx\n
  *         C1SCR        CH3S           LL_C1_IPCC_SetFlag_CHx\n
  *         C1SCR        CH4S           LL_C1_IPCC_SetFlag_CHx\n
  *         C1SCR        CH5S           LL_C1_IPCC_SetFlag_CHx\n
  *         C1SCR        CH6S           LL_C1_IPCC_SetFlag_CHx
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_SetFlag_CHx(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  WRITE_REG(IPCCx->C1SCR, Channel << IPCC_C1SCR_CH1S_Pos);
}

/**
  * @brief  Return the device revision identifier
  * @note   This field indicates the revision of the device.
  * @rmtoll DBGMCU_IDCODE REV_ID        LL_DBGMCU_GetRevisionID
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetRevisionID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID) >> DBGMCU_IDCODE_REV_ID_Pos);
}

/**
  * @brief  Return the device identifier
  * @note   For STM32WBxxxx devices, the device ID is 0x495
  * @rmtoll DBGMCU_IDCODE DEV_ID        LL_DBGMCU_GetDeviceID
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFF (ex: device ID is 0x495)
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetDeviceID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID));
}

/**
  * @brief  Unmask transmit channel free interrupt for processor 1.
  * @rmtoll C1MR        CH1FM           LL_C1_IPCC_EnableTransmitChannel\n
  *         C1MR        CH2FM           LL_C1_IPCC_EnableTransmitChannel\n
  *         C1MR        CH3FM           LL_C1_IPCC_EnableTransmitChannel\n
  *         C1MR        CH4FM           LL_C1_IPCC_EnableTransmitChannel\n
  *         C1MR        CH5FM           LL_C1_IPCC_EnableTransmitChannel\n
  *         C1MR        CH6FM           LL_C1_IPCC_EnableTransmitChannel
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be a combination of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval None
  */
__STATIC_INLINE void LL_C1_IPCC_EnableTransmitChannel(IPCC_TypeDef *IPCCx, uint32_t Channel)
{
  CLEAR_BIT(IPCCx->C1MR, Channel << IPCC_C1MR_CH1FM_Pos);
}

/**
  * @brief  Get channel status for processor 2.
  * @rmtoll C2TOC1SR        CH1F           LL_C2_IPCC_IsActiveFlag_CHx\n
  *         C2TOC1SR        CH2F           LL_C2_IPCC_IsActiveFlag_CHx\n
  *         C2TOC1SR        CH3F           LL_C2_IPCC_IsActiveFlag_CHx\n
  *         C2TOC1SR        CH4F           LL_C2_IPCC_IsActiveFlag_CHx\n
  *         C2TOC1SR        CH5F           LL_C2_IPCC_IsActiveFlag_CHx\n
  *         C2TOC1SR        CH6F           LL_C2_IPCC_IsActiveFlag_CHx
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C2_IPCC_IsActiveFlag_CHx(IPCC_TypeDef  const *const IPCCx, uint32_t Channel)
{
  return ((READ_BIT(IPCCx->C2TOC1SR, Channel) == (Channel)) ? 1UL : 0UL);
}

/**
  * @brief  Get channel status for processor 1.
  * @rmtoll C1TOC2SR        CH1F           LL_C1_IPCC_IsActiveFlag_CHx\n
  *         C1TOC2SR        CH2F           LL_C1_IPCC_IsActiveFlag_CHx\n
  *         C1TOC2SR        CH3F           LL_C1_IPCC_IsActiveFlag_CHx\n
  *         C1TOC2SR        CH4F           LL_C1_IPCC_IsActiveFlag_CHx\n
  *         C1TOC2SR        CH5F           LL_C1_IPCC_IsActiveFlag_CHx\n
  *         C1TOC2SR        CH6F           LL_C1_IPCC_IsActiveFlag_CHx
  * @param  IPCCx IPCC Instance.
  * @param  Channel This parameter can be one of the following values:
  *         @arg @ref LL_IPCC_CHANNEL_1
  *         @arg @ref LL_IPCC_CHANNEL_2
  *         @arg @ref LL_IPCC_CHANNEL_3
  *         @arg @ref LL_IPCC_CHANNEL_4
  *         @arg @ref LL_IPCC_CHANNEL_5
  *         @arg @ref LL_IPCC_CHANNEL_6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_C1_IPCC_IsActiveFlag_CHx(IPCC_TypeDef  const *const IPCCx, uint32_t Channel)
{
  return ((READ_BIT(IPCCx->C1TOC2SR, Channel) == (Channel)) ? 1UL : 0UL);
}


#define __HAL_RCC_IPCC_CLK_ENABLE()            LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_IPCC)
#define __HAL_RCC_IPCC_CLK_DISABLE()           LL_AHB3_GRP1_DisableClock(LL_AHB3_GRP1_PERIPH_IPCC)


#endif /* OS_HAL_PORTS_STM32_LLD_IPCCV1_IPCCV1_H_ */
