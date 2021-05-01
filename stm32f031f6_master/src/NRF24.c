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

#include "NRF24.h"
#include "UART.h"
#include <stdio.h>
#include "stupid_delay.h"

void EXTI4_15_IRQHandler(void)
{
}

static HAL_StatusTypeDef NRF24_reg_read(struct nrf24 *nrf,
					uint8_t reg,
					uint8_t *data,
					uint8_t dataSize)
{
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);
	ret = SPI_send(&(nrf->spi_handler), &reg);
	if (ret)
		return ret;
	ret = SPI_read(&(nrf->spi_handler), data, dataSize);
	if (ret)
		return ret;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	return ret;
}

static HAL_StatusTypeDef NRF24_reg_write(struct nrf24 *nrf,
					 uint8_t reg,
					 uint8_t *data,
					 uint8_t dataSize)
{
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);
	uint8_t value = 0b00100000 | reg;
	ret = SPI_send(&(nrf->spi_handler), &value);
	if (ret) return ret;
	ret = SPI_send(&(nrf->spi_handler), data);
	if (ret) return ret;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	return ret;
}

void NRF24_init(struct nrf24 *nrf)
{
	SPI_init(&(nrf->spi_handler), nrf->spi_instance);

	GPIO_InitTypeDef gpio =
	{
		nrf->csn,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(nrf->gpio, &gpio);

	gpio.Pin = nrf->ce;
	HAL_GPIO_Init(nrf->gpio, &gpio);

	if (nrf->irq) {
		gpio.Pin = nrf->irq;
		gpio.Mode = GPIO_MODE_IT_FALLING;
		gpio.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(nrf->gpio, &gpio);
		if (nrf->irq == GPIO_PIN_4) {
//			HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
		}
	}

	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	delay_ms(100);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_SET); //CE
	delay_ms(100);
}

void NRF24_init_receiver(struct nrf24 *nrf)
{
	uint8_t value = 0x0b;
	NRF24_reg_write(nrf, 0x00, &value, 1);

	//set packet length to 1
	value = 1;
	NRF24_reg_write(nrf, 0x11, &value, 1);

	//set data rate to 250kbps and RF output power to 0dBm
	value = 0b00100110;
	NRF24_reg_write(nrf, 0x06, &value, 1);
}

void NRF24_receiver_receive(struct nrf24 *nrf, uint8_t *buffer, size_t bufferSize)
{
	uint8_t status;
	NRF24_reg_read(nrf, 0x07, &status, 1);
	if (status & 0b01000000) {
		// got packet - check its size
		uint8_t readVal;
		uint8_t it = 0;
		uint8_t sizeOfMessage;
		uint8_t registerValue;
packet:
		NRF24_reg_read(nrf, 0x11, &sizeOfMessage, 1);

		if (it + sizeOfMessage > bufferSize)
			return; //TODO: error logging

		// read packet
		readVal = 0b01100001;
		HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);
		SPI_send(&(nrf->spi_handler), &readVal);
		SPI_read(&(nrf->spi_handler), &buffer[it], sizeOfMessage);
		it += sizeOfMessage;
		HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET);

		//clear irq

		NRF24_reg_read(nrf, 0x07, &registerValue, 1);
		registerValue |= 0b01000000;
		NRF24_reg_write(nrf, 0x07, &registerValue, 1);

		//check if there is data
		NRF24_reg_read(nrf, 0x17, &registerValue, 1);
		if (!(registerValue & 0b00000001)) {
			goto packet;
		}
	}
}
