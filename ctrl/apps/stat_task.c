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
  uint8_t gpio_in, gpio_msk;
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
    if(param_get_flag_bit(ENCODER_CAL_BIT) == false) {
      delay(10);
      // read encoder
      encoder->addr ++;
      if(encoder_read(encoder, &encoder_val) != status_ok) {
#if CONFIG_LOG_ENABLE
        ky_warn(TAG, "e%d read fail", encoder->addr);
#endif /* CONFIG_LOG_ENABLE */
        param_set_flag_bit(1 << (encoder->addr - 1 + ENCODER_ERROR_BIT_OFF));
      } else {
        param_set_encval(encoder_val, encoder->addr - 1);
//#if CONFIG_LOG_ENABLE
//      ky_info(TAG, "e1: %05d", val);
//#endif /* CONFIG_LOG_ENABLE */
      }
      if(encoder->addr >= ENCODER_NUMBER) encoder->addr = 0;
    } else { // origin reset
#if CONFIG_LOG_ENABLE
      ky_warn(TAG, "encoder calibrate start!");
#endif /* CONFIG_LOG_ENABLE */
      for(int i = 0; i < ENCODER_NUMBER; i ++) {
        delay(10);
        encoder->addr = i + 1;
        encoder_origin(encoder);
#if CONFIG_LOG_ENABLE
        ky_warn(TAG, "e%d RESET DONE!", encoder->addr);
#endif /* CONFIG_LOG_ENABLE */
        param_set_encval(0, encoder->addr - 1);
      }
      param_set_exppit(0);
      param_set_expyaw(0);
      param_sav_flash();
      encoder->addr = 0x00;
      param_clr_flag_bit(ENCODER_CAL_BIT);
    }

    // read GPIO input
    gpio_in = 0;
    gpio_msk = 1;
    for(int i = 0; i < INPUT_IO_NUMBER; i ++) {
      if(input_port_read(i) != 0)
        gpio_in |= gpio_msk;
      gpio_msk <<= 1;
    }
    param_cfg_flag_bits(0xFFFFFF00, gpio_in);
  }
}
