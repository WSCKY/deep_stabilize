/*
 * stat_task.c
 *
 *  Created on: Mar 4, 2020
 *      Author: kychu
 */

#include "main_task.h"
#include "encoder.h"

#if CONFIG_LOG_ENABLE
static const char* TAG = "STAT";
#endif /* CONFIG_LOG_ENABLE */

extern rtu_handle_t rtu_rs485_1;
static const encoder_handle_t encoder1 = {
  .addr = 0x01,
  .hrtu = &rtu_rs485_1,
};
static const encoder_handle_t encoder2 = {
  .addr = 0x02,
  .hrtu = &rtu_rs485_1,
};

void stat_task(void const *arg)
{
  uint16_t val1, val2;
  for(;;) {
    delay(200);
    USER_LED_TOG();
    if(encoder_read(&encoder1, &val1) != status_ok) {
#if CONFIG_LOG_ENABLE
      ky_warn(TAG, "e1 read fail");
#endif /* CONFIG_LOG_ENABLE */
      continue;
    }
    delay(200);
    USER_LED_TOG();
    if(encoder_read(&encoder2, &val2) != status_ok) {
#if CONFIG_LOG_ENABLE
      ky_warn(TAG, "e2 read fail");
#endif /* CONFIG_LOG_ENABLE */
      continue;
    }
#if CONFIG_LOG_ENABLE
    ky_info(TAG, "e1: %05d, e2: %05d", val1, val2);
#endif /* CONFIG_LOG_ENABLE */
  }
}
