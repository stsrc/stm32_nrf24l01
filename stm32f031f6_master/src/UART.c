#include "UART.h"
#include <stm32f0xx_hal_uart.h>
#include <stm32f0xx_hal_gpio.h>
#include <stm32f0xx_hal_cortex.h>

static UART_HandleTypeDef uart_1_handler;

struct simple_buffer UART1_transmit_buffer;
struct simple_buffer UART1_receive_buffer;

void USART1_IRQHandler(void)
{
	uint8_t ret;
	uint8_t data;
	if (USART1->ISR & (USART_ISR_TC | USART_ISR_TXE)) {
		ret = buffer_get_byte(&UART1_transmit_buffer, &data);
		if (!ret)
			USART1->TDR = data;
		else
			USART1->CR1 &= ~USART_CR1_TXEIE;
	}
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

	memset(&UART1_transmit_buffer, 0, sizeof(struct simple_buffer));

	USART1->CR1 = 0;
	ret = HAL_UART_Init(&uart_1_handler);
	//USART1->CR1 |= USART_CR1_TCIE;
	USART1->CR1 |= USART_CR1_TXEIE;

	buffer_init(&UART1_transmit_buffer);

	return ret;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef init_gpio;

	init_gpio.Pin = GPIO_PIN_2;

	init_gpio.Pull = GPIO_NOPULL;
	init_gpio.Mode = GPIO_MODE_AF_PP;
	init_gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	init_gpio.Alternate = GPIO_AF1_USART1;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_Init(GPIOA, &init_gpio);

	init_gpio.Pin = GPIO_PIN_3;

	init_gpio.Pull = GPIO_PULLDOWN;
	init_gpio.Mode = GPIO_MODE_AF_OD;
	init_gpio.Alternate = GPIO_AF1_USART1;

	HAL_GPIO_Init(GPIOA, &init_gpio);

	__HAL_RCC_USART1_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void UART_1_transmit(void)
{
	USART1->CR1 |= USART_CR1_TXEIE;
}
