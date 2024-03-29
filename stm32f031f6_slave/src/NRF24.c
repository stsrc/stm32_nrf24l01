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
//	while(1);
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

//	if (nrf->irq) {
//		gpio.Pin = nrf->irq;
//		gpio.Mode = GPIO_MODE_IT_FALLING;
//		gpio.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(nrf->gpio, &gpio);
//		if (nrf->irq == GPIO_PIN_4) {
//			HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
//		}
//	}

	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	delay_ms(100);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_RESET); //CE
	delay_ms(100);
}

void NRF24_init_transmitter(struct nrf24 *nrf)
{
	uint8_t value = 0x0a;
	uint8_t valueRead = 0;
	NRF24_reg_read(nrf, 0x00, &valueRead, 1);
	if (valueRead != 0x08)
		while(1);
	NRF24_reg_write(nrf, 0x00, &value, 1);
	NRF24_reg_read(nrf, 0x00, &valueRead, 1);
	if (value != valueRead)
		while(1);

	value = 0x01;
	NRF24_reg_write(nrf, 0x1d, &value, 1);
	NRF24_reg_read(nrf, 0x1d, &valueRead, 1);
	if (value != valueRead)
		while(1);

	//set data rate to 250kbps and RF output power to 0dBm
	value = 0b00100110;
	NRF24_reg_write(nrf, 0x06, &value, 1);
}

void NRF24_transmitter_send(struct nrf24 *nrf, uint8_t *data, uint8_t size)
{
	if (size > 32)
		return;

//	uint8_t value = 0b10100000;
	uint8_t value = 0b10110000;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);

	HAL_StatusTypeDef ret = SPI_send(&(nrf->spi_handler), &value);
	if (ret)
		goto end;

	SPI_send_multiple(&(nrf->spi_handler), data, size);
	delay_ms(10);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_SET); //CE
	delay_ms(10);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_RESET); //CE

end:
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET);
}

