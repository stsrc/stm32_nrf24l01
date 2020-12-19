#ifndef _SPI_H_
#define _SPI_H_

#include <stdlib.h>
#include <string.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_dma.h>
#include <stm32f1xx_hal_spi.h>
#include <stm32f1xx_hal_cortex.h>

/**
 * @addtogroup RFID_System libraries
 * @{
 */

/**
 * @defgroup SPI
 * @brief SPI library.
 * @{
 *
 * SPI library, which is based on HAL SPI library. LCD, touchpanel and SD card reader uses
 * SPI in this project.
 */

/**
 * @brief Function presents that SPI result failed, by switching on LEDs.
 * @{
 * If error occures, execution of program is stoped.
 */
void SPI_show_error(HAL_StatusTypeDef rt);

HAL_StatusTypeDef SPI_1_init();
HAL_StatusTypeDef SPI_1_send_multiple(uint8_t *data, uint8_t size);
HAL_StatusTypeDef SPI_1_send(uint8_t *data);
HAL_StatusTypeDef SPI_1_send_and_receive(uint8_t data, uint8_t *receiveData);
HAL_StatusTypeDef SPI_1_read(uint8_t *data, uint16_t bytes);

#endif

/* TODO unification ! */
