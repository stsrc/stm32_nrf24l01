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

	GPIO_InitTypeDef gpio =
	{
		nrf->csn,
		GPIO_MODE_OUTPUT_PP,
		GPIO_PULLDOWN,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(nrf->gpio, &gpio);

	gpio.Pin = nrf->ce;
	HAL_GPIO_Init(nrf->gpio, &gpio);

	if (nrf->irq) {
		gpio.Pin = nrf->irq;
		gpio.Mode = GPIO_MODE_IT_FALLING;
		gpio.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(nrf->gpio, &gpio);
		if (nrf->irq == GPIO_PIN_12) {
			HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		}
	}

	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET); //CSN
	delay_ms(100);
	HAL_GPIO_WritePin(nrf->gpio, nrf->ce, GPIO_PIN_SET); //CE
	delay_ms(100);
}

void NRF24_init_transmitter(struct nrf24 *nrf)
{
	uint8_t value = 0x0a;
	NRF24_reg_write(nrf, 0x00, &value, 1);
}

void NRF24_init_receiver(struct nrf24 *nrf)
{
	uint8_t value = 0x0b;
	NRF24_reg_write(nrf, 0x00, &value, 1);

	//set packet length to 1
	value = 1;
	NRF24_reg_write(nrf, 0x11, &value, 1);
}

void NRF24_transmitter_send(struct nrf24 *nrf, uint8_t *data, uint8_t size)
{
	if (size > 32)
		return;

	uint8_t value = 0b10100000;
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = SPI_send(&(nrf->spi_handler), &value);
	if (ret)
		goto end;

	SPI_send_multiple(&(nrf->spi_handler), data, size);
end:
	HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET);
}

void NRF24_receiver_receive(struct nrf24 *nrf, uint8_t *buffer, uint8_t bufferSize)
{
	uint8_t status;
	NRF24_reg_read(nrf, 0x07, &status, 1);
	if (status & 0b01000000) {
		// got packet - check its size
		uint8_t readVal;
		uint8_t it = 0;
		uint8_t sizeOfMessage;
packet:
		NRF24_reg_read(nrf, 0x11, &sizeOfMessage, 1);

		if (it + sizeOfMessage > bufferSize)
			return; //TODO: error logging

		// read packet
		readVal = 0b01100001;
		HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_RESET);
		SPI_send(&(nrf->spi_handler), &readVal);
		SPI_read(&(nrf->spi_handler), &buffer[it], sizeOfMessage);
		it += sizeOfMessage;
		HAL_GPIO_WritePin(nrf->gpio, nrf->csn, GPIO_PIN_SET);

		//clear irq
		uint8_t registerValue;
		NRF24_reg_read(nrf, 0x07, &registerValue, 1);
		registerValue |= 0b01000000;
		NRF24_reg_write(nrf, 0x07, &registerValue, 1);

		//check if there is data
		NRF24_reg_read(nrf, 0x17, &registerValue, 1);
		if (!(registerValue & 0b00000001)) {
			goto packet;
		}
	}
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
