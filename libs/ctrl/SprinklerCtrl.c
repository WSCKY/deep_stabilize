/*
 * SprinklerCtrl.c
 *
 *  Created on: Mar 9, 2020
 *      Author: kychu
 */

#include "SprinklerCtrl.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "uart.h"
#include "kyLink.h"

#define KYLINK_CACHE_SIZE                      (80)

#define MSG_BOARD_STATE                        0x60
#define MSG_BOARD_CONFIG                       0x61

#define ENCODER_NUMBER                         2

#define CTRL_LOOP_ENABLE_BIT   0x01000000

typedef struct {
  float AngleRateVal;
  float AngleVal;
} AngleInfo_t;

__PACK_BEGIN typedef struct {
	AngleInfo_t angInfo;
	uint16_t encoder[ENCODER_NUMBER];
	float exp_pitch;
	float exp_yaw;
	uint32_t flags;
} __PACK_END Params_t;

__PACK_BEGIN typedef struct {
  uint8_t mode;
  float pitch;
  float yaw;
  uint32_t flag;
} __PACK_END CtrlInfoDef;

static Params_t devState;
static CtrlInfoDef ctrlInfo;
static KYLINK_CORE_HANDLE kylink_tool;

static pthread_t decode_thread;
static bool_t _should_exit = false;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void decode_task(void);
static status_t update_config(void);
static void kylink_decode_callback(kyLinkBlockDef *pRx);

static int kylink_setup(void);
static status_t kylink_tx_bytes(uint8_t *p, uint32_t l);

int SprinklerCtrl_start(const char *dev, const char *baudrate)
{
  if(uart_open(dev, baudrate) != EXIT_SUCCESS) {
    printf("\e[0;31mfailed to open uart %s.\e[0m\n", dev);
    return -1;
  }
  if(kylink_setup() != 0) {
    printf("\e[0;31mfailed to setup kylink.\e[0m\n");
    return -2;
  }
  memset(&devState, 0, sizeof(Params_t));
  memset(&ctrlInfo, 0, sizeof(CtrlInfoDef));
  if(pthread_create(&decode_thread, NULL, (void *)decode_task, NULL) != 0)
    return -3;
  return 0;
}

int SprinklerCtrl_enable_stabilize(int e)
{
  if(e != 0) ctrlInfo.flag |= CTRL_LOOP_ENABLE_BIT;
  else ctrlInfo.flag &= ~CTRL_LOOP_ENABLE_BIT;
  if(update_config() != status_ok) return -2;
  return 0;
}

int SprinklerCtrl_set_pitch(float pitch)
{
  if(pitch < -15 || pitch > 30) return -1;
  ctrlInfo.pitch = pitch;
  if(update_config() != status_ok) return -2;
  return 0;
}

float SprinklerCtrl_get_pitch(void)
{
  float ret;
  pthread_mutex_lock(&mtx);
  ret = devState.angInfo.AngleVal;
  pthread_mutex_unlock(&mtx);
  return ret;
}

float SprinklerCtrl_get_pitchrate(void)
{
  return devState.angInfo.AngleRateVal;
}

/* from -180 to +180 */
int SprinklerCtrl_set_yaw(float yaw)
{
  if(yaw < -180 || yaw > 180) return -1; // invalid parameter
  ctrlInfo.yaw = yaw;
  if(update_config() != status_ok) return -2;
  return 0;
}

int SprinklerCtrl_stop(void)
{
  _should_exit = 1;
  pthread_join(decode_thread, NULL);
  uart_close();
  return 0;
}

static status_t update_config(void)
{
  return kylink_send(&kylink_tool, &ctrlInfo, MSG_BOARD_CONFIG, sizeof(CtrlInfoDef));
}

static void decode_task(void)
{
  int recved, cnt = 0;
  uint8_t rx_cache[40];
  memset(rx_cache, 0, 40);
  while(_should_exit != 1) {
    recved = uart_read((char *)rx_cache, 40);
    if(recved > 0) {
      cnt = 0;
      while(recved --) {
        kylink_decode(&kylink_tool, rx_cache[cnt]);
        cnt ++;
      }
    }
  }
}

static void kylink_decode_callback(kyLinkBlockDef *pRx)
{
  if(pRx->msg_id == MSG_BOARD_STATE) {
    pthread_mutex_lock(&mtx);
    devState = *(Params_t *)pRx->buffer;
    pthread_mutex_unlock(&mtx);
  }
}

static int kylink_setup(void)
{
  kyLinkConfig_t cfg;
  cfg.cache_size = KYLINK_CACHE_SIZE;
  cfg.decoder_cache = (uint8_t *)malloc(KYLINK_CACHE_SIZE);
  if(cfg.decoder_cache == NULL) return -1;
  cfg.callback = kylink_decode_callback;
  cfg.txfunc = kylink_tx_bytes;
  kylink_init(&kylink_tool, &cfg);
  return 0;
}

static status_t kylink_tx_bytes(uint8_t *p, uint32_t l)
{
  if(uart_write((char *)p, (size_t)l) < 0)
    return status_error;
  return status_ok;
}
