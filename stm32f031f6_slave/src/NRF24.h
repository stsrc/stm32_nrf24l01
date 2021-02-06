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

#ifndef _NRF24_H_
#define _NRF24_H_
#include "SPI.h"

struct nrf24 {
	SPI_HandleTypeDef spi_handler;
	SPI_TypeDef *spi_instance;
	uint32_t csn;
	uint32_t ce;
	uint32_t irq;
	GPIO_TypeDef *gpio;
};

void NRF24_init(struct nrf24 *nrf);
void NRF24_init_transmitter(struct nrf24 *nrf);
//void NRF24_init_receiver(struct nrf24 *nrf);
//void NRF24_test(struct nrf24 *nrf);
void NRF24_transmitter_send(struct nrf24 *nrf, uint8_t *data, uint8_t size);
//void NRF24_receiver_receive(struct nrf24 *nrf, uint8_t *buffer, uint8_t bufferSize);
#endif
