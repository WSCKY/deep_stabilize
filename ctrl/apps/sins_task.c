/*
 * sins_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"

#include "parameter.h"

#if CONFIG_LOG_ENABLE
static const char* TAG = "SINS";
#endif /* CONFIG_LOG_ENABLE */

#if CONFIG_USE_BOARD_IMU
#include "icm20602.h"
#include <string.h>
#include <math.h>
#include "maths.h"
#include "cpu_utils.h"

#define FUSION_ALGORITHM_USE_QUATERNION (0)

typedef struct {
  IMU_RAW_6DOF raw;
  IMU_UNIT_6DOF unit;
  _3AxisUnit gyroff;
  IMU_6DOF_T imu_info;
#if FUSION_ALGORITHM_USE_QUATERNION
  Quat_T est_q;
  Euler_T est_e;
#else
  kf_handle_t kf_pitch;
#endif /* FUSION_ALGORITHM_USE_QUATERNION */
} SINS_INFO;

static int imu_peace_check(_3AxisUnit *gyr, _3AxisUnit *off);

void sins_task(void const *arg)
{
  SINS_INFO *sins_info;
  uint32_t time_now, log_ts = 0;

  sins_info = kmm_alloc(sizeof(SINS_INFO));
  if(sins_info == NULL) {
    ky_err(TAG, "no memory to start SINS");
    kmm_free(sins_info);
    vTaskDelete(NULL);
  }

  if(icm20602_init() != status_ok) {
    ky_err(TAG, "IMU init failed");
    vTaskDelete(NULL);
  }

  memset(sins_info, 0, sizeof(SINS_INFO));
#if FUSION_ALGORITHM_USE_QUATERNION
  sins_info->est_q.qw = 1.0f;
  sins_info->est_q.qx = 0.0f;
  sins_info->est_q.qy = 0.0f;
  sins_info->est_q.qz = 0.0f;
#else
  kalman_filter_init(&sins_info->kf_pitch, 0.001f, 0.003f, 0.5f, 0.002f);
#endif /* FUSION_ALGORITHM_USE_QUATERNION */
#if CONFIG_LOG_ENABLE
  ky_info(TAG, "keep IMU motionless");
#endif /* CONFIG_LOG_ENABLE */
  for(;;) {
    if(icm20602_read(&sins_info->raw, &sins_info->unit, osWaitForever) == status_ok) {
      if(imu_peace_check(&sins_info->unit.Gyr, &sins_info->gyroff) != 0) {
        ky_info(TAG, "got GYR offset: x:%d, y:%d, z:%d",
                (int)(sins_info->gyroff.X*1000), (int)(sins_info->gyroff.Y*1000), (int)(sins_info->gyroff.Z*1000));
        break;
      }
    }
  }
  for(;;) {
    if(icm20602_read(&sins_info->raw, &sins_info->unit, osWaitForever) == status_ok) {
      sins_info->imu_info.ax = sins_info->unit.Acc.X;
      sins_info->imu_info.ay = sins_info->unit.Acc.Y;
      sins_info->imu_info.az = sins_info->unit.Acc.Z;
      sins_info->imu_info.gx = sins_info->unit.Gyr.X - sins_info->gyroff.X;
      sins_info->imu_info.gy = sins_info->unit.Gyr.Y - sins_info->gyroff.Y;
      sins_info->imu_info.gz = sins_info->unit.Gyr.Z - sins_info->gyroff.Z;
#if FUSION_ALGORITHM_USE_QUATERNION
      fusionQ_6dot(&sins_info->imu_info, &sins_info->est_q, 3.0f, 0, 0.002f);
      Quat2Euler(&sins_info->est_q, &sins_info->est_e);
      sins_info->est_e.roll  = atan2f(2 * (sins_info->est_q.qw * sins_info->est_q.qx + sins_info->est_q.qy * sins_info->est_q.qz) , 1 - 2 * (sins_info->est_q.qx * sins_info->est_q.qx + sins_info->est_q.qy * sins_info->est_q.qy))*RAD_TO_DEG;  //+-90
      sins_info->est_e.pitch = asinf(2 * (sins_info->est_q.qw * sins_info->est_q.qy - sins_info->est_q.qz * sins_info->est_q.qx))*RAD_TO_DEG;                                 //+-180
      sins_info->est_e.yaw   = atan2f(2 * (sins_info->est_q.qw * sins_info->est_q.qz + sins_info->est_q.qx * sins_info->est_q.qy) , 1 - 2 * (sins_info->est_q.qy * sins_info->est_q.qy + sins_info->est_q.qz * sins_info->est_q.qz))*RAD_TO_DEG;  //+-180
#else
      kalman_filter_fusion(&sins_info->kf_pitch, atan2f(sins_info->imu_info.ax, sins_info->imu_info.az) * RAD_TO_DEG, sins_info->imu_info.gx);
#endif /* FUSION_ALGORITHM_USE_QUATERNION */
    }
    time_now = xTaskGetTickCountFromISR();
    if(time_now - log_ts >= 1000) {
      log_ts = time_now;
//      ky_info(TAG, "p:%d, r:%d  %u%%", (int)(sins_info->est_e.pitch), (int)(sins_info->est_e.roll), osGetCPUUsage());
      ky_info(TAG, "p:%d, r:%d  %u%%", (int)(sins_info->kf_pitch.angle), (int)(sins_info->kf_pitch.angle_dot), osGetCPUUsage());
    }
  }
}

#define GYR_PEACE_THRESHOLD                      (0.2f) /* unit: dps */

//static _3AxisUnit last_gyr = {0};
static int imu_peace_check(_3AxisUnit *gyr, _3AxisUnit *off)
{
  static uint32_t peace_ts = 0;
  static uint32_t peace_tn = 0;
  static uint32_t gyr_peace_flag = 0;

  peace_tn = xTaskGetTickCount();
  if((fabs(gyr->X - off->X) < GYR_PEACE_THRESHOLD) &&
     (fabs(gyr->Y - off->Y) < GYR_PEACE_THRESHOLD) &&
     (fabs(gyr->Z - off->Z) < GYR_PEACE_THRESHOLD)) {
    if(peace_ts == 0) peace_ts = peace_tn;
    if(peace_tn - peace_ts > configTICK_RATE_HZ) { // keep 1s.
      if(gyr_peace_flag == 0) {
        gyr_peace_flag = 1;
      }
    }
  } else {
    gyr_peace_flag = 0;
    peace_ts = peace_tn;
  }

  /* update the last data */
  off->X = gyr->X;
  off->Y = gyr->Y;
  off->Z = gyr->Z;
  return gyr_peace_flag;
}

#else
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
#if CONFIG_LOG_ENABLE
    ky_err(TAG, "no memory to read IMU");
#endif /* CONFIG_LOG_ENABLE */
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
#endif /* CONFIG_USE_BOARD_IMU */
