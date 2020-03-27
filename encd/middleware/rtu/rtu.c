/*
 * rtu.c
 *
 *  Created on: Feb 20, 2020
 *      Author: kychu
 */

#include "rtu.h"

static status_t rtu_checksum(uint8_t *buffer, uint32_t len);
static void cal_crc16(uint8_t *buffer, uint32_t len, uint8_t *ret);

status_t rtu_read_single(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr)
{
  status_t ret;
  uint32_t cnt = 0;
  if(hrtu->cache_size < 8) return status_nomem; // at least 8 bytes
  if(hrtu->cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->cache[cnt ++] = dev_addr;
  hrtu->cache[cnt ++] = RTU_CMD_READ_SINGLE;
  hrtu->cache[cnt ++] = reg_addr >> 8;
  hrtu->cache[cnt ++] = reg_addr;
  hrtu->cache[cnt ++] = 0x00;
  hrtu->cache[cnt ++] = 0x01;
  cal_crc16(hrtu->cache, cnt, hrtu->cache + cnt);
  ret = hrtu->tx_bytes(hrtu->cache, cnt + 2);
  if(ret != status_ok) return ret;
  ret = hrtu->rx_bytes(hrtu->cache, 7);
  if(ret != status_ok) return ret;
  ret = rtu_checksum(hrtu->cache, 7);
  return ret;
}

status_t rtu_read_multi(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t reg_numb)
{
  status_t ret;
  uint32_t cnt = 0, min_size;
  min_size = 5 + (reg_numb * 2);
  if(min_size < 8) min_size = 8;
  if(hrtu->cache_size < min_size) return status_nomem;
  if(hrtu->cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->cache[cnt ++] = dev_addr;
  hrtu->cache[cnt ++] = RTU_CMD_READ_MULTI;
  hrtu->cache[cnt ++] = reg_addr >> 8;
  hrtu->cache[cnt ++] = reg_addr;
  hrtu->cache[cnt ++] = reg_numb >> 8;
  hrtu->cache[cnt ++] = reg_numb;
  cal_crc16(hrtu->cache, cnt, hrtu->cache + cnt);
  ret = hrtu->tx_bytes(hrtu->cache, cnt + 2);
  if(ret != status_ok) return ret;
  ret = hrtu->rx_bytes(hrtu->cache, 5 + (reg_numb * 2));
  if(ret != status_ok) return ret;
  ret = rtu_checksum(hrtu->cache, 5 + (reg_numb * 2));
  return ret;
}

status_t rtu_write_single(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t reg_data)
{
  status_t ret;
  uint32_t cnt = 0;
  if(hrtu->cache_size < 8) return status_nomem;
  if(hrtu->cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->cache[cnt ++] = dev_addr;
  hrtu->cache[cnt ++] = RTU_CMD_WRITE_SINGLE;
  hrtu->cache[cnt ++] = reg_addr >> 8;
  hrtu->cache[cnt ++] = reg_addr;
  hrtu->cache[cnt ++] = reg_data >> 8;
  hrtu->cache[cnt ++] = reg_data;
  cal_crc16(hrtu->cache, cnt, hrtu->cache + cnt);
  ret = hrtu->tx_bytes(hrtu->cache, cnt + 2);
  if(ret != status_ok) return ret;
  ret = hrtu->rx_bytes(hrtu->cache, 8);
  if(ret != status_ok) return ret;
  ret = rtu_checksum(hrtu->cache, 8);
  return ret;
}

status_t rtu_write_multi(rtu_handle_t *hrtu, uint8_t dev_addr, uint16_t reg_addr, uint16_t *reg_data, uint16_t size)
{
  status_t ret;
  uint32_t cnt = 0, i;
  if(hrtu->cache_size < (9 + (size * 2))) return status_nomem;
  if(hrtu->cache == NULL) return status_nomem;
  if(hrtu->tx_bytes == NULL) return status_error;
  hrtu->cache[cnt ++] = dev_addr;
  hrtu->cache[cnt ++] = RTU_CMD_WRITE_MULTI;
  hrtu->cache[cnt ++] = reg_addr >> 8;
  hrtu->cache[cnt ++] = reg_addr;
  /* number of registers */
  hrtu->cache[cnt ++] = size >> 8;
  hrtu->cache[cnt ++] = size;
  /* number of bytes */
  hrtu->cache[cnt ++] = size << 1;
  for(i = 0; i < size; i ++) {
    hrtu->cache[cnt ++] = reg_data[i] >> 8;
    hrtu->cache[cnt ++] = reg_data[i];
  }
  cal_crc16(hrtu->cache, cnt, hrtu->cache + cnt);
  ret = hrtu->tx_bytes(hrtu->cache, cnt + 2);
  if(ret != status_ok) return ret;
  ret = hrtu->rx_bytes(hrtu->cache, 8);
  if(ret != status_ok) return ret;
  ret = rtu_checksum(hrtu->cache, 8);
  return ret;
}

static status_t rtu_checksum(uint8_t *buffer, uint32_t len)
{
  uint16_t wcrc = 0XFFFF;
  uint8_t temp;
  uint32_t i = 0, j = 0;

  for (i = 0; i < len - 2; i ++) {
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
  if((*buffer == (wcrc & 0xff)) && (*(buffer + 1) == (wcrc >> 8)))
    return status_ok;
  return status_error;
}

static void cal_crc16(uint8_t *buffer, uint32_t len, uint8_t *ret)
{
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
  ret[0] = wcrc & 0xff;
  ret[1] = wcrc >> 8;
}
