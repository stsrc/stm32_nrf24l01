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

/*	GPIO_InitTypeDef gpioo = {
		GPIO_PIN_2 | GPIO_PIN_3,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOA, &gpioo);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);*/

	while(1){
//		uint8_t buffer[16];
//		NRF24_transmitter_send(&nrf, (uint8_t *) "this is my position", 3);
//		UART_1_transmit(0xA6);
//		memset(buffer, 0, 16);
//		UART_1_receive(buffer, 16);
		delay_ms(100);
	};
}


