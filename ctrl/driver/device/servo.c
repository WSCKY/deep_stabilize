/*
 * servo.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "servo.h"

void servo_init(const servo_handle_t *hsrv)
{

}

status_t servo_detect(const servo_handle_t *hsrv)
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

status_t servo_get_position(const servo_handle_t *hsrv, uint32_t *val)
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

status_t servo_set_current(const servo_handle_t *hsrv, uint32_t val)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x000D, val * 100);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_speed_up_time(const servo_handle_t *hsrv, uint16_t time)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x0098, time);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_speed_down_time(const servo_handle_t *hsrv, uint16_t time)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x0099, time);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_set_speed(const servo_handle_t *hsrv, uint16_t val) // default speed is 300r/min
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x009A, val);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_get_speed(const servo_handle_t *hsrv, uint16_t *val)
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

status_t servo_set_inching(const servo_handle_t *hsrv, servo_inching_t cmd)
{
  status_t ret;
  uint16_t val = 0;
  if(cmd.dir) val |= (1 << 15);
  val |= (cmd.speed & 0x1FF) << 6;
  if(cmd.stop) val |= (1 << 5);
  if(cmd.cmd) val |= 1;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x00CA, val);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_time(const servo_handle_t *hsrv, int32_t time)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = time & 0xFFFF;
  buf[1] = (time >> 16) & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00CC, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_pulse(const servo_handle_t *hsrv, int32_t pulse)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = pulse & 0xFFFF;
  buf[1] = (pulse >> 16) & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00CE, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_run_position(const servo_handle_t *hsrv, int32_t pos)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = pos & 0xFFFF;
  buf[1] = (pos >> 16) & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00D0, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_set_position(const servo_handle_t *hsrv, int32_t pos)
{
  status_t ret;
  uint16_t buf[2];
  buf[0] = pos & 0xFFFF;
  buf[1] = (pos >> 16) & 0xFFFF;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_multi(hsrv->hrtu, hsrv->addr, 0x00D2, buf, 2);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}

status_t servo_set_release(const servo_handle_t *hsrv, uint32_t sta)
{
  status_t ret;
  if(rtu_req_grant(hsrv->hrtu) == 0) return status_timeout;
  ret = rtu_write_single(hsrv->hrtu, hsrv->addr, 0x00D4, sta);
  if(ret != status_ok) goto exit;
exit:
  rtu_rel_grant(hsrv->hrtu);
  return ret;
}
