/*
 * pwm.h
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#ifndef PERIPH_PWM_H_
#define PERIPH_PWM_H_

#include "SysConfig.h"

#define PWM_TIM                TIM3
#define PWM_TIM_CLK_EN()       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)

#define PWM_GPIO_PORT          GPIOB
#define PWM_GPIO_CLK           RCC_AHBPeriph_GPIOB
#define PWM_GPIO_PIN           GPIO_Pin_0
#define PWM_GPIO_PinSource     GPIO_PinSource0
#define PWM_GPIO_PIN_AF        GPIO_AF_1

#define PWM_TIM_OCxInit        TIM_OC3Init
#define PWM_TIM_SET_CCR(x)     do { PWM_TIM->CCR3= x; } while(0);

void pwm_init(void);
void pwm_dutycycle(uint8_t d);

#endif /* PERIPH_PWM_H_ */
