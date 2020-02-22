/*
 * encoder.c
 *
 *  Created on: Feb 22, 2020
 *      Author: kychu
 */

#include "encoder.h"

#define REG_ENCODER_VALUE      0x0000
#define REG_BAUDRATE_VALUE     0x0003
#define REG_CONFIG_ORIGIN      0x000D

void encoder_init(encoder_handle_t *henc)
{

}

status_t encoder_read(encoder_handle_t *henc, uint16_t *val)
{
  status_t ret;
  if(rtu_req_grant(henc->hrtu) == 0) return status_timeout;
  ret = rtu_read_multi(henc->hrtu, henc->addr, REG_ENCODER_VALUE, 1);
  if(ret != status_ok) goto exit;
  // process the data in cache.
  *val = ((uint16_t)henc->hrtu->cache[3] << 8) | henc->hrtu->cache[4];
exit:
  rtu_rel_grant(henc->hrtu);
  return ret;
}

status_t encoder_baudrate(encoder_handle_t *henc, uint32_t baudrate)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = (baudrate >> 16) & 0xFFFF;
  buf[1] = baudrate & 0xFFFF;
  if(rtu_req_grant(henc->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(henc->hrtu, henc->addr, REG_BAUDRATE_VALUE, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(henc->hrtu);
  return ret;
}

status_t encoder_origin(encoder_handle_t *henc)
{
  status_t ret;
  if(rtu_req_grant(henc->hrtu) == 0) return status_timeout;
  // write 0x0001 to set current point as origin
  ret = rtu_write_single(henc->hrtu, henc->addr, REG_CONFIG_ORIGIN, 0x0001);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(henc->hrtu);
  return ret;
}
