/*
 * hal_ipcc_lld.h
 *
 *  Created on: Aug 17, 2023
 *      Author: yuyw
 */

#ifndef LIB_CHIBIOS_OS_HAL_PORTS_STM32_LLD_IPCCV1_HAL_IPCC_LLD_H_
#define LIB_CHIBIOS_OS_HAL_PORTS_STM32_LLD_IPCCV1_HAL_IPCC_LLD_H_


void ipcc_lld_init(void);
void ipcc_lld_deninit(void);
void ipcc_lld_reset(void);
void ipcc_lld_start(void);


#endif /* LIB_CHIBIOS_OS_HAL_PORTS_STM32_LLD_IPCCV1_HAL_IPCC_LLD_H_ */
