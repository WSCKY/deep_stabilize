#include "main_task.h"

static KYLINK_CORE_HANDLE *kylink_uart;
//static KYLINK_CORE_HANDLE *CDC_PortHandle;

#define UART_DECODER_CACHE_SIZE                (88)
static uint8_t *uart_decoder_cache;

#define MSG_UPGRADE_REQUEST                    0x80
#define MSG_BOARD_STATE                        0x60
#define MSG_PITCH_CONFIG                       0x61

float exp_angle = 0.0f;
extern AngleInfo_t AngleInfo;

__PACK_BEGIN typedef struct {
  uint8_t mode;
  float pitch;
} __PACK_END CtrlInfoDef;

__PACK_BEGIN typedef struct {
  float pitch;
  uint32_t state;
} __PACK_END StateInfoDef;

static void com_decode_callback(kyLinkBlockDef *pRx);

void com_task(void const *arg)
{
  kyLinkConfig_t *cfg = NULL;
  StateInfoDef *msg = NULL;

  uint8_t rcache[16];
  uint32_t rx_len, cnt;
  uint32_t tx_period_div = 0;

  msg = kmm_alloc(sizeof(StateInfoDef));
  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  kylink_uart = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  uart_decoder_cache = kmm_alloc(UART_DECODER_CACHE_SIZE);
  if(msg == NULL || cfg == NULL || kylink_uart == NULL || uart_decoder_cache == NULL) {
    kmm_free(msg);
    kmm_free(cfg);
    kmm_free(kylink_uart);
    kmm_free(uart_decoder_cache);
    vTaskDelete(NULL);
  }

  uart2_init();

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
	tx_period_div ++;
	if(tx_period_div == 5) {
      tx_period_div = 0;

      // prepare data
      msg->pitch = AngleInfo.AngleVal;
      msg->state = 0x00000000;
      // send message
      kylink_send(kylink_uart, msg, MSG_BOARD_STATE, sizeof(StateInfoDef));
	}
  }
}

static void com_decode_callback(kyLinkBlockDef *pRx)
{
  switch(pRx->msg_id) {
    case MSG_PITCH_CONFIG:
    {
      CtrlInfoDef *p = (CtrlInfoDef *)pRx->buffer;
      if(p->mode == 1) {
        exp_angle = p->pitch;
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
