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

static SprinklerCtrl_event_callback event_callback = 0;

static pthread_t decode_thread;
static pthread_t event_proc_thread;
static bool_t _should_exit = false;

//static bool_t _msg_updated_flag = 0;
static pthread_mutex_t ctrlInfo_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t devState_mtx = PTHREAD_MUTEX_INITIALIZER;

static void decode_task(void);
static void event_proc_task(void);
static status_t update_config(void);
static void kylink_decode_callback(kyLinkBlockDef *pRx);

static int kylink_setup(void);
static status_t kylink_tx_bytes(uint8_t *p, uint32_t l);

int SprinklerCtrl_start(const char *dev, SprinklerCtrl_event_callback callback)
{
  if(callback == 0) return -1;
  event_callback = callback;
  if(uart_open(dev, (const char *)"115200") != EXIT_SUCCESS) {
    printf("\e[0;31mfailed to open uart %s.\e[0m\n", dev);
    return -1;
  }
  if(kylink_setup() != 0) {
    printf("\e[0;31mfailed to setup kylink.\e[0m\n");
    return -2;
  }
  memset(&devState, 0, sizeof(Params_t));
  memset(&ctrlInfo, 0, sizeof(CtrlInfoDef));
  ctrlInfo.mode = 1;
  ctrlInfo.flag |= CTRL_LOOP_ENABLE_BIT;
  if(pthread_create(&decode_thread, NULL, (void *)decode_task, NULL) != 0)
    return -3;
  if(pthread_create(&event_proc_thread, NULL, (void *)event_proc_task, NULL) != 0)
    return -3;
  return 0;
}

int SprinklerCtrl_enable_stabilize(int e)
{
  int ret = 0;
  pthread_mutex_lock(&ctrlInfo_mtx);
  if(e != 0) ctrlInfo.flag |= CTRL_LOOP_ENABLE_BIT;
  else ctrlInfo.flag &= ~CTRL_LOOP_ENABLE_BIT;
  if(update_config() != status_ok) ret = -2;
  pthread_mutex_unlock(&ctrlInfo_mtx);
  return ret;
}

int SprinklerCtrl_set_pitch(float pitch)
{
  int ret = 0;
  if(pitch < -15 || pitch > 60) return -1;
  pthread_mutex_lock(&ctrlInfo_mtx);
  ctrlInfo.pitch = pitch;
  if(update_config() != status_ok) ret = -2;
  pthread_mutex_unlock(&ctrlInfo_mtx);
  return ret;
}

/* from -90 to +90 */
int SprinklerCtrl_set_yaw(float yaw)
{
  int ret = 0;
  if(yaw < -90 || yaw > 90) return -1; // invalid parameter

  pthread_mutex_lock(&ctrlInfo_mtx);
  ctrlInfo.yaw = yaw;
  if(update_config() != status_ok) {
    ret = -2;
  }
  pthread_mutex_unlock(&ctrlInfo_mtx);

  return ret;
}

int SprinklerCtrl_set_angle(float pitch, float yaw)
{
  int ret = 0;
  if(pitch < -15 || pitch > 60) return -1;
  if(yaw < -90 || yaw > 90) return -1;
  pthread_mutex_lock(&ctrlInfo_mtx);
  ctrlInfo.pitch = pitch;
  ctrlInfo.yaw = yaw;
  if(update_config() != status_ok) ret = -2;
  pthread_mutex_unlock(&ctrlInfo_mtx);
  return ret;
}

float SprinklerCtrl_get_pitch(void)
{
  float ret;
  pthread_mutex_lock(&devState_mtx);
  ret = devState.encoder[0] * 0.02197265625f;
  pthread_mutex_unlock(&devState_mtx);
  if(ret > 180) ret -= 360;
  ret = -ret;
  return ret;
}

