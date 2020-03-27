/*
 * pwm16.h
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#ifndef PERIPH_PWM16_H_
#define PERIPH_PWM16_H_

#include "SysConfig.h"

void pwm16_init(void);
void pwm16_dutycycle(uint8_t d);
void pwm16_period(uint32_t p);

#endif /* PERIPH_PWM16_H_ */
