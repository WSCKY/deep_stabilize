/*
 * stat_task.c
 *
 *  Created on: Mar 4, 2020
 *      Author: kychu
 */

#include "main_task.h"
#include "parameter.h"
#include "encoder.h"

#if CONFIG_LOG_ENABLE
static const char* TAG = "STAT";
#endif /* CONFIG_LOG_ENABLE */

#define RS485_RTU_CACHE_SIZE   16

osSemaphoreDef(SEM_ENC_RTU);

void stat_task(void const *arg)
{
  uint16_t encoder_val;
  encoder_handle_t *encoder;
  rtu_handle_t *rtu_rs485_1;

  if(rs485_1_init() != status_ok) {
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "rs485 init failed.");
#endif /* CONFIG_LOG_ENABLE */
  }

  rtu_rs485_1 = kmm_alloc(sizeof(rtu_handle_t));
  encoder = kmm_alloc(sizeof(encoder_handle_t));

  rtu_rs485_1->cache_size = 0;
  rtu_rs485_1->cache = kmm_alloc(RS485_RTU_CACHE_SIZE);
  rtu_rs485_1->sync_obj = osSemaphoreCreate(osSemaphore(SEM_ENC_RTU), 1);

  if(encoder == NULL || \
     rtu_rs485_1 == NULL || \
	 rtu_rs485_1->cache == NULL || \
	 rtu_rs485_1->sync_obj == NULL) {
#if CONFIG_LOG_ENABLE
    ky_info(TAG, "task start failed");
#endif /* CONFIG_LOG_ENABLE */
    kmm_free(encoder);
    if(rtu_rs485_1 != NULL) {
      kmm_free(rtu_rs485_1->cache);
      osSemaphoreDelete(rtu_rs485_1->sync_obj);
    }
    kmm_free(rtu_rs485_1);
    vTaskDelete(NULL);
  }

  rtu_rs485_1->tx_bytes = rs485_1_tx;
  rtu_rs485_1->rx_bytes = rs485_1_rx;
  rtu_rs485_1->cache_size = RS485_RTU_CACHE_SIZE;

  encoder->addr = 0x00;
  encoder->hrtu = rtu_rs485_1;

  for(;;) {
    delay(20);
    encoder->addr ++;
    if(encoder_read(encoder, &encoder_val) != status_ok) {
#if CONFIG_LOG_ENABLE
      ky_warn(TAG, "e%d read fail", encoder->addr);
#endif /* CONFIG_LOG_ENABLE */
    } else {
      param_set_encval(encoder_val, encoder->addr - 1);
#if CONFIG_LOG_ENABLE
//      ky_info(TAG, "e1: %05d", val);
#endif /* CONFIG_LOG_ENABLE */
    }
    if(encoder->addr >= ENCODER_NUMBER) encoder->addr = 0;
  }
}
