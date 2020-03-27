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

typedef struct {
  uint8_t dir:1;
  uint8_t stop:1;
  uint8_t cmd:1;
  uint8_t rev:5;
  uint16_t speed;
} servo_inching_t;

void servo_init(const servo_handle_t *hsrv);
status_t servo_detect(const servo_handle_t *hsrv);
status_t servo_get_position(const servo_handle_t *hsrv, uint32_t *val);
status_t servo_set_current(const servo_handle_t *hsrv, uint32_t val);
status_t servo_speed_up_time(const servo_handle_t *hsrv, uint16_t time);
status_t servo_speed_down_time(const servo_handle_t *hsrv, uint16_t time);
status_t servo_set_speed(const servo_handle_t *hsrv, uint16_t val);
status_t servo_get_speed(const servo_handle_t *hsrv, uint16_t *val);
status_t servo_set_inching(const servo_handle_t *hsrv, servo_inching_t cmd);
status_t servo_run_time(const servo_handle_t *hsrv, int32_t time);
status_t servo_run_pulse(const servo_handle_t *hsrv, int32_t pulse);
status_t servo_run_position(const servo_handle_t *hsrv, int32_t pos);
status_t servo_set_position(const servo_handle_t *hsrv, int32_t pos);
status_t servo_set_release(const servo_handle_t *hsrv, uint32_t sta);

#endif /* DRIVER_DEVICE_SERVO_H_ */
