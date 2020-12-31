#include "SPI.h"

void SPI_show_error(HAL_StatusTypeDef rt)
{
	if (rt)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	(void)hspi;
	if (hspi->Instance == SPI1) {
		GPIO_InitTypeDef gpio =
		{
			GPIO_PIN_5, //GPIOA5 - sck; GPIOA7 - mosi
			GPIO_MODE_AF_PP,
			GPIO_PULLDOWN,
			GPIO_SPEED_FREQ_LOW
		};

		GPIO_InitTypeDef gpio_miso =
		{
			GPIO_PIN_6,
			GPIO_MODE_AF_INPUT,
			GPIO_PULLDOWN,
			GPIO_SPEED_FREQ_LOW
		};

		__HAL_RCC_SPI1_CLK_ENABLE();

		HAL_GPIO_Init(GPIOA, &gpio);

		gpio.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOA, &gpio);

		HAL_GPIO_Init(GPIOA, &gpio_miso);
	} else if (hspi->Instance == SPI2) {
		GPIO_InitTypeDef gpio =
		{
			GPIO_PIN_13, // sck
			GPIO_MODE_AF_PP,
			GPIO_PULLDOWN,
			GPIO_SPEED_FREQ_LOW
		};

		GPIO_InitTypeDef gpio_miso =
		{
			GPIO_PIN_14,
			GPIO_MODE_AF_INPUT,
			GPIO_PULLDOWN,
			GPIO_SPEED_FREQ_LOW
		};

		__HAL_RCC_SPI2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		HAL_GPIO_Init(GPIOB, &gpio);

		gpio.Pin = GPIO_PIN_15; // mosi
		HAL_GPIO_Init(GPIOB, &gpio);

		HAL_GPIO_Init(GPIOB, &gpio_miso);
	}
}

static void SPI_handler_basic_init(SPI_HandleTypeDef *spi_handler,
				   SPI_TypeDef *inst)
{
	spi_handler->Instance = inst;
	spi_handler->Init.Mode = SPI_MODE_MASTER;
	spi_handler->Init.Direction = SPI_DIRECTION_2LINES;
	spi_handler->Init.DataSize = SPI_DATASIZE_8BIT;
	spi_handler->Init.CLKPolarity = SPI_POLARITY_LOW;
	spi_handler->Init.CLKPhase = SPI_PHASE_1EDGE;
	spi_handler->Init.NSS = SPI_NSS_SOFT;
	spi_handler->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	spi_handler->Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi_handler->Init.TIMode = SPI_TIMODE_DISABLE;
	spi_handler->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
}

static uint16_t SPI_wait_for_EOT(SPI_HandleTypeDef *handler)
{
	while(handler->Instance->CR2 & SPI_CR2_TXDMAEN);
	while(handler->Instance->CR2 & SPI_CR2_RXDMAEN);
	while(!(handler->Instance->SR & SPI_SR_TXE));
	while(handler->Instance->SR & SPI_SR_BSY);
	return 0;
}

HAL_StatusTypeDef SPI_init(SPI_HandleTypeDef *spi, SPI_TypeDef *instance)
{
	HAL_StatusTypeDef rt;
	SPI_handler_basic_init(spi, instance);
	rt = HAL_SPI_Init(spi);
	if (rt)
		return rt;
	return HAL_OK;
}

HAL_StatusTypeDef SPI_send(SPI_HandleTypeDef *spi_handler, uint8_t *data)
{
	HAL_StatusTypeDef rt;
	SPI_wait_for_EOT(spi_handler);
	rt = HAL_SPI_Transmit(spi_handler, data, 1, 0xFFFF);
	SPI_wait_for_EOT(spi_handler);
	return rt;
}

HAL_StatusTypeDef SPI_read(SPI_HandleTypeDef *spi_handler, uint8_t *data, uint16_t bytes)
{
	HAL_StatusTypeDef rt;
	SPI_wait_for_EOT(spi_handler);
	rt = HAL_SPI_Receive(spi_handler, data, bytes, 0xFFFF);
	SPI_wait_for_EOT(spi_handler);
	return rt;
}

HAL_StatusTypeDef SPI_send_and_receive(SPI_HandleTypeDef *spi_handler, uint8_t data, uint8_t *receiveData)
{
	HAL_StatusTypeDef rt;
	SPI_wait_for_EOT(spi_handler);
	rt = HAL_SPI_TransmitReceive(spi_handler, &data, receiveData, 1, 0xFFFF);
	SPI_wait_for_EOT(spi_handler);
	return rt;
}

HAL_StatusTypeDef SPI_send_multiple(SPI_HandleTypeDef *spi_handler, uint8_t *data, uint8_t size)
{
	HAL_StatusTypeDef rt;
	SPI_wait_for_EOT(spi_handler);
	rt = HAL_SPI_Transmit(spi_handler, data, size, 0xFFFF);
	SPI_wait_for_EOT(spi_handler);
	return rt;
}

