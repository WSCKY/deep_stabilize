/*
 * pwm.h
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#ifndef PERIPH_PWM2_H_
#define PERIPH_PWM2_H_

#include "SysConfig.h"

void pwm2_init(void);
void pwm2_dutycycle(uint8_t d);
void pwm2_period(uint32_t p);

#endif /* PERIPH_PWM2_H_ */
