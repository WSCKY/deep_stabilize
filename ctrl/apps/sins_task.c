/*
 * sins_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

#include "parameter.h"

static KYLINK_CORE_HANDLE *kylink_sins;

#define SINS_DECODER_CACHE_SIZE                (12)

static void sins_decode_callback(kyLinkBlockDef *pRx);

void sins_task(void const *arg)
{
  kyLinkConfig_t *cfg = NULL;
  uint8_t *sins_decoder_cache;

  uint8_t rcache[16];
  uint32_t rx_len, cnt;

  if(uart3_init() != status_ok) {
    vTaskDelete(NULL);
  }

  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  kylink_sins = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  sins_decoder_cache = kmm_alloc(SINS_DECODER_CACHE_SIZE);
  if(cfg == NULL || kylink_sins == NULL || sins_decoder_cache == NULL) {
    kmm_free(cfg);
    kmm_free(kylink_sins);
    kmm_free(sins_decoder_cache);
    vTaskDelete(NULL);
  }

  cfg->txfunc = kyNULL;
  cfg->callback = sins_decode_callback;
  cfg->decoder_cache = sins_decoder_cache;
  cfg->cache_size = SINS_DECODER_CACHE_SIZE;
  kylink_init(kylink_sins, cfg);

  kmm_free(cfg);
  for(;;) {
    delay(2);
    do {
      rx_len = uart3_pullBytes(rcache, 16);
      for(cnt = 0; cnt < rx_len; cnt ++) {
        kylink_decode(kylink_sins, rcache[cnt]);
      }
    } while(rx_len != 0);
  }
}

static void sins_decode_callback(kyLinkBlockDef *pRx)
{
  if(pRx->msg_id == 0x17) {
    param_set_anginfo((AngleInfo_t *)pRx->buffer);
  }
}
