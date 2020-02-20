/*
 * gpio.h
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#ifndef PERIPH_GPIO_H_
#define PERIPH_GPIO_H_

#include "SysConfig.h"

#define USER_IO_LOW()                  GPIOB->BRR = GPIO_Pin_12;
#define USER_IO_HIGH()                 GPIOB->BSRR = GPIO_Pin_12;
#define USER_IO_TOGGLE()               GPIOB->ODR ^= GPIO_Pin_12;

typedef enum {
  IO_INPUT1 = 0,
  IO_INPUT2 = 1,
  IO_INPUT3 = 2,
  IO_INPUT4 = 3,
} INPUT_IO_ID;

typedef enum {
  IO_OUTPUT1 = 0,
  IO_OUTPUT2 = 1,
  IO_OUTPUT3 = 2,
  IO_OUTPUT4 = 3,
  IO_OUTPUT5 = 4,
  IO_OUTPUT6 = 5,
  LED_GREEN = 6,
  RS485_CTRL1 = 7,
  RS485_CTRL2 = 8,
} OUTPUT_IO_ID;

void board_gpio_init(void);
uint8_t input_port_read(uint32_t idx);

void output_port_set(uint32_t idx);
void output_port_clear(uint32_t idx);
void output_port_toggle(uint32_t idx);
void output_port_write(uint32_t idx, uint32_t val);

#endif /* PERIPH_GPIO_H_ */
