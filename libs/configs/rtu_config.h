/*
 * rtu_config.h
 *
 *  Created on: Feb 22, 2020
 *      Author: kychu
 */

#ifndef SYSCONFIG_RTU_CONFIG_H_
#define SYSCONFIG_RTU_CONFIG_H_

#include <pthread.h>
#include <stdint.h>

#include "sys_def.h"

//#define rtu_delay              delay

#define RTU_RECV_TIMEOUT       100     /* milliseconds */
#define RTU_MUTEX_TIMEOUT      1000

#define RTU_SYNC_ENABLED       (1)

#if RTU_SYNC_ENABLED
#define RTU_SYNC_t             pthread_mutex_t*
#endif /* RTU_SYNC_ENABLED */

#endif /* SYSCONFIG_RTU_CONFIG_H_ */
