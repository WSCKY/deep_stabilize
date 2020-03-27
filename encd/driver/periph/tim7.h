/*
 * tim7.h
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#ifndef DRIVER_PERIPH_TIM7_H_
#define DRIVER_PERIPH_TIM7_H_

#include "SysConfig.h"

typedef void (*UpdateCallback)(void);

status_t tim7_init(uint32_t period, UpdateCallback callback);

#endif /* DRIVER_PERIPH_TIM7_H_ */
