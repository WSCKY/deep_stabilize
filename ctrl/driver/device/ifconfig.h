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
#include "gpio.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#define USER_UART_TX_BYTE                   uart2_TxByte
#define USER_UART_TX_BYTES                  uart2_TxBytesDMA

#define USER_PWM_SET_DUTYCYCLE              pwm_dutycycle

#define USER_LED_ON                         USER_IO_LOW
#define USER_LED_OFF                        USER_IO_HIGH
#define USER_LED_TOG                        USER_IO_TOGGLE

#if FREERTOS_ENABLED
#define delay                               osDelay
#else
#define delay                               _delay_ms
#endif /* FREERTOS_ENABLED */

#endif /* DRIVER_DEVICE_IFCONFIG_H_ */
