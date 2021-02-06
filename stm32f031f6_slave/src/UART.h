/*
 * where-is-catiusha - cat localisation project
 * Copyright (C) 2021 Konrad Gotfryd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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

void UART_1_transmit(uint8_t value);
void UART_1_receive(uint8_t *buffer, size_t bufferSize);

extern struct simple_buffer UART1_transmit_buffer;
extern struct simple_buffer UART1_receive_buffer;

#endif
