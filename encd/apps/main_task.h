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

#include "kyLink.h"

#include "irq.h"

#if CONFIG_LOG_ENABLE
#include "log.h"
#endif /* CONFIG_LOG_ENABLE */

#define ENCODER_NUMBER       2

#define SINS_TASK_MODULE_ENABLE                  (0)

void com_task(void const *arg);
void sins_task(void const *arg);
void ctrl_task(void const *arg);
void stat_task(void const *arg);
void encd_task(void const *arg);

#endif /* APPS_MAIN_TASK_H_ */
