#include "main_task.h"
#include "parameter.h"

static KYLINK_CORE_HANDLE *kylink_uart;
//static KYLINK_CORE_HANDLE *CDC_PortHandle;

#define UART_DECODER_CACHE_SIZE                (88)

#define MSG_UPGRADE_REQUEST                    0x80
#define MSG_BOARD_STATE                        0x60
#define MSG_PITCH_CONFIG                       0x61

__PACK_BEGIN typedef struct {
  uint8_t mode;
  float pitch;
  uint32_t flag;
} __PACK_END CtrlInfoDef;

static void com_decode_callback(kyLinkBlockDef *pRx);

void com_task(void const *arg)
{
  kyLinkConfig_t *cfg = NULL;
  Params_t *param = NULL;
  uint8_t *uart_decoder_cache;

  uint8_t rcache[16];
  uint32_t rx_len, cnt;
#if (USER_LOG_PORT != 2)
  uint32_t tx_period_div = 0;
#endif /* (USER_LOG_PORT != 2) */

  if(uart2_init() != status_ok) {
    vTaskDelete(NULL);
  }

  param = kmm_alloc(sizeof(Params_t));
  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  kylink_uart = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  uart_decoder_cache = kmm_alloc(UART_DECODER_CACHE_SIZE);
  if(param == NULL || cfg == NULL || kylink_uart == NULL || uart_decoder_cache == NULL) {
    kmm_free(param);
    kmm_free(cfg);
    kmm_free(kylink_uart);
    kmm_free(uart_decoder_cache);
    vTaskDelete(NULL);
  }

  cfg->txfunc = uart2_TxBytesDMA;
  cfg->callback = com_decode_callback;
  cfg->decoder_cache = uart_decoder_cache;
  cfg->cache_size = UART_DECODER_CACHE_SIZE;
  kylink_init(kylink_uart, cfg);

  kmm_free(cfg);

  for(;;) {
    delay(10);
	while((rx_len = uart2_pullBytes(rcache, 16)) > 0) {
      for(cnt = 0; cnt < rx_len; cnt ++) {
        kylink_decode(kylink_uart, rcache[cnt]);
      }
	}
#if (USER_LOG_PORT != 2)
	tx_period_div ++;
	if(tx_period_div == 5) {
      tx_period_div = 0;

      // prepare data
      param_get_param(param);
      // send message
      kylink_send(kylink_uart, param, MSG_BOARD_STATE, sizeof(Params_t));
	}
#endif /* (USER_LOG_PORT != 2) */
  }
}

static void com_decode_callback(kyLinkBlockDef *pRx)
{
  switch(pRx->msg_id) {
    case MSG_PITCH_CONFIG:
    {
      CtrlInfoDef *p = (CtrlInfoDef *)pRx->buffer;
      if(p->mode == 1) {
        param_set_exppit(p->pitch);
      }
    }
    break;
    case MSG_UPGRADE_REQUEST:
      if(pRx->length == 16) {
        __disable_irq();
        NVIC_SystemReset();
      }
    break;
    default: break;
  }
}
