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
//static const char* TAG = "CTRL";
#endif /* CONFIG_LOG_ENABLE */

#define OUTPUT_LIMIT           8000

//float exp_angle = 0.0f;

/* extern rtu_handle_t rtu_rs485_1;

static const servo_handle_t motor_pitch = {
  .addr = 0x01,
  .hrtu = &rtu_rs485_1,
}; */

void ctrl_task(void const *arg)
{
  PID *pid;
  AngleInfo_t AngleInfo;
  float angle = 0, exp_ang;
//  uint32_t task_period_div = 0;
  pwm16_init();
  pwm17_init();
  delay(1000); // wait motor driver ready.

  pid = kmm_alloc(sizeof(PID));
  if(tim7_init(2) != status_ok || pid == NULL) {
    vTaskDelete(NULL);
  }

  pid->kp = 2500;
  pid->ki = 000;
  pid->kd = 0;
  pid->I_max = 3000;
  pid->I_sum = 0;
  pid->dt = 0.005;
  pid->D_max = 100;

  for(;;) {
    if(tim7_check_update(100) == status_ok) {
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
//        pwm16_period((int)(1200 * fabs((AngleInfo.AngleVal - 0))));
      if(pid->Output < 0) {
        output_port_set(IO_OUTPUT2);
      } else {
        output_port_clear(IO_OUTPUT2);
      }
    }
  }
}
