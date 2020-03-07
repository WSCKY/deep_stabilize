/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"
#include "parameter.h"
#include "maths.h"
#include "servo.h"
#include <math.h>

#if CONFIG_LOG_ENABLE
static const char* TAG = "CTRL";
#endif /* CONFIG_LOG_ENABLE */

#define CTRL_LOOP_PERIOD_MS    (2)

#define OUTPUT_LIMIT           8000

static osThreadId ctrlThread;

static void ctrl_task_notify(void);

void ctrl_task(void const *arg)
{
  PID *pid;
  AngleInfo_t AngleInfo;
  float angle = 0, exp_ang;

  ctrlThread = xTaskGetCurrentTaskHandle();

  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  pid = kmm_alloc(sizeof(PID));
  if(tim7_init(CTRL_LOOP_PERIOD_MS, ctrl_task_notify) != status_ok || pid == NULL) {
    kmm_alloc(pid);
    vTaskDelete(NULL);
  }

  pid->kp = 2500;
  pid->ki = 000;
  pid->kd = 0;
  pid->I_max = 3000;
  pid->I_sum = 0;
  pid->dt = CTRL_LOOP_PERIOD_MS * 0.001f;
  pid->D_max = 100;

  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      param_get_anginfo(&AngleInfo);
      param_get_exppit(&exp_ang);

      if(ABS(exp_ang - AngleInfo.AngleVal) < 0.5f)
        angle = exp_ang;
      else
        angle = AngleInfo.AngleVal;
      pid_loop(pid, exp_ang, angle);
//        pid->Output += ((0 - angle) * 300 - AngleInfo.AngleRateVal) * 1.0f;
      pid->Output = pid->Output - AngleInfo.AngleRateVal * 40.6667f;
      pid->Output = LIMIT(pid->Output, OUTPUT_LIMIT, -OUTPUT_LIMIT);
      pwm16_period((uint32_t)ABS(pid->Output));
      if(pid->Output < 0) {
        output_port_set(IO_OUTPUT2);
      } else {
        output_port_clear(IO_OUTPUT2);
      }
    } else {
#if CONFIG_LOG_ENABLE
      ky_err(TAG, "wait notify error");
#endif /* CONFIG_LOG_ENABLE */
    }
  }
}

static void ctrl_task_notify(void)
{
  osSignalSet(ctrlThread, 0x00000001);
}
