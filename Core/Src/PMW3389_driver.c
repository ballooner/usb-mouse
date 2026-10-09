/*
 * PMW3389_driver.c
 *
 *  Created on: Oct 5, 2026
 *      Author: jaced
 */

#include "PMW3389_driver.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"

static SPI_HandleTypeDef* 	PMW_hspi;
static GPIO_TypeDef*		PMW_cs_port;
static TIM_HandleTypeDef*	PMW_htim3;
static uint16_t			PMW_cs_pin;

static void delay_us(uint16_t us)
{
	if (!PMW_htim3)
	{
		return;
	}
	__HAL_TIM_SET_COUNTER(PMW_htim3, 0);

	while (__HAL_TIM_GET_COUNTER(PMW_htim3) < us);
}


// Initialize global variables for PMW3389 to function properly
void pmw3389_init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, TIM_HandleTypeDef* htim3, uint16_t cs_pin)
{
	if (!hspi || !cs_port || !htim3)
	{
		return;
	}

	PMW_hspi 	= 	hspi;
	PMW_cs_port = 	cs_port;
	PMW_htim3	= 	htim3;
	PMW_cs_pin	=	cs_pin;

	HAL_TIM_Base_Start(&htim3);
}

// Send a write register command to the PMW3389
HAL_StatusTypeDef pmw3389_write_register(uint8_t address, uint8_t data_in)
{
	if (!PMW_hspi || !PMW_cs_port || !PMW_htim3)
	{
		// pmw3389_init() wasn't called if this happens
		return HAL_ERROR;
	}

	uint8_t packets[2] =
	{
			address | 0x80,
			data_in
	};

	HAL_GPIO_WritePin(PMW_cs_port, PMW_cs_pin, GPIO_PIN_RESET);

	HAL_StatusTypeDef retVal = HAL_SPI_Transmit(PMW_hspi, packets, 2, 1);

	HAL_GPIO_WritePin(PMW_cs_port, PMW_cs_pin, GPIO_PIN_SET);

	if (retVal == HAL_OK)
	{
		delay_us(180); // Delay between read/write commands
	}

	return retVal;
}


void pmw3389_read_register();
void pmw3389_enable();
void pmw3389_load_srom();
