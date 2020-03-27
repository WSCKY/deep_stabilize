/**
  ******************************************************************************
  * @file    ./src/TimerCounter.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   This module provides a timing clock for system.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "TimerCounter.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TC_TIMER               TIM6
#define TC_TIMER_CLK_EN()      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE)
#define TC_TIMER_IRQCHAN       TIM6_DAC_IRQn
#define TC_TIMER_IRQHANDLER    TIM6_DAC_IRQHandler

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t OverFlowCnt = 0;
static uint32_t TimeMeasure = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the clock timer.
	* @param  None
	* @retval None
	*/
void _TimeTicksInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* TC_TIMER clock enable */
	TC_TIMER_CLK_EN();

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; /* 16-bit timer */
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock) / 1000000) - 1; /* 1us */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TC_TIMER, &TIM_TimeBaseStructure);

	/* NVIC configuration *******************************************************/
	NVIC_InitStructure.NVIC_IRQChannel = TC_TIMER_IRQCHAN;
	NVIC_InitStructure.NVIC_IRQChannelPriority = SYSTEM_TIMER_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Clear TC_TIMER Counter */
	TIM_SetCounter(TC_TIMER, 0);

	/* TC_TIMER Interrupts enable */
	TIM_ITConfig(TC_TIMER, TIM_IT_Update, ENABLE);

	/* TC_TIMER counter enable */
	TIM_Cmd(TC_TIMER, ENABLE);
}

/*
 * return timer counter value.
 */
uint16_t _Get_Ticks(void) { return TC_TIMER->CNT; }

/*
 * time in microsecond.
 */
uint32_t _Get_Micros(void)
{
	uint32_t t1, t2;
	do{
		t1 = (OverFlowCnt << 16) + (TC_TIMER->CNT & 0xFFFF);
		t2 = (OverFlowCnt << 16) + (TC_TIMER->CNT & 0xFFFF);
	} while(t1 != t2);
	return t1;
}

/*
 * time in millisecond.
 */
uint32_t _Get_Millis(void)
{
	return ((_Get_Micros()) / 1000);
}

/*
 * return seconds.
 */
uint32_t _Get_Secnds(void)
{
	return ((_Get_Millis()) / 1000);
}

/*
 * delay a time in microsecond.
 */
void _delay_us(uint32_t us)
{
	uint32_t _start = _Get_Micros();
	uint32_t _us = us;
	uint32_t _cur = _start;
	do {
		_cur = _Get_Micros();
	} while((_cur - _start) < _us);
}

/*
 * delay a time in millisecond.
 */
void _delay_ms(uint32_t ms)
{
	uint32_t _start = _Get_Millis();
	uint32_t _ms = ms;
	uint32_t _cur = _start;
	do {
		_cur = _Get_Millis();
	} while((_cur - _start) < _ms);
}

void _MeasureTimeStart(void)
{
  TimeMeasure = _Get_Micros();
}

uint32_t _GetTimeMeasured(void)
{
  return (_Get_Micros() - TimeMeasure);
}

/**
  * @brief  This function handles the TC_TIMER interrupt request.
  * @param  None
  * @retval None
  */
void TC_TIMER_IRQHANDLER(void)
{
	if (TIM_GetITStatus(TC_TIMER, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TC_TIMER, TIM_IT_Update);
		OverFlowCnt ++;
	}
}

/******************************** END OF FILE *********************************/
