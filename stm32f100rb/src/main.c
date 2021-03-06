#include <stm32f1xx.h>
#include <core_cm3.h>
#include "stupid_delay.h"
#include "UART.h"
#include "simple_buffer.h"

#include "GPS.h"
#include "NRF24.h"

#define LED_port GPIOC
#define LED_Blue (1 << 8)
#define GPIO_setBit(PORT, PIN) (PORT->BSRR |= PIN)
#define GPIO_clearBit(PORT, PIN) (PORT->BSRR |= (PIN << 0x10))

static void init_blue_led() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	LED_port->CRH |= GPIO_CRH_MODE8_0;
	LED_port->CRH &= ~GPIO_CRH_CNF8;
}

static void configure_clock() {
	RCC->CR |= (1 << 16); //External high speed clock enabled;
	while(!(RCC->CR & (1 << 17))); //External high-speed clock ready flag check

	RCC->CFGR |= 1 << 17; //LSB of division factor PREDIV1
	RCC->CFGR |= 1 << 16; //PLL entry clock source => clock from prediv1
	RCC->CFGR |= (0b0100 << 18); //PLL multiplication factor => x6

	RCC->CR |= (1 << 24); //PLL enabled;
	while(!(RCC->CR & (1 << 25))); //PLL clock ready flag

	RCC->CR &= ~(1 << 19); //Clock security system disabled
	RCC->CR &= ~(1 << 18); //External high-speed clock not bypassed

	RCC->CR |= 1 << 0; //Internal high-speed clock enabled
	while(!(RCC->CR & (1 << 1))); //Internal high-speed clock ready flag

	RCC->CFGR &= ~0b11; //select PLL as input
	RCC->CFGR |= (0b10);
	while(!(RCC->CFGR & 0b1000)); //wait until PLL is selected

	SystemCoreClock = 24000000;
}

extern struct simple_buffer UART2_transmit_buffer;
volatile static int irq_flag = 0;
int main(void){
	uint8_t counter = 0;
	struct nrf24 nrf_transmitter = {
		.spi_handler = { 0 },
		.spi_instance = SPI1,
		.gpio = GPIOA,
		.ce = GPIO_PIN_3,
		.csn = GPIO_PIN_4,
		.irq = 0
	};

	struct nrf24 nrf_receiver = {
		.spi_handler = { 0 },
		.spi_instance = SPI2,
		.gpio = GPIOB,
		.ce = GPIO_PIN_10,
		.csn = GPIO_PIN_11,
		.irq = GPIO_PIN_12
	};

	configure_clock();

	init_blue_led();
	delay_init();
	UART_1_init();

	NRF24_init(&nrf_transmitter);
	NRF24_init(&nrf_receiver);

	// two first reads always fail, I don't know why
	delay_ms(1000);
	NRF24_test(&nrf_transmitter);
	delay_ms(1000);
	NRF24_test(&nrf_receiver);
	delay_ms(1000);

	NRF24_init_transmitter(&nrf_transmitter);
	NRF24_init_receiver(&nrf_receiver);

      while(1){
		counter = (counter + 1) % 20;
		char buffer = 'A' + counter;
		char resultBuffer[3] = { 0 };
		NRF24_transmitter_send(&nrf_transmitter, (uint8_t *)&buffer, 1);
		if (irq_flag) {
			irq_flag = 0;
			NRF24_receiver_receive(&nrf_receiver, (uint8_t *)resultBuffer, sizeof(resultBuffer));
			buffer_set_text(&UART1_transmit_buffer, resultBuffer, sizeof(resultBuffer));
			UART_1_transmit();
		}
		delay_ms(400);

//		GPIO_setBit(LED_port, LED_Blue);
//		GPIO_clearBit(LED_port, LED_Blue);
	}
	return 0;
}

void EXTI15_10_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	static int mode = 0;
	if (mode) {
		GPIO_setBit(LED_port, LED_Blue);
	} else {
		GPIO_clearBit(LED_port, LED_Blue);
	}
	mode = (mode + 1) % 2;

	if (GPIO_Pin == GPIO_PIN_12) {
		irq_flag = 1;
	}
}
