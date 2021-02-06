#include <stm32f0xx.h>
#include <core_cm0.h>
#include <stm32f0xx_hal_gpio.h>
//#include "GPS.h"
#include <string.h>
#include "NRF24.h"
#include "stupid_delay.h"

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

	GPIO_InitTypeDef gpio = {
		GPIO_PIN_1,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOB, &gpio);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

	while(1){
		NRF24_transmitter_send(&nrf, (uint8_t *) "this is my position", 3);
		delay_ms(100);
	};
}

EXTI4_15_IRQHandler(void)
{
	while(1);
}

