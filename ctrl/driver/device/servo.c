/*
 * servo.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "servo.h"

void servo_init(servo_handle_t *hsrv)
{

}

status_t servo_detect(servo_handle_t *hsrv)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_read_multi(hsrv->hrtu, hsrv->addr, 0x0000, 2);
  if(ret != status_ok) goto exit;
  if(hsrv->hrtu->cache[3] == 0x56 && hsrv->hrtu->cache[4] == 0x78 &&
     hsrv->hrtu->cache[5] == 0x12 && hsrv->hrtu->cache[6] == 0x34)
    ret = status_ok;
  else
    ret = status_error;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_get_position(servo_handle_t *hsrv, uint32_t *val)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_read_multi(hsrv->hrtu, hsrv->addr, 0x0004, 2);
  if(ret != status_ok) goto exit;
  *val = (((uint16_t)hsrv->hrtu->cache[3] << 8) | hsrv->hrtu->cache[4]) |
         ((uint32_t)(((uint16_t)hsrv->hrtu->cache[5] << 8) | hsrv->hrtu->cache[6]) << 16);
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_speed_up_time(servo_handle_t *hsrv, uint16_t time)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x0098, time);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_speed_down_time(servo_handle_t *hsrv, uint16_t time)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x0099, time);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_set_speed(servo_handle_t *hsrv, uint16_t val) // default speed is 300r/min
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x009A, val);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_get_speed(servo_handle_t *hsrv, uint16_t *val)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_read_single(hsrv->hrtu, hsrv->addr, 0x009A);
  if(ret != status_ok) goto exit;
  *val = ((uint16_t)hsrv->hrtu->cache[3] << 8) | hsrv->hrtu->cache[4];
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_time(servo_handle_t *hsrv, int32_t time)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = (time >> 16) & 0xFFFF;
  buf[1] = time & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00CC, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_pulse(servo_handle_t *hsrv, int32_t pulse)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = (pulse >> 16) & 0xFFFF;
  buf[1] = pulse & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00CE, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_position(servo_handle_t *hsrv, int32_t pos)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = (pos >> 16) & 0xFFFF;
  buf[1] = pos & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00D0, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_set_position(servo_handle_t *hsrv, int32_t pos)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = (pos >> 16) & 0xFFFF;
  buf[1] = pos & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00D2, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}
