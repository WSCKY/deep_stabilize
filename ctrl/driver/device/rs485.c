/*
 * RS485.c
 *
 *  Created on: Feb 22, 2020
 *      Author: kychu
 */

#include "rs485.h"

#include "rtu.h"

#define RS485_RTU_CACHE_SIZE   80

rtu_handle_t rtu_rs485_1, rtu_rs485_2;

static status_t rs485_1_tx(uint8_t *buffer, uint32_t size);
static status_t rs485_2_tx(uint8_t *buffer, uint32_t size);
static status_t rs485_1_rx(uint8_t *buffer, uint32_t size, uint32_t timeout);
static status_t rs485_2_rx(uint8_t *buffer, uint32_t size, uint32_t timeout);

status_t rs485_init(void)
{
  status_t ret = status_ok;
  ret = uart1_init();
  if(ret != status_ok) return ret;
  ret = uart4_init();
  if(ret != status_ok) return ret;
  // receiver output enable
  output_port_clear(RS485_CTRL1);
  output_port_clear(RS485_CTRL2);

  rtu_rs485_1.cache_size = 0;
  rtu_rs485_2.cache_size = 0;

  osSemaphoreDef(RTU_SEM_1);
  osSemaphoreDef(RTU_SEM_2);

  rtu_rs485_1.tx_bytes = rs485_1_tx;
  rtu_rs485_1.rx_bytes = rs485_1_rx;
  rtu_rs485_1.sync_obj = osSemaphoreCreate(osSemaphore(RTU_SEM_1), 1);
  if(rtu_rs485_1.sync_obj == NULL)
    return status_error;
  rtu_rs485_1.cache = kmm_alloc(RS485_RTU_CACHE_SIZE);
  if(rtu_rs485_1.cache == NULL)
    return status_nomem;
  rtu_rs485_1.cache_size = RS485_RTU_CACHE_SIZE;

  rtu_rs485_2.tx_bytes = rs485_2_tx;
  rtu_rs485_2.rx_bytes = rs485_2_rx;
  rtu_rs485_2.sync_obj = osSemaphoreCreate(osSemaphore(RTU_SEM_2), 1);
  if(rtu_rs485_2.sync_obj == NULL)
    return status_error;
  rtu_rs485_2.cache = kmm_alloc(RS485_RTU_CACHE_SIZE);
  if(rtu_rs485_2.cache == NULL)
    return status_nomem;
  rtu_rs485_2.cache_size = RS485_RTU_CACHE_SIZE;

  return ret;
}

static status_t rs485_1_tx(uint8_t *buffer, uint32_t size)
{
  output_port_set(RS485_CTRL1); // driver output enable
  uart1_TxBytes(buffer, size);
  _delay_us(100);
//  uart1_TxByte(0); // send one data for delay
  output_port_clear(RS485_CTRL1); // receiver output enable
  return status_ok;
}

static status_t rs485_2_tx(uint8_t *buffer, uint32_t size)
{
  output_port_set(RS485_CTRL2); // driver output enable
  uart4_TxBytes(buffer, size);
  _delay_us(100);
//  uart4_TxByte(0); // send one data for delay
  output_port_clear(RS485_CTRL2); // receiver output enable
  return status_ok;
}

static status_t rs485_1_rx(uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  uint32_t rec = 0;
  uint32_t ts_now, ts_start;
  ts_start = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
  do {
    rec += uart1_pullBytes(buffer, size - rec);
    if(rec < size) {
      ts_now = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
      if((ts_now - ts_start) < timeout)
        uart1_waitBytes(size - rec, timeout + ts_start - ts_now);
      else
        return status_timeout;
    }
  } while(rec < size);
  return status_ok;
}

static status_t rs485_2_rx(uint8_t *buffer, uint32_t size, uint32_t timeout)
{
  uint32_t rec = 0;
  uint32_t ts_now, ts_start;
  ts_start = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
  do {
    rec += uart4_pullBytes(buffer, size - rec);
    if(rec < size) {
      ts_now = xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
      if((ts_now - ts_start) < timeout)
        uart4_waitBytes(size - rec, timeout + ts_start - ts_now);
      else
        return status_timeout;
    }
  } while(rec < size);
  return status_ok;
}

int rtu_req_grant(rtu_handle_t *hrtu)
{
  if(osSemaphoreWait(hrtu->sync_obj, RTU_MUTEX_TIMEOUT) == osOK)
    return 1;
  return 0;
}

void rtu_rel_grant(rtu_handle_t *hrtu)
{
  osSemaphoreRelease(hrtu->sync_obj);
}
