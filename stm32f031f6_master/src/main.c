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
#include <string.h>
#include "NRF24.h"
#include "stupid_delay.h"
#include <stm32f0xx_hal.h>
#include "UART.h"

void clock_init(void)
{
	//HSE turn on
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));

	// PLL input clock x6
	RCC->CFGR &= ~(0xF << 18);
	RCC->CFGR |= RCC_CFGR_PLLMUL6;

	// PLL source - HSE/PREDIV
	RCC->CFGR &= ~(0x3 << 15);
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;

	//HSE divider for PLL input clock
	RCC->CFGR &= ~(1 << 17);

	//PREDIV input clock - no prediv
	RCC->CFGR2 &= ~(0b1111);

	//PLL turn on
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	// Use PLL
	RCC->CFGR &= ~(0b11);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));

	SystemCoreClock = 48000000;
}

int main(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_Init();

	clock_init();

	delay_init();
	UART_1_init();

	GPIO_InitTypeDef gpio = {
		GPIO_PIN_1,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOB, &gpio);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
	delay_ms(1000);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	delay_ms(1);

	struct nrf24 nrf = {
		.spi_handler = { 0 },
		.spi_instance = SPI1,
		.csn = GPIO_PIN_0,
		.ce = GPIO_PIN_1,
		.irq = GPIO_PIN_4,
		.gpio = GPIOA
	};
	NRF24_init(&nrf);
	NRF24_init_receiver(&nrf);
	delay_ms(2);

	while(1){
		uint8_t buffer[512];
		memset(buffer, 0, sizeof(buffer));
		NRF24_receiver_receive(&nrf, buffer, sizeof(buffer));
		size_t stringLength = strlen((char *) buffer);
		if (stringLength) {
			buffer_set_text(&UART1_transmit_buffer, (const char *) buffer, stringLength);
			UART_1_transmit();
		}
	}

	return 0;
}

