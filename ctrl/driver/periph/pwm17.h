/*
 * pwm17.h
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#ifndef PERIPH_PWM17_H_
#define PERIPH_PWM17_H_

#include "SysConfig.h"

void pwm17_init(void);
void pwm17_dutycycle(uint8_t d);
void pwm17_period(uint32_t p);

#endif /* PERIPH_PWM17_H_ */
