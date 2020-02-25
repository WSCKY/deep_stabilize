/*
 * main_task.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef APPS_MAIN_TASK_H_
#define APPS_MAIN_TASK_H_

#include "SysConfig.h"

#include "ifconfig.h"

#include "rs485.h"
#include "mpu9250.h"

#include "kyLink.h"

#include "irq.h"

typedef struct {
  float AngleRateVal;
  float AngleVal;
} AngleInfo_t;

void com_task(void const *arg);
void sins_task(void const *arg);
void ctrl_task(void const *arg);

#endif /* APPS_MAIN_TASK_H_ */
