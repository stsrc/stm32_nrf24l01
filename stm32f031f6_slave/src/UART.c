#include "UART.h"
#include <stm32f0xx_hal_uart.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_cortex.h>

static UART_HandleTypeDef uart_1_handler;

struct simple_buffer UART1_transmit_buffer;
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

	ret = HAL_UART_Init(&uart_1_handler);

	USART1->CR1 |= USART_CR1_RXNEIE;

	return ret;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef init_gpio;

	init_gpio.Pin = GPIO_PIN_2; //txd
	init_gpio.Pull = GPIO_NOPULL;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	init_gpio.Pin = GPIO_PIN_3; //rxd
	init_gpio.Pull = GPIO_PULLDOWN;
	init_gpio.Mode = GPIO_MODE_AF_OD;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	HAL_NVIC_EnableIRQ(USART1_IRQn);

}

void UART_1_transmit(uint8_t value)
{
	HAL_UART_Transmit(&uart_1_handler, &value, 1, 0);
}

void UART_1_receive(uint8_t *buffer, size_t bufferSize)
{
	HAL_UART_Receive_IT(&uart_1_handler, buffer, bufferSize);
}
