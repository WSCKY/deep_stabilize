/*
 * tim7.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "tim7.h"

static UpdateCallback update_evt;

status_t tim7_init(uint32_t period, UpdateCallback callback)
{
  uint16_t tmpcr1 = 0;

  if(callback == NULL) return status_error;
  update_evt = callback;

  /* enable TIM7 peripheral clock */
  RCC->APB1ENR |= RCC_APB1Periph_TIM7;

  tmpcr1 = TIM7->CR1;
  tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CR1_CKD));
  TIM7->CR1 = tmpcr1;
  TIM7->ARR = period * 10 - 1; /* period in millisecond */
  TIM7->PSC = (uint16_t) ((SystemCoreClock) / 10000) - 1; /* 100us */
  TIM7->EGR = TIM_PSCReloadMode_Immediate;

  TIM7->CNT = 0;
  TIM7->DIER |= TIM_IT_Update;
  TIM7->CR1 |= TIM_CR1_CEN;

  return status_ok;
}

void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    if(update_evt != NULL)
      update_evt();
  }
}
