/*
 * encoder.h
 *
 *  Created on: Feb 22, 2020
 *      Author: kychu
 */

#ifndef DRIVER_DEVICE_ENCODER_H_
#define DRIVER_DEVICE_ENCODER_H_

#include "ifconfig.h"

#include "rtu.h"

typedef struct {
  uint8_t addr;
  rtu_handle_t *hrtu;
} encoder_handle_t;

void encoder_init(const encoder_handle_t *henc);
status_t encoder_read(const encoder_handle_t *henc, uint16_t *val);
status_t encoder_baudrate(const encoder_handle_t *henc, uint32_t baudrate);
status_t encoder_origin(const encoder_handle_t *henc);

#endif /* DRIVER_DEVICE_ENCODER_H_ */
