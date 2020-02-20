/*
 * rtu.c
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#include "rtu.h"

static void cal_crc16(uint8_t *buffer, uint32_t len, uint8_t *ret);

status_t rtu_read_single(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr)
{
  uint32_t cnt = 0;
  if(hrtu->cache_size == 0) return status_nomem;
  if(hrtu->tx_cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->tx_cache[cnt ++] = dev_addr;
  hrtu->tx_cache[cnt ++] = RTU_CMD_READ_SINGLE;
  hrtu->tx_cache[cnt ++] = reg_addr >> 8;
  hrtu->tx_cache[cnt ++] = reg_addr;
  hrtu->tx_cache[cnt ++] = 0x00;
  hrtu->tx_cache[cnt ++] = 0x01;
  cal_crc16(hrtu->tx_cache, cnt, hrtu->tx_cache + cnt);
  return hrtu->tx_bytes(hrtu->tx_cache, cnt + 2);
}

status_t rtu_read_multi(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t reg_numb)
{
  uint32_t cnt = 0;
  if(hrtu->cache_size == 0) return status_nomem;
  if(hrtu->tx_cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->tx_cache[cnt ++] = dev_addr;
  hrtu->tx_cache[cnt ++] = RTU_CMD_READ_MULTI;
  hrtu->tx_cache[cnt ++] = reg_addr >> 8;
  hrtu->tx_cache[cnt ++] = reg_addr;
  hrtu->tx_cache[cnt ++] = reg_numb >> 8;
  hrtu->tx_cache[cnt ++] = reg_numb;
  cal_crc16(hrtu->tx_cache, cnt, hrtu->tx_cache + cnt);
  return hrtu->tx_bytes(hrtu->tx_cache, cnt + 2);
}

status_t rtu_write_single(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t reg_data)
{
  uint32_t cnt = 0;
  if(hrtu->cache_size == 0) return status_nomem;
  if(hrtu->tx_cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->tx_cache[cnt ++] = dev_addr;
  hrtu->tx_cache[cnt ++] = RTU_CMD_WRITE_SINGLE;
  hrtu->tx_cache[cnt ++] = reg_addr >> 8;
  hrtu->tx_cache[cnt ++] = reg_addr;
  hrtu->tx_cache[cnt ++] = reg_data >> 8;
  hrtu->tx_cache[cnt ++] = reg_data;
  cal_crc16(hrtu->tx_cache, cnt, hrtu->tx_cache + cnt);
  return hrtu->tx_bytes(hrtu->tx_cache, cnt + 2);
}

status_t rtu_write_multi(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t *reg_data, uint16_t size)
{
  uint32_t cnt = 0, i;
  if(hrtu->cache_size == 0) return status_nomem;
  if(hrtu->tx_cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->tx_cache[cnt ++] = dev_addr;
  hrtu->tx_cache[cnt ++] = RTU_CMD_WRITE_MULTI;
  hrtu->tx_cache[cnt ++] = reg_addr >> 8;
  hrtu->tx_cache[cnt ++] = reg_addr;
  /* number of registers */
  hrtu->tx_cache[cnt ++] = size >> 8;
  hrtu->tx_cache[cnt ++] = size;
  /* number of bytes */
  hrtu->tx_cache[cnt ++] = size << 1;
  for(i = 0; i < size; i ++) {
    hrtu->tx_cache[cnt ++] = reg_data[i] >> 8;
    hrtu->tx_cache[cnt ++] = reg_data[i];
  }
  cal_crc16(hrtu->tx_cache, cnt, hrtu->tx_cache + cnt);
  return hrtu->tx_bytes(hrtu->tx_cache, cnt + 2);
}

status_t rtu_checksum()
{
  return status_ok;
}

static void cal_crc16(uint8_t *buffer, uint32_t len, uint8_t *ret)
{
//  uint8_t CRC_L, CRC_H;
  uint16_t wcrc = 0XFFFF;
  uint8_t temp;
  uint32_t i = 0, j = 0;

  for (i = 0; i < len; i ++) {
    temp = *buffer & 0X00FF;
    buffer ++;
    wcrc ^= temp;
    for (j = 0; j < 8; j++) {
      if (wcrc & 0X0001) {
        wcrc >>= 1;
        wcrc ^= 0XA001;
      } else {
        wcrc >>= 1;
      }
    }
  }
//  CRC_L = wcrc & 0xff;
//  CRC_H = wcrc >> 8;
  ret[0] = wcrc & 0xff;
  ret[1] = wcrc >> 8;
//  return (((uint16_t)CRC_L << 8) | CRC_H);
}
