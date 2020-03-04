/*
 * ctrl_task.c
 *
 *  Created on: Feb 23, 2020
 *      Author: kychu
 */

#include "main_task.h"
#include "maths.h"
#include "servo.h"
#include <math.h>

#if CONFIG_LOG_ENABLE
//static const char* TAG = "CTRL";
#endif /* CONFIG_LOG_ENABLE */

//float exp_angle = 0.0f;
extern AngleInfo_t AngleInfo;

/* extern rtu_handle_t rtu_rs485_1;

static const servo_handle_t motor_pitch = {
  .addr = 0x01,
  .hrtu = &rtu_rs485_1,
}; */

void ctrl_task(void const *arg)
{
  PID *pid;
  float angle = 0;
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
//        if(ABS(0 - AngleInfo.AngleVal) < 0.5f)
//        	angle = 0.0f;
//        else
        	angle = AngleInfo.AngleVal;
        pid_loop(pid, 0, angle);
//        pid->Output += ((0 - angle) * 300 - AngleInfo.AngleRateVal) * 1.0f;
        pid->Output = pid->Output - AngleInfo.AngleRateVal * 40.6667f;
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
