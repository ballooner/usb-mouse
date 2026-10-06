/*
 * PMW3389_driver.h
 *
 *  Created on: Oct 5, 2026
 *      Author: jaced
 */

#ifndef INC_PMW3389_DRIVER_H_
#define INC_PMW3389_DRIVER_H_

void pmw3389_write_register(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin, uint8_t read_address, uint8_t data_in);
void pmw3389_read_register();
void pmw3389_enable();
void pmw3389_load_srom();

#endif /* INC_PMW3389_DRIVER_H_ */
