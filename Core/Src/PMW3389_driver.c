/*
 * PMW3389_driver.c
 *
 *  Created on: Oct 5, 2026
 *      Author: jaced
 */

#include "PMW3389_driver.h"
#include "stm32f4xx_hal_spi.h"

// Send a write register command to the PMW3389
HAL_StatusTypeDef pmw3389_write_register(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, uint8_t ddress, uint8_t data_in)
{
	if (!hspi)
	{
		return HAL_ERROR;
	}

	uint8_t packets[2] =
	{
			address | 0x80,
			data_in
	};

	HAL_GPIO_WritePin(cs_port, cs_pin, 0);

	HAL_StatusTypeDef retVal = HAL_SPI_Transmit(hspi, packets, 2, 1);

	HAL_GPIO_WritePin(cs_port, cs_pin, 1);

	return retVal;
}


void pmw3389_read_register();
void pmw3389_enable();
void pmw3389_load_srom();