float SprinklerCtrl_get_yaw(void)
{
  float ret;
  pthread_mutex_lock(&devState_mtx);
  ret = devState.encoder[1] * 0.02197265625f;
  pthread_mutex_unlock(&devState_mtx);
  if(ret > 180) ret -= 360;
  ret = -ret;
  return ret;
}

int SprinklerCtrl_get_angle(float *pitch, float *yaw)
{
  float v1, v2;
  pthread_mutex_lock(&devState_mtx);
  v1 = devState.encoder[0] * 0.02197265625f;
  v2 = devState.encoder[1] * 0.02197265625f;
  pthread_mutex_unlock(&devState_mtx);
  if(v1 > 180) v1 -= 360; // (-180, 180]
  v1 = -v1;
  *pitch = v1;
  if(v2 > 180) v2 -= 360; // (-180, 180]
  v2 = -v2;
  *yaw = v2;
  return 0;
}

int SprinklerCtrl_get_pitchencoder(void)
{
  int ret;
  pthread_mutex_lock(&devState_mtx);
  ret = devState.encoder[0];
  pthread_mutex_unlock(&devState_mtx);
  return ret;
}

int SprinklerCtrl_get_yawencoder(void)
{
  int ret;
  pthread_mutex_lock(&devState_mtx);
  ret = devState.encoder[1];
  pthread_mutex_unlock(&devState_mtx);
  return ret;
}

int SprinklerCtrl_stop(void)
{
  _should_exit = 1;
  pthread_join(decode_thread, NULL);
  pthread_join(event_proc_thread, NULL);
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
    pthread_mutex_lock(&devState_mtx);
    devState = *(Params_t *)pRx->buffer;
    pthread_mutex_unlock(&devState_mtx);
//    _msg_updated_flag = 1;
  }
}

#define CTRL_LOST_BIT_OFF      24
#define CTRL_LOST_PIT_BIT      0x01000000
#define CTRL_LOST_YAW_BIT      0x02000000

#define ENCODER_ERROR_BIT_OFF  26
#define ENCODER_ERROR_BIT_1    0x04000000
#define ENCODER_ERROR_BIT_2    0x08000000

#define CTRL_ADJ_RUN_BIT_OFF   28
#define CTRL_ADJ_RUN_PIT_BIT   0x10000000
#define CTRL_ADJ_RUN_YAW_BIT   0x20000000
#define CTRL_ADJ_RUN_ALL_BIT   0x30000000

static void event_proc_task(void)
{
  uint32_t last_flag = 0, current_flag = 0;
  printf("\e[0;31mevent proc task start.\e[0m\n");
  if(event_callback == 0) return;
  printf("\e[0;31mevent process start.\e[0m\n");
  while(_should_exit != 1) {
    usleep(10000);
//    if(_msg_updated_flag == 1) {
//      _msg_updated_flag = 0;
      pthread_mutex_lock(&devState_mtx);
      current_flag = devState.flags;
      pthread_mutex_unlock(&devState_mtx);
      if((current_flag & CTRL_LOST_PIT_BIT) == CTRL_LOST_PIT_BIT) {
        event_callback(pitch_lost_control);
      }
      if((current_flag & CTRL_LOST_YAW_BIT) == CTRL_LOST_YAW_BIT) {
        event_callback(yaw_lost_control);
      }
      if((current_flag & ENCODER_ERROR_BIT_1) == ENCODER_ERROR_BIT_1) {
        event_callback(pitch_encoder_error);
      }
      if((current_flag & ENCODER_ERROR_BIT_2) == ENCODER_ERROR_BIT_2) {
        event_callback(yaw_encoder_error);
      }
      if(((last_flag & CTRL_ADJ_RUN_ALL_BIT) != 0) && ((current_flag & CTRL_ADJ_RUN_ALL_BIT) == 0)) {
        printf("\e[0;31madj done event trig\e[0m\n");
        event_callback(sprinkler_adj_done);
      }
      last_flag = current_flag;
//    }
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
