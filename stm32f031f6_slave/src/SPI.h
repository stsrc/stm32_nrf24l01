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

#ifndef _SPI_H_
#define _SPI_H_

#include <stdlib.h>
#include <string.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_rcc.h>
#include <stm32f0xx_hal_dma.h>
#include <stm32f0xx_hal_spi.h>
#include <stm32f0xx_hal_cortex.h>

/**
 * @addtogroup RFID_System libraries
 * @{
 */

/**
 * @defgroup SPI
 * @brief SPI library.
 * @{
 *
 * SPI library, which is based on HAL SPI library.
 */

/**
 * @brief Function presents that SPI result failed, by switching on LEDs.
 * @{
 * If error occures, execution of program is stoped.
 */
void SPI_show_error(HAL_StatusTypeDef rt);

HAL_StatusTypeDef SPI_init(SPI_HandleTypeDef *spi_handler, SPI_TypeDef *SPI);
HAL_StatusTypeDef SPI_send_multiple(SPI_HandleTypeDef *spi_handler,
				    uint8_t *data, uint8_t size);
HAL_StatusTypeDef SPI_send(SPI_HandleTypeDef *spi_handler, uint8_t *data);
HAL_StatusTypeDef SPI_send_and_receive(SPI_HandleTypeDef *spi_handler,
				       uint8_t data, uint8_t *receiveData);
HAL_StatusTypeDef SPI_read(SPI_HandleTypeDef *spi_handler, uint8_t *data,
			   uint16_t bytes);

#endif

/* TODO unification ! */
