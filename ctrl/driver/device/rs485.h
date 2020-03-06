/*
 * RS485.h
 *
 *  Created on: Feb 22, 2020
 *      Author: kychu
 */

#ifndef DRIVER_DEVICE_RS485_H_
#define DRIVER_DEVICE_RS485_H_

#include "ifconfig.h"

status_t rs485_1_init(void);
status_t rs485_1_tx(uint8_t *buffer, uint32_t size);
status_t rs485_1_rx(uint8_t *buffer, uint32_t size);

#if CONFIG_RS485_2_ENABLE
status_t rs485_2_tx(uint8_t *buffer, uint32_t size);
status_t rs485_2_rx(uint8_t *buffer, uint32_t size, uint32_t timeout);
#endif /* CONFIG_RS485_2_ENABLE */

#endif /* DRIVER_DEVICE_RS485_H_ */
