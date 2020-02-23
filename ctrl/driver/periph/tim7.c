/*
 * tim7.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "tim7.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#if FREERTOS_ENABLED
static osSemaphoreId t7_semaphore;
#else
static uint32_t t7_update_flag = 0;
#endif /* FREERTOS_ENABLED */

status_t tim7_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

#if FREERTOS_ENABLED
  /* Define used semaphore */
  osSemaphoreDef(T7_SEM);
  /* Create the semaphore */
  t7_semaphore = osSemaphoreCreate(osSemaphore(T7_SEM) , 1);
  if(t7_semaphore == NULL) return status_error;
#endif /* FREERTOS_ENABLED */

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 4999; /* 5ms */
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock) / 1000000) - 1; /* 1us */
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
  TIM7->CNT = 0;
  TIM7->DIER |= TIM_IT_Update;
  TIM7->CR1 |= TIM_CR1_CEN;

  return status_ok;
}

status_t tim7_check_update(uint32_t timeout)
{
#if FREERTOS_ENABLED
  if(osSemaphoreWait(t7_semaphore, timeout) == osOK) {
    return status_ok;
  }
  return status_timeout;
#else
  while(t7_update_flag == 0);
  return status_ok;
#endif /* FREERTOS_ENABLED */
}

void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
#if FREERTOS_ENABLED
    osSemaphoreRelease(t7_semaphore);
#else
    t7_update_flag = 1;
#endif /* FREERTOS_ENABLED */
  }
}
