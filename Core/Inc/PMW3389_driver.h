/*
 * PMW3389_driver.h
 *
 *  Created on: Oct 5, 2026
 *      Author: jaced
 */

#ifndef INC_PMW3389_DRIVER_H_
#define INC_PMW3389_DRIVER_H_

#include "stm32f4xx_hal.h"

/* Chip register address defines */
#define MOTION_REGISTER					0x02
#define DELTA_X_L_REGISTER				0x03
#define DELTA_X_H_REGISTER				0x04
#define DELTA_Y_L_REGISTER				0x05
#define DELTA_Y_H_REGISTER				0x06
#define POWER_UP_RESET_REGISTER 		0x3A


/* Chip delay defines */
// TSRAD delay (wait for chip to prepare data for read command
#define READ_DATA_DELAY_MICROSECONDS 	160
// TSWW and TSWR delay
#define WRITE_DELAY_MICROSECONDS 		180
// TSRW and TSRR delay
#define READ_DELAY_MICROSECONDS 		20

int pmw3389_init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, TIM_HandleTypeDef* htim3, uint16_t cs_pin);
HAL_StatusTypeDef pmw3389_write_register(uint8_t address, uint8_t data_in);
HAL_StatusTypeDef pmw3389_read_register(uint8_t address, uint8_t* data);
int pmw3389_load_srom();

#endif /* INC_PMW3389_DRIVER_H_ */
