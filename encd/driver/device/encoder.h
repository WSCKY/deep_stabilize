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

#define ENCODER_PRECISION      0x4000 /*(1 << 14)*/
#define ENCODER_DEG2INT(x)     ((uint16_t)((x) * 45.5111111111f))
#define ENCODER_INT2DEG(x)     ((float)(x) * 0.02197265625f)

typedef struct {
  uint8_t addr;
  rtu_handle_t *hrtu;
} encoder_handle_t;

void encoder_init(const encoder_handle_t *henc);
status_t encoder_read(const encoder_handle_t *henc, uint16_t *val);
status_t encoder_baudrate(const encoder_handle_t *henc, uint32_t baudrate);
status_t encoder_origin(const encoder_handle_t *henc);

#endif /* DRIVER_DEVICE_ENCODER_H_ */
