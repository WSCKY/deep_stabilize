/*
 * pwm.c
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#include "pwm.h"

/**
  * @brief  Configure the PWM Pin.
  * @param  None
  * @retval None
  */
void pwm_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* PWM_GPIO_PORT Clocks enable */
  RCC_AHBPeriphClockCmd(PWM_GPIO_CLK, ENABLE);

  /* PWM_GPIO_PORT Configuration: PWM_GPIO_PIN as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = PWM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(PWM_GPIO_PORT, PWM_GPIO_PinSource, PWM_GPIO_PIN_AF);

  /* Timer Configuration ---------------------------------------------------
   Generate 1 PWM signal with 0% duty cycles:
   Timer input clock (TIMxCLK) is set to APB1 clock (PCLK1)
    => TIMxCLK = PCLK1 = SystemCoreClock
   TIMxCLK = SystemCoreClock, Prescaler = 47, TIMx counter clock = 1MHz
   SystemCoreClock is set to 48 MHz for STM32F0xx devices

   TIMx_Period = (1MHz / 10KHz) - 1 = 99
   The duty cycle is set to 0% as default.
   The Timer pulse is calculated as follows:
     - ChannelyPulse = DutyCycle * (TIMx_Period - 1) / 100
  ----------------------------------------------------------------------- */

  /* timer clock enable */
  PWM_TIM_CLK_EN();

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 47;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 99;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(PWM_TIM, &TIM_TimeBaseStructure);

  /* PWM_TIM Channel Configuration in PWM mode */
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  PWM_TIM_OCxInit(PWM_TIM, &TIM_OCInitStructure);

  /* PWM_TIM counter enable */
  TIM_Cmd(PWM_TIM, ENABLE);
}

void pwm_dutycycle(uint8_t d)
{
	if(d > 99) d = 99;
	PWM_TIM_SET_CCR(d);
}
