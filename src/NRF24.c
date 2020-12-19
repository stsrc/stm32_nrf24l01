#include "NRF24.h"
#include "UART.h"
#include <stdio.h>
#include "stupid_delay.h"

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
	GPIO_InitTypeDef gpio_ce_csn =
	{
		nrf->csn | nrf->ce, //csn - GPIOA4, ce - GPIOA3
		GPIO_MODE_OUTPUT_PP,
		GPIO_NOPULL,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(nrf->gpio, &gpio_ce_csn);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_RESET); //CE
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	delay_ms(100);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_SET); //CE
}

void NRF24_init_transmitter(struct nrf24 *nrf)
{

}

void NRF24_init_receiver(struct nrf24 *nrf)
{

}

void NRF24_test(struct nrf24 *nrf)
{
	static unsigned iteration = 0;
	iteration++;
	uint8_t result = 0;
	char buffer[64];
	HAL_StatusTypeDef ret;

	ret = NRF24_reg_read(nrf, 0x0, &result, 1);
	if (ret)
		return;

	sprintf(buffer, "iteration - 0x%08x\r\n", iteration);
	buffer_set_text(&UART1_transmit_buffer, buffer, strlen(buffer));
	sprintf(buffer, "register  - 0x%02x\r\n\n", result);
	buffer_set_text(&UART1_transmit_buffer, buffer, strlen(buffer));
	UART_1_transmit();
}
