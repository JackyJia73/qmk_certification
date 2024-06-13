#include "hal.h"

#include "ipccv1.h"


//#if HAL_USE_IPCC || defined(__DOXYGEN__)
#define IPCC_ALL_RX_BUF 0x0000003FU /*!< Mask for all RX buffers. */
#define IPCC_ALL_TX_BUF 0x003F0000U /*!< Mask for all TX buffers. */
#define CHANNEL_INDEX_MASK 0x0000000FU /*!< Mask the channel index to avoid overflow */

/**
  * @brief  Enable HSI48
  * @rmtoll CRRCR          HSI48ON       LL_RCC_HSI48_Enable
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI48_Enable(void)
{
  SET_BIT(RCC->CRRCR, RCC_CRRCR_HSI48ON);
}

/**
  * @brief  Check if HSI48 oscillator Ready
  * @rmtoll CRRCR          HSI48RDY      LL_RCC_HSI48_IsReady
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_RCC_HSI48_IsReady(void)
{
  return ((READ_BIT(RCC->CRRCR, RCC_CRRCR_HSI48RDY) == (RCC_CRRCR_HSI48RDY)) ? 1UL : 0UL);
}

/**
  * @brief Enable VDDUSB supply.
  * @note  Remove VDDUSB electrical and logical isolation, once VDDUSB supply is present.
  * @retval None
  */
void HAL_PWREx_EnableVddUSB(void)
{
  SET_BIT(PWR->CR2, PWR_CR2_USV);
}

/**
  * @brief  Enable HSI Automatic from stop mode
  * @rmtoll CR           HSIASFS       LL_RCC_HSI_EnableAutoFromStop
  * @retval None
  */
__STATIC_INLINE void LL_RCC_HSI_EnableAutoFromStop(void)
{
  SET_BIT(RCC->CR, RCC_CR_HSIASFS);
}

#define HAL_USE_IPCC                        TRUE
#define STM32_IPCC_C1_RX_HANDLER            VectorF0
#define STM32_IPCC_C1_TX_HANDLER            VectorF4
#define STM32_IPCC_C1_RX_NUMBER             44
#define STM32_IPCC_C1_TX_NUMBER             45
#define STM32_IPCC_C1_RX_IRQ_PRIORITY       10 /*0*/
#define STM32_IPCC_C1_TX_IRQ_PRIORITY       10 /*1*/

void ipcc_lld_init(void){
	/* RFWKP */
	MODIFY_REG(RCC->CSR, (3 << 14), (3 << 14)); /* LSE used as RF system wakeup clock. */

	/* Peripheral clock enable */
	__HAL_RCC_IPCC_CLK_ENABLE();

	/* IPCC interrupt Init */
	nvicEnableVector(STM32_IPCC_C1_RX_NUMBER, STM32_IPCC_C1_RX_IRQ_PRIORITY);
	nvicEnableVector(STM32_IPCC_C1_TX_NUMBER, STM32_IPCC_C1_TX_IRQ_PRIORITY);

	/* Keeping this sequence for legacy: */
	LL_RCC_HSI48_Enable();
	while (!LL_RCC_HSI48_IsReady()) {
	}
}

void ipcc_lld_deninit(void){
	/* Peripheral clock disable */
	__HAL_RCC_IPCC_CLK_DISABLE();

	/* IPCC interrupt DeInit */
	nvicDisableVector(STM32_IPCC_C1_RX_NUMBER);
	nvicDisableVector(STM32_IPCC_C1_TX_NUMBER);
}

void ipcc_lld_reset(void){
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_IPCC);

  LL_C1_IPCC_ClearFlag_CHx(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

  LL_C2_IPCC_ClearFlag_CHx(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

  LL_C1_IPCC_DisableTransmitChannel(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

  LL_C2_IPCC_DisableTransmitChannel(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

  LL_C1_IPCC_DisableReceiveChannel(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);

  LL_C2_IPCC_DisableReceiveChannel(
	  IPCC,
	  LL_IPCC_CHANNEL_1 | LL_IPCC_CHANNEL_2 | LL_IPCC_CHANNEL_3 | LL_IPCC_CHANNEL_4
	  | LL_IPCC_CHANNEL_5 | LL_IPCC_CHANNEL_6);
}

void SystemPower_Config(void){
	/*
	 * System Power Configuration
	 */
	// Before going to stop or standby modes, do the settings so that system clock and IP80215.4 clock
	// start on HSI automatically
	LL_RCC_HSI_EnableAutoFromStop();

	/**
	* Select HSI as system clock source after Wake Up from Stop mode
	*/
	LL_RCC_SetClkAfterWakeFromStop(LL_RCC_STOP_WAKEUPCLOCK_HSI);

	/* Initialize the CPU2 reset value before starting CPU2 with C2BOOT */
	LL_C2_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);

#if (HAL_USE_USB == TRUE)
	HAL_PWREx_EnableVddUSB();
#endif /* HAL_USE_USB */
}

void ipcc_lld_start(void){
	/* Reset all registers of the current cpu to default state */
//	IPCC_Reset_Register(IPCC_C1);
	/* Disable RX and TX interrupts */
	IPCC_C1->CR  = 0x00000000U;

	/* Mask RX and TX interrupts */
	IPCC_C1->MR  = (IPCC_ALL_TX_BUF | IPCC_ALL_RX_BUF);

	/* Clear RX status */
	IPCC_C1->SCR = IPCC_ALL_RX_BUF;

	/* Activate the interrupts */
	IPCC_C1->CR |= (IPCC_CR_RXOIE | IPCC_CR_TXFIE);
}

//#endif
