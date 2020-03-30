/*
 * encd_task.c
 *
 *  Created on: Mar 28, 2020
 *      Author: kychu
 */

#include "main_task.h"

#include "cpu_utils.h"

#if CONFIG_LOG_ENABLE
static const char* TAG = "ENCD";
#endif /* CONFIG_LOG_ENABLE */

float encoder_value = 0;

/*
 * keep encd_task task handle
 */
static osThreadId encdThread;

static void encd_task_notify(void);

#define ENCD_LOOP_PERIOD_MS            (10)

void encd_task(void const *arg)
{
  int16_t encoder_rd;
  uint32_t time_now, log_ts = 0;

  encdThread = xTaskGetCurrentTaskHandle();

  encoder3_init();

  /* initialize task sync timer */
  tim7_init(ENCD_LOOP_PERIOD_MS, encd_task_notify);

  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      encoder_rd = encoder3_read();
      encoder_value = encoder_rd;
      time_now = xTaskGetTickCountFromISR();
      if(time_now - log_ts >= 1000) {
        log_ts = time_now;
        ky_info(TAG, "counter:%d   %u%%", encoder_rd, osGetCPUUsage());
      }
    }
  }
}

static void encd_task_notify(void)
{
  osSignalSet(encdThread, 0x00000001);
}
