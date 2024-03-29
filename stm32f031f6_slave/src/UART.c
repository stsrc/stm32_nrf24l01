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

#include "UART.h"
#include <stm32f0xx_hal_uart.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_cortex.h>

struct simple_buffer UART1_receive_buffer;

void USART1_IRQHandler(void)
{
	uint8_t data;
	if (USART1->ISR & USART_ISR_RXNE) {
		data = (uint8_t)USART1->RDR;
		buffer_set_byte(&UART1_receive_buffer, data);
	}

	if (USART1->ISR & USART_ISR_ORE) {
		USART1->ICR |= USART_ICR_ORECF;
	}

	return;
}

HAL_StatusTypeDef UART_1_init()
{
	__HAL_RCC_USART1_CLK_ENABLE();

	buffer_init(&UART1_receive_buffer);

	GPIO_InitTypeDef init_gpio;

	init_gpio.Pin = GPIO_PIN_2; //txd
	init_gpio.Pull = GPIO_NOPULL;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	init_gpio.Pin = GPIO_PIN_3; //rxd
	init_gpio.Pull = GPIO_NOPULL;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	// UARTDIV = fck / baud
	//USART1->BRR = 833; //When 8MHz clock
	//USART1->BRR = 5000; //When 48MHz clock
	USART1->BRR = SystemCoreClock / 9600;
	USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

	return 0;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef init_gpio;

	init_gpio.Pin = GPIO_PIN_2; //txd
	init_gpio.Pull = GPIO_PULLUP;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	init_gpio.Pin = GPIO_PIN_3; //rxd
	init_gpio.Pull = GPIO_PULLUP;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void UART_1_transmit(uint8_t value)
{
	USART1->TDR = value;
}
