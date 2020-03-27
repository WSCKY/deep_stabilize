/*
 * ifconfig.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef DRIVER_DEVICE_IFCONFIG_H_
#define DRIVER_DEVICE_IFCONFIG_H_

#include "spi1.h"
#include "tim7.h"
#include "intio.h"
#include "uart1.h"
#include "uart2.h"
#include "uart3.h"
#include "uart4.h"
#include "pwm.h"
#include "pwm16.h"
#include "pwm17.h"
#include "gpio.h"
#include "enc3.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#define USER_PWM_SET_DUTYCYCLE              pwm_dutycycle

#define USER_LED_ON                         USER_IO_LOW
#define USER_LED_OFF                        USER_IO_HIGH
#define USER_LED_TOG                        USER_IO_TOGGLE

#if FREERTOS_ENABLED
#define delay                               osDelay
#else
#define delay                               _delay_ms
#endif /* FREERTOS_ENABLED */

#define USER_LOG_PORT                       (4)

#if (USER_LOG_PORT == 1)
#define LOG_PORT_INIT                       uart1_init
#define LOG_PORT_TXCHAR                     uart1_TxByte
#define LOG_PORT_OUTPUT                     uart1_TxString
#elif (USER_LOG_PORT == 2)
#define LOG_PORT_INIT                       uart2_init
#define LOG_PORT_TXCHAR                     uart2_TxByte
#define LOG_PORT_OUTPUT                     uart2_TxString
#elif (USER_LOG_PORT == 3)
#define LOG_PORT_INIT                       uart3_init
#define LOG_PORT_TXCHAR                     uart3_TxByte
#define LOG_PORT_OUTPUT                     uart3_TxString
#elif (USER_LOG_PORT == 4)
#define LOG_PORT_INIT                       uart4_init
#define LOG_PORT_TXCHAR                     uart4_TxByte
#define LOG_PORT_OUTPUT                     uart4_TxString
#else
#define LOG_PORT_INIT                       uart2_init
#define LOG_PORT_TXCHAR                     uart2_TxByte
#define LOG_PORT_OUTPUT                     uart2_TxString
#endif

#endif /* DRIVER_DEVICE_IFCONFIG_H_ */
