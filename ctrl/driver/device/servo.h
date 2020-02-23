/*
 * servo.h
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#ifndef DRIVER_DEVICE_SERVO_H_
#define DRIVER_DEVICE_SERVO_H_

#include "ifconfig.h"

#include "rtu.h"

typedef struct {
  uint8_t addr;
  rtu_handle_t *hrtu;
} servo_handle_t;

#endif /* DRIVER_DEVICE_SERVO_H_ */
