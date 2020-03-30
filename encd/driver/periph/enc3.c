/*
 * encd.c
 *
 *  Created on: Mar 27, 2020
 *      Author: kychu
 */

#include "enc3.h"

void encoder3_init(void)
{
  /* Initialize Structure */
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  /* Enable periph clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  /* Configure GPIO PIN as alternate function mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_1);

  /* Configure Timer as Encoder Interface */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* Counter counts on both TI1FP1 and TI2FP2 edges && specifies the IC1 Polarity is Rising edge */
  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  /* Configure Input Capture Parameters */
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 10;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);

  /* Enable the Encoder Timer Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  /* Enable Update interrupt */
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_SetCounter(TIM3, 0); /* Reset Counter Register */
  TIM_Cmd(TIM3, ENABLE);
}

int16_t encoder3_read(void)
{
  int16_t cnt = 0;
  cnt = (int16_t)TIM3->CNT;
  TIM3->CNT = 0;
  return cnt;
}

/*
 * this function handles Encoder A Timer interrupt.
 */
void TIM3_IRQHandler(void)
{
  if(TIM3->SR & 0X0001) {
    /* __nop(); */
  }
  TIM3->SR &= ~(1 << 0);
}
