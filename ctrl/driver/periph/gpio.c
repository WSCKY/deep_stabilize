/*
 * gpio.c
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#include "gpio.h"

typedef struct {
  GPIO_TypeDef* GPIOx;
  uint32_t GPIO_Pin;
} USER_IO_LIST;

#define INPUT_IO_NUMBER        4
static const USER_IO_LIST INPUT_IOS[INPUT_IO_NUMBER] = {
  {GPIOA, GPIO_Pin_15},
  {GPIOB, GPIO_Pin_3},
  {GPIOB, GPIO_Pin_4},
  {GPIOB, GPIO_Pin_5},
};

#define OUTPUT_IO_NUMBER       7
static const USER_IO_LIST OUTPUT_IOS[OUTPUT_IO_NUMBER] = {
  {GPIOB, GPIO_Pin_13},
  {GPIOB, GPIO_Pin_14},
  {GPIOB, GPIO_Pin_15},
  {GPIOA, GPIO_Pin_8},
  {GPIOA, GPIO_Pin_9},
  {GPIOA, GPIO_Pin_10},
  {GPIOB, GPIO_Pin_12}, // GREEN LED
};

void board_gpio_init(void)
{
  int i;
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIOs clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);

  /* configuration for output pins */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  for(i = 0; i < OUTPUT_IO_NUMBER; i ++) {
    GPIO_InitStructure.GPIO_Pin = OUTPUT_IOS[i].GPIO_Pin;
    GPIO_Init(OUTPUT_IOS[i].GPIOx, &GPIO_InitStructure);
  }

  /* configuration for input pins */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  for(i = 0; i < INPUT_IO_NUMBER; i ++) {
    GPIO_InitStructure.GPIO_Pin = INPUT_IOS[i].GPIO_Pin;
    GPIO_Init(INPUT_IOS[i].GPIOx, &GPIO_InitStructure);
  }
}

uint8_t input_port_read(uint32_t idx)
{
  uint8_t bitstatus = 0x00;
  if(INPUT_IO_NUMBER > idx) {
    if((INPUT_IOS[idx].GPIOx->IDR & INPUT_IOS[idx].GPIO_Pin) != (uint32_t)Bit_RESET) {
      bitstatus = (uint8_t)Bit_SET;
    } else {
      bitstatus = (uint8_t)Bit_RESET;
    }
  }
  return bitstatus;
}

void output_port_set(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin;
  }
}

void output_port_clear(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->BRR = OUTPUT_IOS[idx].GPIO_Pin;
  }
}

void output_port_toggle(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->ODR ^= OUTPUT_IOS[idx].GPIO_Pin;
  }
}

void output_port_write(uint32_t idx, uint32_t val)
{
  if(OUTPUT_IO_NUMBER > idx) {
    if(val != 0) {
      OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin;
    } else {
      OUTPUT_IOS[idx].GPIOx->BRR = OUTPUT_IOS[idx].GPIO_Pin;
    }
  }
}
