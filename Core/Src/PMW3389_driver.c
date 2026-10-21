/*
 * PMW3389_driver.c
 *
 *  Created on: Oct 5, 2026
 *      Author: jaced
 */

#include "PMW3389_driver.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal.h"

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
// Returns:
// 0 on success
// -1 if one of the parameters was NULL
// -2 on failure to download SROM
// -3 if step 8 never reads 0xC0 out of register 0x3D
int pmw3389_init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, TIM_HandleTypeDef* htim3, uint16_t cs_pin)
{
	if (!hspi || !cs_port || !htim3)
	{
		return -1;
	}

	PMW_hspi 	= 	hspi;
	PMW_cs_port = 	cs_port;
	PMW_htim3	= 	htim3;
	PMW_cs_pin	=	cs_pin;

	HAL_TIM_Base_Start(htim3);

	/* POWER UP SEQUENCE */

	// Step 2
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_SET); // Drive cs high
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_RESET); // Drive cs low

	// Step 3
	pmw3389_write_register(POWER_UP_RESET_REGISTER, 0x5A);

	// Step 4
	HAL_Delay(50);

	// Step 5
	uint8_t data = 0;
	pmw3389_read_register(MOTION_REGISTER, &data);
	pmw3389_read_register(DELTA_X_L_REGISTER, &data);
	pmw3389_read_register(DELTA_X_H_REGISTER, &data);
	pmw3389_read_register(DELTA_Y_L_REGISTER, &data);
	pmw3389_read_register(DELTA_Y_H_REGISTER, &data);

	// Step 6
	if (pmw_3389_load_srom() != 0)
	{
		return -2;
	}

	// Step 7
	pmw3389_write_register(0x3D, 0x80);

	// Step 8
	data = 0;
	uint16_t iter = 55;
	while (iter > 0 || data == 0xC0)
	{
		pmw3389_read_register(0x3D, &data);
		// Datasheet calls for 1ms delay between reads with +/-1% tolerance
		// pmw3389_read_register has an inherent delay of READ_REGISTER_DELAY or 20us
		// so we delay for 980us to get 1ms
		delay_us(980);
	}

	if (data != 0xC0)
	{
		return -2; // Didn't get correct value
	}

	// Step 9
	pmw3389_write_register(0x3D, 0x00);

	// Step 10
	pmw3389_write_register(0x10, 0x20);

	// Step 11??? Not sure what load configuration means at this point? Figure out later

	return 0;
}

// Send a write register command to the PMW3389
HAL_StatusTypeDef pmw3389_write_register(uint8_t address, uint8_t data_in)
{
	if (!PMW_hspi || !PMW_cs_port || !PMW_htim3)
	{
		// pmw3389_init() wasn't called or wasn't successful
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
		delay_us(WRITE_DELAY_MICROSECONDS); // Delay between read/write commands
	}

	return retVal;
}


HAL_StatusTypeDef pmw3389_read_register(uint8_t address, uint8_t* data)
{
	if (!PMW_hspi || !PMW_cs_port || !PMW_htim3)
	{
		// pmw3389_init() wasn't called or wasn't successful
		return HAL_ERROR;
	}

	uint8_t packet = address & 0x7F;
	uint8_t dummy_tx = 0xFF;

	// CS low
	HAL_GPIO_WritePin(PMW_cs_port, PMW_cs_pin, GPIO_PIN_RESET);

	// Send packet read packet
	HAL_StatusTypeDef retVal = HAL_SPI_Transmit(PMW_hspi, &packet, 1, 10);

	if (retVal == HAL_OK)
	{
		// Wait for sensor to prepare data
		delay_us(READ_DATA_DELAY_MICROSECONDS);

        // Transmit dummy data and receive register data
        retVal = HAL_SPI_TransmitReceive(
            PMW_hspi,
            &dummy_tx,
            data,
            1,
            10
        );
	}

	// CS high
	HAL_GPIO_WritePin(PMW_cs_port, PMW_cs_pin, GPIO_PIN_SET);

	// Delay until the next command is able to be received
	delay_us(READ_DELAY_MICROSECONDS);

	return retVal;
}

int pmw3389_load_srom();
