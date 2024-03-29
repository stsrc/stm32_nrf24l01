#ifndef _UART_HPP_
#define _UART_HPP_

#include <stm32f0xx_hal_def.h>
#include <stm32f0xx_hal_dma.h>
#include <stm32f0xx_hal_rcc.h>
#include "simple_buffer.h"

/**
 * @addtogroup RFID_System libraries
 * @{
 */

/**
 * @defgroup UART
 * @brief UART library.
 * @{
 */

HAL_StatusTypeDef UART_1_init();

void UART_1_transmit();

extern struct simple_buffer UART1_transmit_buffer;

#endif
