#include "NRF24.h"
#include "UART.h"
#include <stdio.h>
#include "stupid_delay.h"

static HAL_StatusTypeDef NRF24_reg_read(uint8_t reg, uint8_t *data, uint8_t dataSize)
{
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	ret = SPI_1_send(&reg);
	if (ret)
		return ret;

	ret = SPI_1_read(data, dataSize);
	if (ret)
		return ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //CSN
	return ret;
}

static HAL_StatusTypeDef NRF24_reg_write(uint8_t reg, uint8_t *data, uint8_t dataSize)
{
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	uint8_t value = 0b00100000 | reg;
	ret = SPI_1_send(&value);
	if (ret) return ret;
	ret = SPI_1_send(data);
	if (ret) return ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //CSN
	return ret;
}

void NRF24_read_status(void)
{
	uint8_t status;
	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	uint8_t value = 0b11111111;
	ret = SPI_1_send_and_receive(value, &status);
	if (ret)
		return;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //CSN

	char buffer[64];
	sprintf(buffer, "status - 0x%02x\r\n", status);
	buffer_set_text(&UART1_transmit_buffer, buffer, strlen(buffer));
	UART_1_transmit();
}

HAL_StatusTypeDef NRF24_write_to_tx_fifo(uint8_t *data, uint8_t dataSize) {
	if (dataSize > 32) {
		return HAL_ERROR;
	}

	HAL_StatusTypeDef ret;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	delay_us(100);

	uint8_t opcode = 0b10100000;
	ret = SPI_1_send(&opcode);
	if (ret)
		return ret;

	ret = SPI_1_send_multiple(data, dataSize);
	if (ret)
		return ret;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //CSN
	delay_us(100);

	return ret;
}

void NRF24_init(void)
{
	SPI_1_init();
	GPIO_InitTypeDef gpio_ce_csn =
	{
		GPIO_PIN_3 | GPIO_PIN_4, //csn - GPIOA4, ce - GPIOA3
		GPIO_MODE_OUTPUT_PP,
		GPIO_NOPULL,
		GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOA, &gpio_ce_csn);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); //CE
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); //CSN
	delay_ms(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); //CE
}



void NRF24_test(void)
{
	static unsigned iteration = 0;
	iteration++;
	uint8_t result = 0;
	char buffer[64];
	HAL_StatusTypeDef ret;

	ret = NRF24_reg_read(0x07, &result, 1);
	if (ret)
		return;

	sprintf(buffer, "iteration - 0x%08x\r\n", iteration);
	buffer_set_text(&UART1_transmit_buffer, buffer, strlen(buffer));
	sprintf(buffer, "register  - 0x%02x\r\n\n", result);
	buffer_set_text(&UART1_transmit_buffer, buffer, strlen(buffer));
	UART_1_transmit();
}
