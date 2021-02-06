#include "UART.h"
#include <stm32f0xx_hal_uart.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_cortex.h>

static UART_HandleTypeDef uart_1_handler;

struct simple_buffer UART1_transmit_buffer;
struct simple_buffer UART1_receive_buffer;

void USART1_IRQHandler(void)
{
/*	if (USART1->SR & (USART_SR_ORE | USART_SR_RXNE)) {
		data = (uint8_t)USART1->DR;
		buffer_set_byte(&UART1_receive_buffer, data);
	}

	if (USART1->SR & (USART_SR_TC | USART_SR_TXE)) {
		ret = buffer_get_byte(&UART1_transmit_buffer, &data);
		if (!ret)
			USART1->DR = data;
		else
			USART1->CR1 &= ~USART_CR1_TXEIE;
		USART1->SR &= ~USART_SR_TC;
	}*/
}

HAL_StatusTypeDef UART_1_init()
{
	HAL_StatusTypeDef ret;
	UART_InitTypeDef init;
	init.BaudRate = 9600;
	init.WordLength = UART_WORDLENGTH_8B;
	init.StopBits = UART_STOPBITS_1;
	init.Parity = UART_PARITY_NONE;
	init.Mode = UART_MODE_TX_RX;
	init.HwFlowCtl = UART_HWCONTROL_NONE;
	init.OverSampling = UART_OVERSAMPLING_16;

	uart_1_handler.Init = init;
	uart_1_handler.Instance = USART1;

	memset(&UART1_receive_buffer, 0, sizeof(struct simple_buffer));
	memset(&UART1_transmit_buffer, 0, sizeof(struct simple_buffer));

	USART1->CR1 = 0;
	ret = HAL_UART_Init(&uart_1_handler);
	USART1->CR1 |= USART_CR1_RXNEIE;
	USART1->CR1 |= USART_CR1_TCIE;
	USART1->CR1 |= USART_CR1_TXEIE;
	return ret;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef init_gpio;

	if (huart->Instance == USART1)
		init_gpio.Pin = GPIO_PIN_9;

	init_gpio.Pull = GPIO_NOPULL;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Speed = GPIO_SPEED_FREQ_HIGH;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_Init(GPIOA, &init_gpio);

	init_gpio.Pin = GPIO_PIN_10;

	init_gpio.Pull = GPIO_PULLDOWN;
	init_gpio.Mode = GPIO_MODE_AF_OD;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	if (huart->Instance == USART1) {
		__HAL_RCC_USART1_CLK_ENABLE();
		HAL_NVIC_EnableIRQ(USART1_IRQn);

	}
}

void UART_1_transmit(void)
{
	USART1->CR1 |= USART_CR1_TXEIE;
}
