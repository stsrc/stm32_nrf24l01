#ifndef _NRF24_H_
#define _NRF24_H_
#include "SPI.h"

struct nrf24 {
	SPI_HandleTypeDef spi_handler;
	SPI_TypeDef *spi_instance;
	uint32_t csn;
	uint32_t ce;
	uint32_t irq;
	GPIO_TypeDef *gpio;
};

void NRF24_init(struct nrf24 *nrf);
void NRF24_init_transmitter(struct nrf24 *nrf);
//void NRF24_init_receiver(struct nrf24 *nrf);
//void NRF24_test(struct nrf24 *nrf);
void NRF24_transmitter_send(struct nrf24 *nrf, uint8_t *data, uint8_t size);
//void NRF24_receiver_receive(struct nrf24 *nrf, uint8_t *buffer, uint8_t bufferSize);
#endif
