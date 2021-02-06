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

#include <stm32f0xx.h>
#include <core_cm0.h>
#include <stm32f0xx_hal_gpio.h>
#include "GPS.h"
#include <string.h>
#include "NRF24.h"
#include "stupid_delay.h"

#include "UART.h"

int main(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	struct nrf24 nrf = {
		.spi_handler = { 0 },
		.spi_instance = SPI1,
		.csn = GPIO_PIN_0,
		.ce = GPIO_PIN_1,
		.irq = GPIO_PIN_4,
		.gpio = GPIOA
	};

	delay_init();
	NRF24_init(&nrf);
	NRF24_init_transmitter(&nrf);
	gps_init();

	GPIO_InitTypeDef gpio = {
		GPIO_PIN_1,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOB, &gpio);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

	while(1){
		char buffer[512];
		memset(buffer, 0, sizeof(buffer));
		gps_get_data(buffer, sizeof(buffer));
		NRF24_transmitter_send(&nrf, (uint8_t *) buffer, strlen(buffer));
		delay_ms(20);
	};
}


